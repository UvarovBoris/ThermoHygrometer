#include <Arduino.h>
#include "DHT.h"          //adafruit dht sensor library
#include "GyverTM1637.h"  //gyverlibs/GyverTM1637
#include "microDS3231.h"  //gyverlibs/microDS3231
#include "EncButton.h"    //EncButton

#define DHT_IN_DATA_PIN 2
#define DHT_TYPE DHT11

#define HT_OUT_CLK_PIN 11
#define HT_OUT_DIO_PIN 12

#define CLOCK_OUT_CLK_PIN 3
#define CLOCK_OUT_DIO_PIN 4
#define CLOCK_HOUR_BTN_PIN 7
#define CLOCK_MINUTE_BTN_PIN 8


DHT dht(DHT_IN_DATA_PIN, DHT_TYPE);
GyverTM1637 htDisplay(HT_OUT_CLK_PIN, HT_OUT_DIO_PIN);
long lastHtCheckMS = 0;

MicroDS3231 rtc;
GyverTM1637 clockDisplay(CLOCK_OUT_CLK_PIN, CLOCK_OUT_DIO_PIN);
long lastClockCheckMS = 0;
bool clockPoint = true;
EncButton<EB_TICK, CLOCK_HOUR_BTN_PIN> hourBtn;
EncButton<EB_TICK, CLOCK_MINUTE_BTN_PIN> minuteBtn;

void showHTData(int t, int h) {
  for (int i = 1; i >= 0; i--) {
    htDisplay.display(i, t % 10);
    t /= 10;
  }
  for (int i = 3; i >= 2; i--) {
    htDisplay.display(i, h % 10);
    h /= 10;
  }
  htDisplay.point(true);
}

void setup() {
  Serial.begin(9600);

  dht.begin();

  htDisplay.clear();
  htDisplay.brightness(7);

  clockDisplay.clear();
  clockDisplay.brightness(7);
}

void incHour() {
  DateTime date = rtc.getTime();
  if (date.hour == 23)
    date.hour = 0;
  else
    date.hour++;

  rtc.setTime(date);  
}

void incMinute() {
  DateTime date = rtc.getTime();
  if (date.minute == 59)
    date.minute = 0;
  else
    date.minute++;

  rtc.setTime(date);
}


void loop() {
  if (lastHtCheckMS == 0 || millis() - lastHtCheckMS > 5000) {
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    if (!isnan(t) && !isnan(h)) {
      showHTData(t, h);
    }
    lastHtCheckMS = millis();
  }

  hourBtn.tick();
  if (hourBtn.click()) {
    incHour();
  }
  if (hourBtn.step()) {
    incHour();
  }

  minuteBtn.tick();
  if (minuteBtn.click()) {
    incMinute();
  }
  if (minuteBtn.step()) {
    incMinute();
  }

  if (lastClockCheckMS == 0 || millis() - lastClockCheckMS > 500) {
    clockDisplay.displayClock(rtc.getHours(), rtc.getMinutes());
    clockDisplay.point(clockPoint);
    clockPoint = !clockPoint;
    lastClockCheckMS = millis();
  }
}
