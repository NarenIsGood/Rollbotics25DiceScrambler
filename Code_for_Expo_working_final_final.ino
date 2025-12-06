#include <Servo.h>
#include <LiquidCrystal.h>

Servo gate;
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

const int S0 = 4;
const int S1 = 5;
const int S2 = 6;
const int S3 = A1;
const int sensorOut = A2;

const int motorPin = 3;
const int buzzer = 2;

int redFreq, greenFreq, blueFreq;

int readColor() {
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  redFreq = pulseIn(sensorOut, LOW);

  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  greenFreq = pulseIn(sensorOut, LOW);

  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  blueFreq = pulseIn(sensorOut, LOW);

  if (redFreq < 35 && greenFreq < 40 && blueFreq < 40) return 1;
  if (redFreq > 60 && greenFreq < 50 && blueFreq < 55) return 6;
  if (redFreq < 45 && greenFreq < 60 && blueFreq > 70) return 3;
  if (redFreq < 50 && greenFreq > 70 && blueFreq < 60) return 2;
  if (redFreq > 75 && greenFreq > 80 && blueFreq < 70) return 4;
  return 5;
}

void shakeDice() {
  digitalWrite(motorPin, HIGH);
  delay(750);
  digitalWrite(motorPin, LOW);
}

void openGate() {
  gate.write(90);
  delay(400);
  gate.write(0);
}

void setup() {
  gate.attach(13);
  gate.write(0);

  lcd.begin(16, 2);
  lcd.clear();

  pinMode(motorPin, OUTPUT);
  pinMode(buzzer, OUTPUT);

  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);

  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
}

void loop() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Shaking Dice");

  shakeDice();
  tone(buzzer, 600, 200);

  delay(300);

  lcd.clear();
  lcd.print("Releasing...");
  openGate();
  delay(600);

  lcd.clear();
  lcd.print("Reading...");

  int result = readColor();

  lcd.clear();
  lcd.print("Value: ");
  lcd.print(result);

  tone(buzzer, 900, 150);
  delay(1500);
}
