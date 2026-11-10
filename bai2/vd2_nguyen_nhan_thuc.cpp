#include <Arduino.h>
#include <esp_sleep.h>
#include "driver/rtc_io.h"

#define LED_PIN 2
#define BUTTON_PIN 4   // nut noi giua GPIO4 va GND

void diNgu() {
  while (digitalRead(BUTTON_PIN) == LOW) delay(10);  // doi nha nut

  digitalWrite(LED_PIN, LOW);   // tat LED
  Serial.println("-> Deep sleep 10 giay (nhan nut de thuc som)");
  Serial.flush();

  esp_sleep_enable_timer_wakeup(10ULL * 1000000ULL);  // thuc bang timer
  rtc_gpio_pullup_en(GPIO_NUM_4);
  rtc_gpio_pulldown_dis(GPIO_NUM_4);
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_4, 0);        // hoac thuc bang nut
  esp_deep_sleep_start();
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  rtc_gpio_deinit(GPIO_NUM_4);          // tra GPIO4 ve dung binh thuong
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  Serial.println("Chay lai tu setup()");

  // nguyen nhan thuc:
  switch (esp_sleep_get_wakeup_cause()) {
    case ESP_SLEEP_WAKEUP_TIMER: Serial.println("Thuc day do timer");   break;
    case ESP_SLEEP_WAKEUP_EXT0:  Serial.println("Thuc day do nut GPIO4"); break;
    default:                     Serial.println("Cap nguon / reset");   break;
  }
}

void loop() {
  // LED nhap nhay = CPU con chay
  digitalWrite(LED_PIN, (millis() / 200) % 2);

  // bam nut -> deep sleep
  if (digitalRead(BUTTON_PIN) == LOW) diNgu();
}
