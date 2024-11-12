#include <LiquidCrystal.h>
#include <Servo.h>

/*
 * LCD CONFIGURATION
 * LCD RS pin to digital pin 8
 * LCD Enable pin to digital pin 7
 * LCD D4 pin to digital pin 6
 * LCD D5 pin to digital pin 5
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * K Cathode -GCC
 * A Anode - +5 via 10 ohm resistor
*/
const int rs = 8, en = 7, d4 = 6, d5 = 5, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
Servo myServo;

//Servo Motor
//Red - +5V
//Brown - GCC
//Yellow - ServoPin 13
const int SERVO_PIN = 13;

//360 Rotot
const int RESET = 9;
const int DATA = 10;
const int CLOCK = 11;
//Buzzer
const int BUZZER_PIN = 4;

int centerAngle = 90;
int angleToMove = 50;
int lastStateCLK;
int currentStateCLK;
int timerValue = 3600; // 60 minutes (3600 seconds)
int elapsedTime = 3600; // Also in seconds
int incrementBy = 600; //10 mins - this will increase the time when the knob is rotated

unsigned long lastDebounceTime = 0;
unsigned long lastTimerUpdate = 0;
unsigned long lastServoMove = 0;  // To track the time for servo movement
const unsigned long debounceDelay = 50;
const unsigned long secondInterval = 1000; // 1 second in milliseconds
const unsigned long servoInterval = 10000;  // 10 seconds in milliseconds
bool moveLeft = true;

void setup() {
  lcd.begin(16, 2);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(CLOCK, INPUT);
  pinMode(DATA, INPUT);
  pinMode(RESET, INPUT_PULLUP);
  lastStateCLK = digitalRead(CLOCK);
  updateLCD(timerValue, elapsedTime);
  
  myServo.attach(SERVO_PIN);
  myServo.write(centerAngle);
}

void loop() {
  currentStateCLK = digitalRead(CLOCK);

  // Rotary Encoder to change the timer value
  if (currentStateCLK != lastStateCLK) {
    if ((millis() - lastDebounceTime) > debounceDelay) {
      lastDebounceTime = millis();
      if (digitalRead(DATA) != currentStateCLK) {
        timerValue -= incrementBy;
        if (timerValue < 0) timerValue = 0;
      } else {
        timerValue += incrementBy;
      }
      updateLCD(timerValue, elapsedTime);
    }
  }

  // Reset button to set the elapsed time to the timer value
  if (digitalRead(RESET) == LOW) {
    elapsedTime = timerValue;
    updateLCD(timerValue, elapsedTime);
    delay(200); // Simple debounce delay
  }

  // Timer countdown logic (every second)
  if (elapsedTime > 0 && (millis() - lastTimerUpdate) >= secondInterval) {
    lastTimerUpdate = millis();
    elapsedTime--;
    if (elapsedTime <= 0) {
      beep();
      shutSystem();
      elapsedTime = timerValue;
    }
    updateLCD(timerValue, elapsedTime);
  }

  // Servo movement every 10 seconds
  if (millis() - lastServoMove >= servoInterval) {
    lastServoMove = millis();

    if (moveLeft) {
      myServo.write(centerAngle - angleToMove);
    } else {
      myServo.write(centerAngle + angleToMove);
    }

    moveLeft = !moveLeft;
  }

  lastStateCLK = currentStateCLK;
}

// Function to beep the buzzer 5 times when time is up
void beep() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(500);
    digitalWrite(BUZZER_PIN, LOW);
    delay(500);
  }
}

// Function to update the LCD display with the timer and elapsed time
void updateLCD(int timer, int elapsed) {
  int minutes = elapsed / 60;
  int seconds = elapsed % 60;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Time: ");
  lcd.print(timer / 60);
  lcd.print(" Mins");

  lcd.setCursor(0, 1);
  lcd.print("Blnc: ");
  if (minutes < 10) lcd.print("0");
  lcd.print(minutes);
  lcd.print(":");
  if (seconds < 10) lcd.print("0");
  lcd.print(seconds);
}

//function to keep tuen off the system once the time is elapsed
void shutSystem(){
  lcd.setCursor(0, 1);
  lcd.print("..... Done .....");
  while(true){
    if (digitalRead(RESET) == LOW) {
      elapsedTime = timerValue;
      updateLCD(timerValue, elapsedTime);
      delay(200); // Simple debounce delay
    }
  }
}
