// Read from Moisture senson function
int read_humidity_sensor() {
  digitalWrite(humidity_sensor_vcc, HIGH);
  delay(500);
  int value = analogRead(humidity_sensor_pin);
  digitalWrite(humidity_sensor_vcc, LOW);
  #if ECHO_TO_SERIAL
    Serial.print(value); Serial.print("\n");
  #endif
  return 1023 - value;
  
}
