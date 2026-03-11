#include <WiFi.h>
#include <WiFiClient.h>
// #include <BlynkSimpleEsp32.h>
#include <Preferences.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <NetworkClient.h>
#include "esp_task_wdt.h"
#include <ESPmDNS.h>

void initMDNS() {

  if (!MDNS.begin("calibration-system")) {
    Serial.println("mDNS failed");
    return;
  }

  MDNS.addService("http", "tcp", 80);
  Serial.println("mDNS ready");
}