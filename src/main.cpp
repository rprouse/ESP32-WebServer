/*
 * Board Memory Report
 * Prints SRAM, Flash, and PSRAM sizes over serial for any ESP32 family
 * board (ESP32 / S2 / S3 / C3 / C6 ...) using the Arduino-ESP32 core.
 *
 * IMPORTANT: ESP.getFlashChipSize() returns the size CONFIGURED at build
 * time (the bootloader magic byte), NOT the true physical chip size.
 * For the real physical flash size, run:  esptool flash_id
 *
 * PSRAM only appears if it is enabled in the build config:
 *   Arduino IDE -> Tools -> PSRAM (OPI/QSPI), or
 *   PlatformIO  -> build flag (e.g. -DBOARD_HAS_PSRAM)
 * even when the chip is physically present.
 */

#include <Arduino.h>
#include <WiFi.h>

#include "memory_stats.h"
#include "wifi_conn.h"
#include "ntp.h"

void printTimes()
{
  time_t now = time(nullptr); // UTC epoch — the canonical instant, always

  struct tm utc;
  gmtime_r(&now, &utc); // UTC, ignores TZ entirely
  struct tm local;
  localtime_r(&now, &local); // local, applies TZ + DST

  char buf[64];
  strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &utc);
  Serial.printf("[UTC] %s\r\n", buf);
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S %Z", &local);
  Serial.printf("[Local] %s\r\n", buf); // %Z prints MST or MDT correctly
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) { delay(10); }  // wait for USB-CDC host, cap at 3s

  printMemoryReport();

  WiFi.onEvent(onWiFiEvent);  // register BEFORE begin()

  uint8_t attempts = 0;
  while (!wifiConnect(CONNECT_TIMEOUT_MS) && ++attempts < CONNECT_MAX_ATTEMPTS) {
    Serial.printf("[WiFi] Retry %u after backoff...\n", attempts);
    WiFi.disconnect(true);       // clear stale state before retrying
    delay(2000 * attempts);      // linear backoff
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[WiFi] Giving up — rebooting in 5s.");
    delay(5000);
    ESP.restart();
  }

  initTime();
  printTimes();
}

void loop() {
  wifiWatchdog();   // non-blocking; auto-reconnect does the heavy lifting

  delay(1000);
}
