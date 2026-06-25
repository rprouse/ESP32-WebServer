#include <Arduino.h>

static void printSize(const char* label, uint64_t bytes) {
  Serial.printf("%s %.2f KB (%.2f MB)\r\n",
                label, bytes / 1024.0, bytes / (1024.0 * 1024.0));
}

void printMemoryReport() {
  // ---- Chip identity ----
  Serial.printf("[Chip Model] %s rev %d\r\n",
                ESP.getChipModel(), ESP.getChipRevision());
  Serial.printf("[CPU Cores] %d @ %d MHz\r\n",
                ESP.getChipCores(), getCpuFrequencyMhz());
  Serial.printf("[SDK Version] %s\r\n", ESP.getSdkVersion());
  Serial.println();

  // ---- SRAM (internal RAM / heap) ----
  printSize("[SRAM total heap]", ESP.getHeapSize());
  printSize("[SRAM free heap]",  ESP.getFreeHeap());
  printSize("[SRAM max alloc]",  ESP.getMaxAllocHeap());
  Serial.println();

  // ---- Flash (CONFIGURED size - verify with esptool flash_id) ----
  printSize("[Flash Size]", ESP.getFlashChipSize());
  Serial.printf("[Flash Speed] %d MHz\r\n",
                ESP.getFlashChipSpeed() / 1000000);
  Serial.printf("[Flash Mode] %d (0=QIO 1=QOUT 2=DIO 3=DOUT 4=FAST 5=SLOW)\r\n",
                ESP.getFlashChipMode());
  Serial.println();

  // ---- PSRAM ----
  if (psramFound()) {
    printSize("[PSRAM Total]", ESP.getPsramSize());
    printSize("[PSRAM Free]",  ESP.getFreePsram());
  } else {
    Serial.println("[PSRAM] none detected / not enabled in build");
  }
  Serial.println();
}
