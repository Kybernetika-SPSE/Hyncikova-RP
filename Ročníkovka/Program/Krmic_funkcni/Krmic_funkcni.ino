#include <Wire.h>
#include <RTClib.h>
#include <Servo.h>

RTC_DS3231 rtc;
Servo servoMotor;

#define trigPin 9
#define echoPin 10
#define ledPin 3
#define buttonPin 4

long duration;
float distance;

int lastRunMinute = -1;
int lastRunHour = -1;

bool buttonWasPressed = false;

// -------- Ultrazvuk --------
float zmerVzdalenost() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, 30000);
  return duration * 0.034 / 2;
}

// -------- Servo --------
void otocServo() {
  digitalWrite(ledPin, HIGH);

  servoMotor.write(75);   // pomalejší otáčení
  delay(3000);

  servoMotor.write(90);   // stop
}

// -------- Setup --------
void setup() {
  Serial.begin(9600);
  Wire.begin();
  rtc.begin();

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  servoMotor.attach(6);
  servoMotor.write(90);
}

// -------- Loop --------
void loop() {

  DateTime now = rtc.now();
  int hourNow = now.hour();
  int minuteNow = now.minute();

  distance = zmerVzdalenost();

  Serial.print("Cas: ");
  Serial.print(hourNow);
  Serial.print(":");
  Serial.print(minuteNow);
  Serial.print(" | Vzdalenost: ");
  Serial.println(distance);

  bool prekazka = (distance > 0 && distance < 13.5);

  // -------- TLAČÍTKO --------
  bool buttonPressed = (digitalRead(buttonPin) == LOW);
  if (buttonPressed && !buttonWasPressed) {
    Serial.println("Tlacitko zmacknuto");
    otocServo();
  }
  buttonWasPressed = buttonPressed;

  // -------- KONKRÉTNÍ ČASY --------
  bool jeCas =
    (hourNow == 7  && minuteNow == 0) ||
    (hourNow == 12 && minuteNow == 30) ||
    (hourNow == 18 && minuteNow == 30);

  if (jeCas && !(hourNow == lastRunHour && minuteNow == lastRunMinute)) {

    if (prekazka) {
      Serial.println("Preskoceno kvuli prekazce");
    } else {
      Serial.println("Servo podle konkretniho casu");
      otocServo();
    }

    lastRunHour = hourNow;
    lastRunMinute = minuteNow;
  }

  // -------- LED --------
  if (prekazka) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }

  delay(100);
}