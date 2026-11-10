#include <Arduino.h>
#include <esp_sleep.h>

#define LED_PIN 2

RTC_DATA_ATTR int soLanThuc = 0;  // GIU qua deep sleep
int demThuong = 0;                // MAT qua deep sleep

void nhay(int n) {
  for (int i = 0; i < n; i++) {
    digitalWrite(LED_PIN, HIGH); delay(200);
    digitalWrite(LED_PIN, LOW);  delay(200);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  soLanThuc++;
  demThuong++;
  Serial.printf("RTC = %d | RAM = %d\n", soLanThuc, demThuong);

  nhay(soLanThuc);   // nhay 1, 2, 3... (tang dan)
  delay(1000);
  nhay(demThuong);   // luon nhay 1 lan

  esp_sleep_enable_timer_wakeup(10ULL * 1000000ULL);  // 10 giay
  esp_deep_sleep_start();
}
