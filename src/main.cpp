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
#include "web.h"

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

#if !STATUS_LED_IS_RGB
  pinMode(STATUS_LED_PIN, OUTPUT);  // plain LED needs an output pin
#endif

  printMemoryReport();
  wifiInit();
  initTime();
  printTimes();
  webServerSetup();
}

void loop()
{
  // Toggle the onboard LED each second so we can confirm loop() is running
  // even when serial output isn't visible.
  static bool on = false;
  on = !on;
#if STATUS_LED_IS_RGB
  rgbLedWrite(STATUS_LED_PIN, 0, 0, on ? 16 : 0); // dim blue toggle
#else
  digitalWrite(STATUS_LED_PIN, on);
#endif

  wifiWatchdog();   // non-blocking; auto-reconnect does the heavy lifting

  delay(1000);

  Serial.println(on ? "PING" : "PONG");
}
