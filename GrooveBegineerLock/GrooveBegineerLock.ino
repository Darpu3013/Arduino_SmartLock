#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

const int hallPin = 2;
const int ledPin = 4;
const int buzzerPin = 5;

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

void setup() {
  pinMode(hallPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  digitalWrite(buzzerPin, LOW);

  u8g2.begin();
  u8g2.setFont(u8g2_font_ncenB14_tr);
}

void loop() {
  bool magnetDetected = (digitalRead(hallPin) == LOW);

  u8g2.clearBuffer();

  if (magnetDetected) {
    // Door locked
    digitalWrite(ledPin, LOW);
    noTone(buzzerPin);
    u8g2.drawStr(20, 35, "LOCKED");
  } else {
    // Door open
    digitalWrite(ledPin, HIGH);
    tone(buzzerPin, 1000);   // 1000 Hz beep
    u8g2.drawStr(0, 35, "LOCK OPEN");
  }

  u8g2.sendBuffer();

  delay(100);
}