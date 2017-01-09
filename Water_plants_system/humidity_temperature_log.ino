// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
// RTC PART
#include <Wire.h>
#include "RTClib.h"

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
   #define Serial SerialUSB
#endif

RTC_DS1307 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

DateTime now;

// DHT11
#include <dht11.h>  	// import DHT11 libray
dht11 DHT;   					// class declaration
#define DHT11_PIN 11  // Define a constant to use as DHT pin

// For some weird reason it doesn't work if PIN is 11!!!

String readString;

// SD CARD
#include <SPI.h>
#include <SD.h>

// how many milliseconds between grabbing data and logging it. 1000 ms is once a second
#define LOG_INTERVAL  1000 							// mills between entries (reduce to take more/faster data)
#define SYNC_INTERVAL 600*LOG_INTERVAL 	// mills between calls to flush() - to write data to the card

// how many probes to check the humidity?
#define PROBE_NUMBERS 5
// Define how many millis between next soil humidity check - 6 hour
#define HUMIDITY_CHECK_INTERVAL 21600000 // 6 hour // 60000 // 1 hour // 5000 // 5 sec // 30000 // 30s // 

// Maximum time for the Open Valve - 60s
#define MAX_OPEN_VALVE 300000 // 5 min // 60000 // 60 sec // 300000 // 5 min // 

// Interval between Soil humidity check
#define SOIL_HUMIDITY_INTERVAL 60000 // 10 sec

// Himudity Low limit
#define SOIL_HUMIDITY_LOW_LIMIT 400

// Himudity High limit
#define SOIL_HUMIDITY_HIGH_LIMIT 550

uint32_t syncTime = 0; // time of last sync()

// the digital pins that connect to the LEDs
#define redLEDpin 13

// for the data logging shield, we use digital pin 10 for the SD cs line
const int chipSelect = 10;

// the logging file
File logfile;

// YL-39 + YL-69 humidity sensor
byte humidity_sensor_pin = A1;
byte humidity_sensor_vcc = 6;
int moisture_humidity;

// Solenoid Valve
int ValvePin = 4;                	// Solenoid valve connected to pin 4
int SwPin = 12;
int buttonWas = 0; 								// The state of the switch (pushed = 1, not pushed = 0) last time we looked
int buttonIs = 0; 								// Current state of the switch
int val = 0;

// ECHO_TO_SERIAL 0 - no serial debug / 1 - serial debug
#define ECHO_TO_SERIAL 0

void setup () {
// Setup RTC  
Serial.begin(57600);
Serial.println("Setup STarted");

#if WAIT_TO_START
  Serial.println("Type any character to start");
  while (!Serial.available());
#endif 														//WAIT_TO_START

#ifndef ESP8266
  while (!Serial); 								// for Leonardo/Micro/Zero
#endif

  // Serial.begin(57600);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (30);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  
  // SETUP ERROR
  
  pinMode(redLEDpin, OUTPUT);
  // DHT11
  Serial.println("DHT TEST PROGRAM ");
  Serial.print("LIBRARY VERSION: ");
  Serial.println(DHT11LIB_VERSION);
  Serial.println();
  Serial.println("Humidity (%),\tTemperature (C),\tTemp (Analogica)");
  Serial.println();
  pinMode(DHT11_PIN, OUTPUT);
  
    // initialize the SD card
  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    error("Card failed, or not present");
  }
  else { Serial.println("card initialized."); }
  
  // create a new file
  char filename[] = "LOGGER00.CSV";
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = i/10 + '0';
    filename[7] = i%10 + '0';
    if (! SD.exists(filename)) {
      // only open a new file if it doesn't exist
      logfile = SD.open(filename, FILE_WRITE); 
      break;  // leave the loop!
    }
  }
  
  if (! logfile) {
    error("couldnt create file");
  }
  
  Serial.print("Logging to: ");
  Serial.println(filename);
 
   logfile.println("stamp,datetime,humidity,temp,soil_humidity");    
  //if ECHO_TO_SERIAL
    Serial.println("stamp,datetime,humidity,temp,soil_humidity");
  //endif //ECHO_TO_SERIAL

  // Init the humidity sensor board
  pinMode(humidity_sensor_vcc, OUTPUT);
  digitalWrite(humidity_sensor_vcc, LOW);
  
  // Valve setup
  pinMode(ValvePin, OUTPUT);      					// sets the digital pin as output
  pinMode(SwPin, INPUT);
  buttonIs = digitalRead(SwPin); 						// Read the initial state of the switch!
  
  // Setup OVER
  Serial.println("Setup finished");
  
}
 
