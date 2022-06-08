// Variables will change:
int ledState_ = LOW;             // ledState used to set the LED

// The value will quickly become too large for an int to store
unsigned long previousMillis_ = 0;        // will store last time LED was updated

// constants won't change:
const long interval_ = 1000;  

void flashLED(int led){
  // check to see if it's time to blink the LED; that is, if the difference
  // between the current time and last time you blinked the LED is bigger than
  // the interval at which you want to blink the LED.
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis_ >= interval_) {
    // save the last time you blinked the LED
    previousMillis_ = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState_ == LOW) {
      ledState_ = HIGH;
    } else {
      ledState_ = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(led, ledState_);
  }
}
