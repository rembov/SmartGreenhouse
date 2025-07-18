#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <Servo.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(2, DHT11);
Servo motor;
const int pumpPin = 3;
const int lampPin = 4;
const int ldrPin = A0;
const int soilPin = A1;
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = { A2, A3, 5, 6 };
byte colPins[COLS] = { 7, 10, 11, 12 };
Keypad keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
enum Mode { MAIN, AUTO, MANUAL } mode = MAIN;
float tempThreshold = 26.0;
int moistThreshold = 500;
int lightThreshold = 600;
bool servoActive = false;
bool pumpManual = false;
bool lampManual = false;
bool servoManual = false;
void setup() {
  lcd.init();
  lcd.backlight();
  dht.begin();
  motor.attach(9);
  pinMode(pumpPin, OUTPUT);
  pinMode(lampPin, OUTPUT);
  digitalWrite(pumpPin, LOW);
  digitalWrite(lampPin, LOW);
  motor.write(0);
}
void loop() {
  char key = keypad.getKey();
  if (key) {
    if (key == 'A') mode = AUTO;
    if (key == 'B') mode = MANUAL;
    if (key == 'C') mode = MAIN;
    if (mode == MANUAL) {
      if (key == '1') pumpManual = !pumpManual;
      if (key == '2') lampManual = !lampManual;
      if (key == '3') servoManual = !servoManual;
    }
  }
  lcd.setCursor(0, 0);
  if (mode == MAIN) lcd.print("Mode: MAIN      ");
  if (mode == AUTO) lcd.print("Mode: AUTO      ");
  if (mode == MANUAL) lcd.print("Mode: MANUAL      ");
  float t = dht.readTemperature();
  int l = analogRead(ldrPin);
  int s = analogRead(soilPin);
  lcd.setCursor(0, 1);
  lcd.print("T:");
  lcd.print((int)t);
  lcd.print(" H:");
  lcd.print((int)s);
  lcd.print(" L:");
  lcd.print((int)l);
  if (mode == AUTO) {
   if (t >= tempThreshold && !servoActive) {
      motor.write(180);
      servoActive = true;
    } else if (t < 26 && servoActive) {
      motor.write(0);
      servoActive = false;
}
    if (l < lightThreshold) {
      digitalWrite(lampPin, HIGH);
    } else {
      digitalWrite(lampPin, LOW);
    }
    if (s < moistThreshold) digitalWrite(pumpPin, HIGH);
    else digitalWrite(pumpPin, LOW);
  }
  if (mode == MANUAL) {
    digitalWrite(pumpPin, pumpManual ? HIGH : LOW);
    digitalWrite(lampPin, lampManual ? HIGH : LOW);
    motor.write(servoManual ? 180 : 0);
  }
  delay(10);
}

