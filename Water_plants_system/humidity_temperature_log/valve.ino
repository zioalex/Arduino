//===========Functions=====================//
void getButton() { 
  buttonWas = buttonIs; // Set the old state of the button to be the current state since we're creating a new current state.
  buttonIs = digitalRead(SwPin); // Read the button state
} 

void openValve(){
    digitalWrite(ValvePin, HIGH);
    digitalWrite(redLEDpin,HIGH);
    Serial.println("Valve Open");
}

void closeValve(){
  digitalWrite(ValvePin, LOW);
  digitalWrite(redLEDpin,LOW);
  Serial.println("Valve closed");
}
