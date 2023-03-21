//TODO digital pin 21 does not work- ALL THE ANALOG PINS ARE ALSO DIGITAL MIND BLOWN!!!!!
// Variables will change:
int ledState = LOW;  // ledState used to set the LED

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;  // will store last time LED was updated

// constants won't change:
const long interval = 250;  // interval at which to blink (milliseconds)
const int START_PIN = 2;
const int END_PIN = 21;
void setup() {
  for (int i=START_PIN; i<=END_PIN; i++) {
      pinMode(i, OUTPUT);
   }
}

void loop() {
  // put your main code here, to run repeatedly:

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    for (int i=START_PIN; i<=END_PIN; i++) {
      digitalWrite(i, ledState);
    }
  }
      
}
