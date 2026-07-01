#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

String processor(const String& var) {
  if (var == "UTC") {
    time_t now = time(nullptr); // UTC epoch — the canonical instant, always

    struct tm utc;
    gmtime_r(&now, &utc); // UTC, ignores TZ entirely

    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &utc);
    return String(buf);
  }
  if (var == "LOCAL") {
    time_t now = time(nullptr); // UTC epoch — the canonical instant, always

    struct tm local;
    localtime_r(&now, &local); // local, applies TZ + DST

    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S %Z", &local);
    return String(buf);
  }
  return String();  // unknown placeholder -> empty
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html><body>
  <h1>ESP32 Web Server</h1>
  <h2>UTC Time</h2>
  <p>%UTC%</p>
  <h2>Local Time</h2>
  <p>%LOCAL%</p>
</body></html>
)rawliteral";

void webServerSetup() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", index_html, processor);
  });
  server.begin();
}
