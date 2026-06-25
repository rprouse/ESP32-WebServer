// =====================================================================
//  ESP32 robust WiFi connect + auto-reconnect (Arduino core)
// ---------------------------------------------------------------------
//  - Blocking connect with timeout for setup()
//  - Human-readable status + disconnect-reason decoding over Serial
//  - Event-driven auto-reconnect for the long-running case
//  - Loop watchdog: reboots a headless node if WiFi stays down too long
//
//  Built-in libraries only (WiFi ships with the ESP32 Arduino core),
//  so nothing extra is needed in platformio.ini lib_deps.
//  ESP32 is 2.4 GHz only — a 5 GHz / band-steered SSID looks like a typo.
// =====================================================================

#include <Arduino.h>
#include <WiFi.h>          // ESP32 core WiFi library (must not be shadowed by a local wifi.h)

#include "wifi_conn.h"     // our own module header (renamed to avoid the WiFi.h collision)
#include "secrets.h"       // WiFi credentials (SSID, password, hostname)

// ---------------------- Diagnostics helpers --------------------------
const char* wlStatusStr(wl_status_t s) {
  switch (s) {
    case WL_IDLE_STATUS:     return "IDLE";
    case WL_NO_SSID_AVAIL:   return "NO_SSID_AVAIL (wrong SSID, out of range, or 5 GHz-only AP)";
    case WL_SCAN_COMPLETED:  return "SCAN_COMPLETED";
    case WL_CONNECTED:       return "CONNECTED";
    case WL_CONNECT_FAILED:  return "CONNECT_FAILED (usually wrong password)";
    case WL_CONNECTION_LOST: return "CONNECTION_LOST";
    case WL_DISCONNECTED:    return "DISCONNECTED";
    default:                 return "UNKNOWN";
  }
}

// Common IDF disconnect reason codes (info.wifi_sta_disconnected.reason).
const char* wifiReasonStr(uint8_t reason) {
  switch (reason) {
    case 2:   return "AUTH_EXPIRE";
    case 4:   return "ASSOC_EXPIRE";
    case 15:  return "4WAY_HANDSHAKE_TIMEOUT (often wrong password)";
    case 200: return "BEACON_TIMEOUT";
    case 201: return "NO_AP_FOUND (bad SSID, out of range, or wrong band)";
    case 202: return "AUTH_FAIL (wrong password)";
    case 203: return "ASSOC_FAIL";
    case 204: return "HANDSHAKE_TIMEOUT";
    default:  return "see esp_wifi_types.h";
  }
}

// ---------------------- WiFi event handler ---------------------------
// Registered BEFORE WiFi.begin(). This is where the real "why did it
// drop?" diagnosis lives — the status code alone can't tell you.
void onWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
  switch (event) {
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
      Serial.println("[WiFi] Associated with AP");
      break;

    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      lastConnectedMs = millis();
      Serial.printf("[WiFi] Got IP: %s  (RSSI %d dBm, ch %d)\n",
                    WiFi.localIP().toString().c_str(),
                    WiFi.RSSI(), WiFi.channel());
      break;

    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED: {
      uint8_t reason = info.wifi_sta_disconnected.reason;
      Serial.printf("[WiFi] Disconnected, reason=%u (%s)\n",
                    reason, wifiReasonStr(reason));
      // setAutoReconnect(true) handles routine retries for us; nudge it.
      WiFi.reconnect();
      break;
    }

    default:
      break;
  }
}

// ---------------------- Blocking connect (setup) ---------------------
bool wifiConnect(uint32_t timeoutMs) {
  Serial.printf("[WiFi] Connecting to \"%s\" ...\n", WIFI_SSID);

  WiFi.persistent(false);            // don't rewrite creds to NVS every boot
  WiFi.mode(WIFI_STA);               // station only
  WiFi.setSleep(false);              // keep radio responsive for a server
  WiFi.setAutoReconnect(true);
  WiFi.setHostname(WIFI_HOSTNAME);   // must be BEFORE begin()

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  uint32_t start = millis();
  wl_status_t status = WiFi.status();

  while (status != WL_CONNECTED && (millis() - start) < timeoutMs) {
    delay(250);
    Serial.printf("  [%5lu ms] status=%s\n", millis() - start, wlStatusStr(status));
    status = WiFi.status();
  }

  if (status == WL_CONNECTED) {
    lastConnectedMs = millis();
    Serial.printf("[WiFi] Connected in %lu ms\n", millis() - start);
    Serial.printf("  IP:      %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("  Gateway: %s\n", WiFi.gatewayIP().toString().c_str());
    Serial.printf("  Subnet:  %s\n", WiFi.subnetMask().toString().c_str());
    Serial.printf("  RSSI:    %d dBm\n", WiFi.RSSI());
    Serial.printf("  MAC:     %s\n", WiFi.macAddress().c_str());
    Serial.printf("  Channel: %d\n", WiFi.channel());
    return true;
  }

  Serial.printf("[WiFi] FAILED after %lu ms — last status: %s\n",
                millis() - start, wlStatusStr(WiFi.status()));
  return false;
}

// ---------------------- Loop watchdog --------------------------------
// Auto-reconnect handles transient drops. This is the failsafe for a
// headless node: if we've been down past the grace period, reboot.
void wifiWatchdog() {
  if (WiFi.status() == WL_CONNECTED) {
    lastConnectedMs = millis();
    return;
  }
  if (millis() - lastConnectedMs > DOWN_REBOOT_MS) {
    Serial.printf("[WiFi] Down > %lu ms — rebooting.\n", DOWN_REBOOT_MS);
    delay(100);
    ESP.restart();
  }
}
