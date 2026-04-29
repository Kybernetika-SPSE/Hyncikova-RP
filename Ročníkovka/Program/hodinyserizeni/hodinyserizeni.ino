#include <Wire.h>
#include <RTClib.h>

RTC_DS3231 rtc;

void setup () {
  Serial.begin(9600);

  if (!rtc.begin()) {
    Serial.println("RTC nenalezeno!");
    while (1);
  }

  // Nastaví čas podle počítače (čas při kompilaci)
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  Serial.println("Cas byl nastaven!");
}

void loop () {
  // nic se neděje
}