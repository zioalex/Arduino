void error(char *str) {
  Serial.print("error: ");
  Serial.println(str);
  
  // red LED indicates error
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);              // wait for a second
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);              // wait for a second

  //while(1);  
}