void loop () {
  //closeValve();
  //temphumi();
  
  //DateTime now;
  
  float moisture_humidity_avg = 0;
  
  // Moisture sensor calibration
  for ( uint8_t i=0 ; i<PROBE_NUMBERS ; i++) {
    // log milliseconds since starting
    uint32_t m = millis();
    
    // logfile.print(m);           				// milliseconds since start
    // logfile.print(", ");    
    #if ECHO_TO_SERIAL 
      Serial.print(m);         						// milliseconds since start
      Serial.print(", ");  
    #endif
    
    rtcnow();
    
    //blink();
    //error("test");
    //temphumi();
    moisture_humidity = read_humidity_sensor();
    
    #if ECHO_TO_SERIAL
      Serial.print("Soil Humidity Level (0-1023): ");
      Serial.println(moisture_humidity); 
    #endif
    
    // delay for the amount of time we want between readings
    delay((LOG_INTERVAL -1) - (millis() % LOG_INTERVAL));
  }
  
  // log time
    logfile.print(now.unixtime()); 				// seconds since 1/1/1970
    logfile.print(", ");
    logfile.print('"');
    logfile.print(now.year(), DEC);
    logfile.print("/");
    logfile.print(now.month(), DEC);
    logfile.print("/");
    logfile.print(now.day(), DEC);
    logfile.print(" ");
    logfile.print(now.hour(), DEC);
    logfile.print(":");
    logfile.print(now.minute(), DEC);
    logfile.print(":");
    logfile.print(now.second(), DEC);
    logfile.print('"');
    logfile.print(", ");
    logfile.print(DHT.humidity, 1);
    logfile.print(", ");
    logfile.print(DHT.temperature, 1);
    logfile.print(", ");
    logfile.print(moisture_humidity); logfile.print("\n");
 
  // Log to serial
    #if ECHO_TO_SERIAL
      Serial.print(now.unixtime()); 					// seconds since 1/1/1970
      Serial.print(", ");
      Serial.print('"');
      Serial.print(now.year(), DEC);
      Serial.print("/");
      Serial.print(now.month(), DEC);
      Serial.print("/");
      Serial.print(now.day(), DEC);
      Serial.print(" ");
      Serial.print(now.hour(), DEC);
      Serial.print(":");
      Serial.print(now.minute(), DEC);
      Serial.print(":");
      Serial.print(now.second(), DEC);
      Serial.print('"');
      Serial.print(", ");
      Serial.print(DHT.humidity, 1);
      Serial.print(", ");
      Serial.print(DHT.temperature, 1);
      Serial.print(", ");
      Serial.print(moisture_humidity); Serial.print("\n");
    #endif
    
  if ( moisture_humidity < SOIL_HUMIDITY_LOW_LIMIT ) {
    #if ECHO_TO_SERIAL
      Serial.println("Valve Open");
    #endif
    
    logfile.print("Valve Open\n");
    openValve();  
  
  uint32_t opentime = 0;
  // Initialize the button
  getButton();
  
  while ( (moisture_humidity < SOIL_HUMIDITY_HIGH_LIMIT) || ((buttonIs==1)&&(buttonWas==0)) )  {
    if ( opentime <= MAX_OPEN_VALVE ) {
      moisture_humidity = read_humidity_sensor();
      // log time
      logfile.print(now.unixtime()); 					// seconds since 1/1/1970
      logfile.print(", ");
      logfile.print('"');
      logfile.print(now.year(), DEC);
      logfile.print("/");
      logfile.print(now.month(), DEC);
      logfile.print("/");
      logfile.print(now.day(), DEC);
      logfile.print(" ");
      logfile.print(now.hour(), DEC);
      logfile.print(":");
      logfile.print(now.minute(), DEC);
      logfile.print(":");
      logfile.print(now.second(), DEC);
      logfile.print('"');
      logfile.print(", ");
      logfile.print(DHT.humidity, 1);
      logfile.print(", ");
      logfile.print(DHT.temperature, 1);
      logfile.print(", ");
      logfile.print(moisture_humidity); logfile.print("\n");
      #if ECHO_TO_SERIAL
        Serial.print(moisture_humidity); Serial.print(" ");
      #endif  
      // To reduce the checks in every interval
      delay(round(MAX_OPEN_VALVE/2));
      opentime = opentime + round(MAX_OPEN_VALVE/2);
      
    } else { 
        #if ECHO_TO_SERIAL
          Serial.println("\nTime limit reached");
          Serial.println("Close Valve"); 
        #endif
        logfile.print("Time limit reached\n");
        logfile.print("Valve Open\n");
        closeValve();
        break; }
    }
  // Never arrive here after the break!.
  Serial.println("Close Valve");
  closeValve();
  logfile.print("Valve Open\n");
  }
  // Delay of HUMIDITY_CHECK_INTERVAL
  delay(HUMIDITY_CHECK_INTERVAL);
  
  // Now we write data to disk! Don't sync too often - requires 2048 bytes of I/O to SD card
  // which uses a bunch of power and takes time
  if ((millis() - syncTime) < SYNC_INTERVAL) return;
  syncTime = millis();
  
  // blink LED to show we are syncing data to the card & updating FAT!
  digitalWrite(redLEDpin, HIGH);
  logfile.flush();
  Serial.println("LogFile flushed");
  digitalWrite(redLEDpin, LOW);
}
