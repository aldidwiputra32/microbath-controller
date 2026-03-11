#define BLYNK_TEMPLATE_ID             "CLK-KDS-MICROBATH"
#define BLYNK_TEMPLATE_NAME           "DEVICE_ESP32"
#define STA_SSID_DEFAULT              ""
#define STA_PASS_DEFAULT              ""
#define AP_SSID_DEFAULT               "calibration_system"
#define AP_PASS_DEFAULT               "xbrasikds123"
#define PARAM_FAST_INTEVAL            5
#define PARAM_SLOW_INTEVAL            300
#define PARAM_RETRY_COUNT             3
#define PARAM_AUTH_TOKEN              ""
#define MEMORY_NAMESPACE_WIFI         "wifi"
#define MEMORY_NAMESPACE_PARAMS       "params"
#define MEMORY_STA_SSID               "sta_ssid"
#define MEMORY_STA_PASS               "sta_pass"
#define MEMORY_AP_SSID                "ap_ssid"
#define MEMORY_AP_PASS                "ap_pass"
#define MEMORY_PARAM_FAST_INTERVAL    "fast_interval"
#define MEMORY_PARAM_SLOW_INTERVAL    "slow_interval"
#define MEMORY_PARAM_COUNT_RETRY      "count_retry"
#define MEMORY_PARAM_AUTH_TOKEN       "auth_token"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Preferences.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <NetworkClient.h>
#include "esp_task_wdt.h"
#include <ESPmDNS.h>

BlynkTimer timer;
int idTimerUploadData;

String device_name = "ESP32-Calibration_System";
String firm_ver = "v1.0";
String authToken = "";
bool blynkConfigured = false;

void setup() {
  Serial.begin(115200);
  initWiFi();        
  initMDNS();          
  loadParams();
  initWebServer();
  blynk_config(authToken);
  idTimerUploadData = timer.setInterval(5000L, uploadData);
  Serial.println("System ready");
}

void loop() {
  // reconnect logic
  handleWiFi();
  // handle blynk
  if(blynkConfigured && WiFi.status() == WL_CONNECTED){
    Blynk.run();
    timer.run();
  }      
}