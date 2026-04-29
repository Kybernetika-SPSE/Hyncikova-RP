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

int lastCycleMinute = -1;
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

// -------- Servo (POMALÉ) --------
void otocServo() {
  digitalWrite(ledPin, HIGH);

  servoMotor.write(75); 
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
  servoMotor.write(90); // stop
}

// -------- Loop --------
void loop() {

  DateTime now = rtc.now();
  int minuteNow = now.minute();
  int hourNow = now.hour();

  distance = zmerVzdalenost();

  Serial.print("Cas: ");
  Serial.print(hourNow);
  Serial.print(":");
  Serial.print(minuteNow);
  Serial.print(" | Vzdalenost: ");
  Serial.println(distance);

  bool prekazka = (distance > 0 && distance < 11.5);

  // -------- TLAČÍTKO --------
  bool buttonPressed = (digitalRead(buttonPin) == LOW);
  if (buttonPressed && !buttonWasPressed) {
    Serial.println("Tlacitko zmacknuto");
    otocServo();
  }
  buttonWasPressed = buttonPressed;

  // -------- KAŽDÝCH 5 MINUT --------
  if (minuteNow % 5 == 0 && minuteNow != lastCycleMinute) {

    if (prekazka) {
      Serial.println("Preskoceno kvuli prekazce");
    } else {
      Serial.println("Servo podle hodin (5 min)");
      otocServo();
    }

    lastCycleMinute = minuteNow;
  }

  // -------- LED --------
  if (prekazka) {
    digitalWrite(ledPin, HIGH);  
  } else {
    digitalWrite(ledPin, LOW);
  }

  delay(100);
}