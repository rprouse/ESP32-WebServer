#include <Arduino.h>
#include <time.h>
#include "esp_sntp.h"

#include "secrets.h"

void onTimeSync(struct timeval *tv)
{
  Serial.printf("[NTP] Synced, epoch=%ld\n", (long)tv->tv_sec);
}

void initTime()
{
  sntp_set_time_sync_notification_cb(onTimeSync); // set before configTzTime()
  // sntp_set_sync_mode(SNTP_SYNC_MODE_SMOOTH);    // see note below

  configTzTime(TZ_LOCAL, NTP_1, NTP_2, NTP_3); // starts SNTP + background resync

  sntp_set_sync_interval(6UL * 60 * 60 * 1000); // resync every 6 h (default is 1 h)
  sntp_restart();                               // apply the new interval immediately

  struct tm tm;
  if (getLocalTime(&tm, 10000))
  { // wait up to 10 s for first sync
    Serial.println(&tm, "[NTP] %A %Y-%m-%d %H:%M:%S %Z");
  }
  else
  {
    Serial.println("[NTP] First sync timed out — will keep trying in background");
  }
}
