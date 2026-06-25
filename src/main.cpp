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

#include "memory.h"

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) { delay(10); }  // wait for USB-CDC host, cap at 3s
  printMemoryReport();
}

void loop() {
  delay(10000);  // nothing to do; report is printed once at boot
}
