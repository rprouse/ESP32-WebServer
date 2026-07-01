#pragma once

#include <stdint.h>
#include <WiFi.h>

// Robust WiFi connect + auto-reconnect helpers (see src/wifi.cpp).
// NOTE: named wifi_conn.h, NOT wifi.h, so it doesn't shadow the ESP32
// core's <WiFi.h> on case-insensitive filesystems (Windows/macOS).

// Blocking connect with timeout, for use in setup(). Returns true on success.
bool wifiConnect(uint32_t timeoutMs);

// Registered BEFORE WiFi.begin(). This is where the real "why did it
// drop?" diagnosis lives — the status code alone can't tell you.
void onWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info);

// Loop watchdog: reboots a headless node if WiFi stays down too long.
void wifiWatchdog();

// called from setup() to register the event handler and connect to WiFi. Does not block; auto-reconnect handles the rest.
void wifiInit();

static const uint32_t CONNECT_TIMEOUT_MS   = 15000;  // per attempt in setup()
static const uint8_t  CONNECT_MAX_ATTEMPTS = 3;      // before rebooting
static const uint32_t DOWN_REBOOT_MS       = 120000; // reboot if down this long at runtime

// Tracks when we last had a working connection (for the loop watchdog).
static uint32_t lastConnectedMs = 0;
