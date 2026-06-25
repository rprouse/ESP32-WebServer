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

static void printSize(const char* label, uint64_t bytes) {
  Serial.printf("%-22s %9.2f KB   (%.2f MB)\r\n",
                label, bytes / 1024.0, bytes / (1024.0 * 1024.0));
}

static void printMemoryReport() {


  Serial.println();
  Serial.println("==== Board Memory Report ====");

  // ---- Chip identity ----
  Serial.printf("Chip model:            %s rev %d\r\n",
                ESP.getChipModel(), ESP.getChipRevision());
  Serial.printf("CPU cores:             %d @ %d MHz\r\n",
                ESP.getChipCores(), getCpuFrequencyMhz());
  Serial.printf("SDK version:           %s\r\n", ESP.getSdkVersion());
  Serial.println();

  // ---- SRAM (internal RAM / heap) ----
  printSize("SRAM total heap:", ESP.getHeapSize());
  printSize("SRAM free heap:",  ESP.getFreeHeap());
  printSize("SRAM max alloc:",  ESP.getMaxAllocHeap());
  Serial.println();

  // ---- Flash (CONFIGURED size - verify with esptool flash_id) ----
  Serial.println("Flash (build-config size - see note below):");
  printSize("Flash size:", ESP.getFlashChipSize());
  Serial.printf("Flash speed:           %d MHz\r\n",
                ESP.getFlashChipSpeed() / 1000000);
  Serial.printf("Flash mode:            %d  (0=QIO 1=QOUT 2=DIO 3=DOUT 4=FAST 5=SLOW)\r\n",
                ESP.getFlashChipMode());
  Serial.println();

  // ---- PSRAM ----
  if (psramFound()) {
    printSize("PSRAM total:", ESP.getPsramSize());
    printSize("PSRAM free:",  ESP.getFreePsram());
  } else {
    Serial.println("PSRAM:                 none detected / not enabled in build");
  }
  Serial.println();

  Serial.println("NOTE: flash size above is the build-config value, not the");
  Serial.println("physical chip. For the true size run:  esptool flash_id");
  Serial.println("=============================");
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) { delay(10); }  // wait for USB-CDC host, cap at 3s
  printMemoryReport();
}

void loop() {
  delay(10000);  // nothing to do; report is printed once at boot
}
