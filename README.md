# ESP32 Web Server

## Configuration and Secrets

Configuration settings and the WiFi SSID and password are stored in `include/secrets.h`
which is git ignored. Copy `include/secrets example.h` and fill it in.

## ESP32 NodeMCU v1.1

To upload to this board, wait for the `Connecting....` then press and hold the
boot button, press and release the reset button then release the boot button.

![ESP32 NodeMCU Pinout](./images/nodemcu_esp32.jpg)
