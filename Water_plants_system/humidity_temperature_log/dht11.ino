void temphumi() {
  long chk;
  chk = DHT.read(DHT11_PIN);

  //long sensorValue = analogRead(A1);
  //long tempCelsius = sensorValue*0.5;
  //while (!Serial.available()) {} // wait for data to arrive
  // serial read section
  while (Serial.available())
  {
    delay(10);  //delay to allow buffer to fill
    if (Serial.available() > 0)
    {
      readString = "";
      char c = Serial.read();  //gets one byte from serial buffer
      readString += c; //makes the string readString
    }
  }

  if (readString.length() > 0)
  {
    Serial.print("Arduino received: ");
    Serial.println(readString); //see what was received
  }

  switch (chk){
    case DHTLIB_OK:  
                Serial.print("OK,\t"); 
                break;
    case DHTLIB_ERROR_CHECKSUM: 
                Serial.print("Checksum error,\t"); 
                break;
    case DHTLIB_ERROR_TIMEOUT: 
                Serial.print("Time out error,\t"); 
                break;
    default: 
                Serial.print("Unknown error,\t"); 
                break;
  }
  
  //delay(500);
  #if ECHO_TO_SERIAL
    Serial.print("DHT11 DEBUG\n");
    Serial.print(DHT.humidity, 1); Serial.print(",");
    Serial.print(DHT.temperature, 1); Serial.print("\n");
    Serial.flush();
  #endif
  //if(Serial.available()){ // only send data back if data has been sent

  //}

  //Serial.println(tempCelsius,DEC);   // temperatura del sensore LM335

  //delay(1000);
}
