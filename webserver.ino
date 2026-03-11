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

extern AsyncWebServer server;
extern bool state_sta_wifi;
extern String macAddress;

extern String device_name;
extern String firm_ver;
extern int up_interval;

extern String sta_ssid;
extern String sta_pass;

bool key_sync_webserver = false;

// global server
AsyncWebServer server(80);

// ===== INIT =====
void initWebServer() {

  if (!LittleFS.begin()) {
    Serial.println("LittleFS error!");
  }

  server.serveStatic("/", LittleFS, "/")
        .setDefaultFile("dashboard.html");

  apiStatus();
  apiScanWiFi();
  apiConnectWiFi();
  apiSaveParam();
  server.begin();
}


// ===== STATUS =====
void apiStatus() {
  key_sync_webserver = true;
  // [GET] API UPDATE STATUS
  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = "{";
    json += "\"state_connected\":\"" + String(state_sta_wifi) + "\",";
    json += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
    json += "\"rssi\":\"" + String(WiFi.RSSI()) + "\",";
    json += "\"device_name\":\"" + device_name + "\",";
    json += "\"mac\":\"" + macAddress + "\",";
    json += "\"firmware\":\"" + firm_ver + "\",";
    json += "\"station_connected\":\"" + WiFi.SSID() + "\",";
    json += "\"fastInterval\":\"" + String(uploadIntervalFast) + "\",";
    json += "\"slowInterval\":\"" + String(uploadIntervalSlow) + "\",";
    json += "\"retryCount\":\"" + String(retryCount) + "\",";
    json += "\"authToken\":\"" + authToken + "\"";
    json += "}";

    request->send(200, "application/json", json);
    key_sync_webserver = false;
  });
}


// ===== SCAN =====
void apiScanWiFi() {
  key_sync_webserver = true;
  // [GET] API SCAN WIFI
  server.on("/wifi/scan", HTTP_GET, [](AsyncWebServerRequest *request){
    int n = WiFi.scanNetworks();
    Serial.println("scan wifi");
    String json = "{\"networks\":[";

    for (int i = 0; i < n; i++) {
      json += "{";
      json += "\"ssid\":\"" + WiFi.SSID(i) + "\",";
      json += "\"rssi\":" + String(WiFi.RSSI(i));
      json += "}";
      if (i < n - 1) json += ",";
    }
    json += "]}";
    Serial.println(json);
    WiFi.scanDelete();
    request->send(200, "application/json", json);
  });
}


// ===== CONNECT =====
void apiConnectWiFi() {
  // [POST] API CONNECT WIFI
  server.on("/wifi/connect", HTTP_POST, [](AsyncWebServerRequest *request){}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
    Serial.println("wifi connect");
    // 🔹 Parse JSON
    DynamicJsonDocument doc(256);
    DeserializationError err = deserializeJson(doc, data);

    if (err) {
      request->send(400, "application/json","{\"success\":false,\"msg\":\"JSON error\"}");
      return;
    }

    String ssid = doc["ssid"].as<String>();
    String pass = doc["pass"].as<String>();

    if (ssid.length() == 0) {
      request->send(400, "application/json","{\"success\":false,\"msg\":\"SSID kosong\"}");
      return;
    }

    // Disconnect wifi lama
    WiFi.disconnect(true);
    delay(300);

    // Connect baru
    WiFi.begin(ssid.c_str(), pass.c_str());

    Serial.println("Connecting to WiFi...");

    // Wait connection
    bool connected = false;

    int timeout = 20; // 10 detik
    while (timeout--) {
      esp_task_wdt_reset();
      if (WiFi.status() == WL_CONNECTED) {
        connected = true;
        Serial.print("connected");
        break;
      }
      delay(500);
    }

    // Response
    if (connected) {
      String json = "{";
      json += "\"success\":true,";
      json += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
      json += "\"rssi\":" + String(WiFi.RSSI());
      json += "}";

      request->send(200, "application/json", json);

      // saving data
      saveStringData(MEMORY_NAMESPACE_WIFI, MEMORY_STA_SSID, ssid.c_str());
      saveStringData(MEMORY_NAMESPACE_WIFI, MEMORY_STA_PASS, pass.c_str());
      sta_ssid = ssid;
      sta_pass = pass;
      Serial.println("wifi connect success");
      WiFi.setAutoReconnect(true);
      state_sta_wifi = true;
      WiFi.persistent(true);
    } else {
      request->send(200, "application/json", "{\"success\":false,\"msg\":\"Gagal connect\"}");
      Serial.println("wifi connect fail");
      WiFi.setAutoReconnect(false);
      WiFi.persistent(false);
      WiFi.disconnect(true);
      state_sta_wifi = false;
      delay(300);
    }
  });
}

// ===== SAVE PARAM =====
void apiSaveParam() {
  server.on("/params", HTTP_POST, [](AsyncWebServerRequest *request){}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, data);

    if (error) {
      Serial.print("JSON Error: ");
      Serial.println(error.c_str());
      request->send(400, "application/json", "{\"success\":false}");
      return; 
    }

    uploadIntervalFast = doc["fastInterval"] | 5;
    uploadIntervalSlow = doc["slowInterval"] | 300;
    retryCount         = doc["retry"] | 10;
    authToken          = doc["authToken"] | "";

    saveIntData(MEMORY_NAMESPACE_PARAMS, MEMORY_PARAM_FAST_INTERVAL, (int)uploadIntervalFast);
    saveIntData(MEMORY_NAMESPACE_PARAMS, MEMORY_PARAM_SLOW_INTERVAL, (int)uploadIntervalSlow);
    saveIntData(MEMORY_NAMESPACE_PARAMS, MEMORY_PARAM_COUNT_RETRY, (int)retryCount);
    saveStringData(MEMORY_NAMESPACE_PARAMS, MEMORY_PARAM_AUTH_TOKEN, authToken.c_str());

    request->send(200, "application/json", "{\"success\":true}");
    
    Blynk.disconnect();
    delay(500);
    blynk_config(authToken);
  });
}