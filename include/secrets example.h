#pragma once

// WiFi credentials
static const char* WIFI_SSID     = "your-ssid";
static const char* WIFI_PASS     = "your-pass";
static const char* WIFI_HOSTNAME = "sensor-node";

// NTP server settings
static const char *TZ_LOCAL = "MST7MDT,M3.2.0,M11.1.0"; // Fernie / Mountain Time
static const char *NTP_1 = "time.nrc.ca";               // National Research Council Canada
static const char *NTP_2 = "pool.ntp.org";
static const char *NTP_3 = "time.cloudflare.com";
