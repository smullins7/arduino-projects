#include <LiquidCrystal.h>
#include <math.h>
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

const int maxColumns = 16;
const int TIMER_SETTING_PIN = A0;
const int TIMER_PIN = 4;
const int FULL_RESET_PIN = 3;
const int RED_PIN = 6;
const int GREEN_PIN = 5;
const int CHECK_MS = 50;
const long INITIAL_TIMER_MS = 2000;
const int MAX_SETS = 3;
const int MIN_TIMER = 30; // 30 seconds
const int MAX_TIMER = 180; // 3 minutes
const int TIMER_INCREMENT = 5; // seconds

long previousTimerValue = MIN_TIMER;
long currentTimerSetting;
long timeRemainingMS;
unsigned long previousMillis = 0;
bool flash = true;
bool timerOn = false;
int state = 0;
int displaySteps = 2;
int flashSteps = 20;
int setCounter = 0;

void setup() {
  Serial.begin(9600);
  pinMode(TIMER_PIN, INPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  lcd.begin(16, 2);
}

int readTimerValue() {
  int val = analogRead(TIMER_SETTING_PIN);
  int mapped = map(val, 0, 1023, MIN_TIMER, MAX_TIMER);
  return (mapped / TIMER_INCREMENT) * TIMER_INCREMENT;
}



bool wasPressed() {
  return digitalRead(TIMER_PIN) == HIGH;
}

bool wasReset() {
  return digitalRead(FULL_RESET_PIN) == HIGH;
}


void stateTimerReady() {
  lcd.clear();
  lcd.print("Gym timer: ");
  lcd.print(readTimerValue());
  lcd.print("s");
  lcd.setCursor(0, 1);
  lcd.print("On set: ");
  lcd.print((setCounter % MAX_SETS) + 1);
  lcd.print(" of ");
  lcd.print(MAX_SETS);
}



/*
 * row is zero based, cursor moves to beginning of line
 */
void clearLine(int row) {
  for (int n = 0; n < maxColumns; n++) {
    lcd.setCursor(n, row);
    lcd.print(" ");
  }
  lcd.setCursor(0, row);
}

void displayTimer() {
  lcd.clear();
  lcd.print(String(timeRemainingMS / 1000.0, 1));
  displaySteps = 5;
}

void setTimer(long timer) {
  timeRemainingMS = timer * 1000;
  Serial.print("Setting timer in millis to: ");
  Serial.println(timeRemainingMS);
  Serial.print("raw value is: ");
  Serial.println(timer);
}

void light(char *color) {
  analogWrite(GREEN_PIN, 0);
  analogWrite(RED_PIN, 0);
  if (color == "green") {
    analogWrite(GREEN_PIN, 127);
  } else if (color == "red") {
    analogWrite(RED_PIN, 127);
  } else if (color == "yellow") {
    analogWrite(GREEN_PIN, 255);
    analogWrite(RED_PIN, 56);
  }
}

void loop() {
  if (millis() - previousMillis > CHECK_MS) {    
    int elapsedMS = millis() - previousMillis;
    previousMillis = millis();    

    if (wasPressed()) {
      Serial.println("Detected TIMER button was pushed");
      state = 2;
    } else if (wasReset()) {
      Serial.println("Detected RESET button was pushed");
      state = 0;
    }
    switch (state) {
      case 0:  // initial state, show timer
        light("green");
        setCounter = 0;
        stateTimerReady();
        state = 1;
        break;
      case 1:  // timer has not started yet, check if the timer setting needs to be updated
        light("green");
        currentTimerSetting = readTimerValue();
        if (currentTimerSetting != previousTimerValue) {
          previousTimerValue = currentTimerSetting;
          stateTimerReady();
        }
        break;
      case 2:  // button has been pressed
        light("yellow");
        setTimer(currentTimerSetting);
        state = 3;
        break;
      case 3:  // timer is running, display timer
        displayTimer();
        state = 4;
        break;
      case 4:  // timer is running, display does not need to be updated yet
        displaySteps--;
        timeRemainingMS = timeRemainingMS - (elapsedMS * 2);
        state = 3;
        if (timeRemainingMS <= 0) {
          lcd.clear();
          state = 5;
        }
        break;
      case 5:  // timer has ended, display message
        light("red");
        setCounter += 1;
        scrollTimeIsUp();
        stateTimerReady();
        state = 1;
        break;
      case 6:
        break;
    }
  }
}

void scrollTimeIsUp() {
  lcd.print("Time is up!!!");
  lcd.setCursor(0, 1);
  lcd.print("Get going!!!");
  delay(250);
  for (int i = 0; i < 16; i++) {
    lcd.scrollDisplayRight();
    delay(200);
  }
  for (int i = 0; i < 29; i++) {
    lcd.scrollDisplayLeft();
    delay(200);
  }
}
