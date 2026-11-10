#include <Arduino.h>

#define LED_PIN 2
#define ADC_PIN 34
#define T_LED 500UL
#define T_ADC 2000UL
#define T_HEART 5000UL
#define PHA_MS 30000UL

struct Nhip { const char *ten; uint32_t chuKyUs, lanTruocUs, dem, tongLechUs, lechMaxUs; };
Nhip nLed = {"LED", T_LED * 1000UL, 0, 0, 0, 0};
Nhip nAdc = {"ADC", T_ADC * 1000UL, 0, 0, 0, 0};
Nhip nHeart = {"HEART", T_HEART * 1000UL, 0, 0, 0, 0};

hw_timer_t *timer = nullptr;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
volatile uint32_t timerMs = 0;
uint32_t lastLedMs = 0, lastAdcMs = 0, lastHeartMs = 0, phaCu = 0;
bool ledState = false;
const uint16_t taiMs[] = {30, 180, 70, 150, 10, 120};
uint8_t iTai = 0;

void ARDUINO_ISR_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  ++timerMs;
  portEXIT_CRITICAL_ISR(&timerMux);
}
uint32_t docTimerMs() {
  portENTER_CRITICAL(&timerMux);
  uint32_t now = timerMs;
  portEXIT_CRITICAL(&timerMux);
  return now;
}
void ghiNhan(Nhip &n, uint32_t nowUs) {
  ++n.dem;
  if (n.dem > 2) {
    int32_t e = (int32_t)(nowUs - n.lanTruocUs) - (int32_t)n.chuKyUs;
    if (e < 0) e = -e;
    n.tongLechUs += e;
    if ((uint32_t)e > n.lechMaxUs) n.lechMaxUs = e;
  }
  n.lanTruocUs = nowUs;
}
void xoa(Nhip &n) { n.dem = n.tongLechUs = n.lechMaxUs = 0; }
void inNhip(const Nhip &n) {
  uint32_t samples = n.dem > 2 ? n.dem - 2 : 0;
  float avg = samples ? (float)n.tongLechUs / samples / 1000.0f : 0;
  Serial.printf("%-5s | %3lu mau | lech TB=%8.3f ms | max=%8.3f ms | tong=%9.3f ms\n",
    n.ten, (unsigned long)samples, avg, n.lechMaxUs / 1000.0f, n.tongLechUs / 1000.0f);
}
void viecNang() {
  uint32_t start = micros();
  while ((uint32_t)(micros() - start) < (uint32_t)taiMs[iTai] * 1000UL) { }
  iTai = (iTai + 1) % 6;
}
void tacVu(uint32_t ms) {
  uint32_t us = micros();
  if ((uint32_t)(ms - lastLedMs) >= T_LED) {
    lastLedMs = ms; ledState = !ledState; digitalWrite(LED_PIN, ledState); ghiNhan(nLed, us);
  }
  if ((uint32_t)(ms - lastAdcMs) >= T_ADC) {
    lastAdcMs = ms; ghiNhan(nAdc, us); Serial.printf("ADC = %d\n", analogRead(ADC_PIN));
  }
  if ((uint32_t)(ms - lastHeartMs) >= T_HEART) {
    lastHeartMs = ms; ghiNhan(nHeart, us); Serial.printf("[heartbeat] %.1f s\n", millis() / 1000.0f);
  }
}
void pha() {
  uint32_t p = millis() / PHA_MS;
  if (p != phaCu) {
    Serial.printf("\n===== hardware timer | pha %lu: %s =====\n", (unsigned long)phaCu, phaCu % 2 ? "CPU BAN" : "CPU RANH");
    inNhip(nLed); inNhip(nAdc); inNhip(nHeart); Serial.println();
    xoa(nLed); xoa(nAdc); xoa(nHeart); phaCu = p;
  }
}
void setup() {
  Serial.begin(115200); pinMode(LED_PIN, OUTPUT); pinMode(ADC_PIN, INPUT); analogReadResolution(12);
  timer = timerBegin(1000000);  // Timer counter: 1 MHz, each tick is 1 microsecond.
  if (timer == nullptr) { Serial.println("Loi khoi tao hardware timer"); return; }
  timerAttachInterrupt(timer, &onTimer);
  timerAlarm(timer, 1000, true, 0);  // ISR every 1000 microseconds, or 1 ms.
  Serial.println("hardware timer: ba tac vu doc lap; moi pha 30 giay");
}
void loop() {
  if (timer == nullptr) return;
  uint32_t now = docTimerMs();
  tacVu(now); pha();
  if ((millis() / PHA_MS) % 2) viecNang();
}
