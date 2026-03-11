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
// ===== GLOBAL =====
bool state_sta_wifi = false;
unsigned long lastReconnect = 0;

String sta_ssid;
String sta_pass;
String macAddress;
volatile int apClientCount = 0;

// ===== EVENT =====
void WiFiEvent(WiFiEvent_t event) {
  switch (event) {

    case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
      apClientCount++;
      Serial.print("Client Connected | Total: ");
      Serial.println(apClientCount);
      break;

    case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
      if (apClientCount > 0) apClientCount--;
      Serial.print("Client Disconnected | Total: ");
      Serial.println(apClientCount);
      break;

    default:
      break;
  }
}


// ===== INIT =====
void initWiFi() {

  WiFi.disconnect(true, true);
  delay(300);

  WiFi.mode(WIFI_AP_STA);
  WiFi.setSleep(false);
  WiFi.onEvent(WiFiEvent);
  delay(500);
  macAddress = WiFi.macAddress();

  startSoftAP();
  loadSTAConfig();
  connectSTA();
}


// ===== AP =====
void startSoftAP() {
  bool ok = WiFi.softAP(AP_SSID_DEFAULT, AP_PASS_DEFAULT);

  if (ok) {
    Serial.println("SoftAP started");
    Serial.println(WiFi.softAPIP());
  } else {
    Serial.println("SoftAP failed");
  }
}


// ===== LOAD =====
void loadSTAConfig() {
  sta_ssid = loadStringData(MEMORY_NAMESPACE_WIFI, MEMORY_STA_SSID, "");
  sta_pass = loadStringData(MEMORY_NAMESPACE_WIFI, MEMORY_STA_PASS, "");
}


// ===== CONNECT =====
void connectSTA() {

  if (sta_ssid == "") {
    Serial.println("No data STA Mode");
    return;
  }

  Serial.println("Scanning WiFi...");

  int n = WiFi.scanNetworks(false, true); // async=false, hidden=true
  bool ssidFound = false;

  for (int i = 0; i < n; i++) {
    if (WiFi.SSID(i) == sta_ssid) {
      ssidFound = true;
      break;
    }
  }

  WiFi.scanDelete(); // penting supaya RAM bersih

  if (!ssidFound) {
    Serial.println("Saved SSID not found. Skip connect.");
    state_sta_wifi = false;
    WiFi.setAutoReconnect(false);
    WiFi.persistent(false);
    WiFi.disconnect(true);
    delay(300);
    return;
  }

  Serial.println("SSID found. Connecting STA...");
  WiFi.begin(sta_ssid.c_str(), sta_pass.c_str());

  int timeout = 20; // 10 detik
  while (WiFi.status() != WL_CONNECTED && timeout--) {
    yield();
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nSTA Connected!");
    Serial.print("STA IP: ");
    Serial.println(WiFi.localIP());
    state_sta_wifi = true;
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
  } else {
    Serial.println("\nSTA failed");
    state_sta_wifi = false;
    WiFi.setAutoReconnect(false);
    WiFi.persistent(false);
    WiFi.disconnect(true);
    delay(300);
  }
}


// ===== SAVE =====
void saveSTA(String ssid, String pass) {
  saveStringData(MEMORY_NAMESPACE_WIFI, MEMORY_STA_SSID, ssid.c_str());
  saveStringData(MEMORY_NAMESPACE_WIFI, MEMORY_STA_PASS, pass.c_str());

  sta_ssid = ssid;
  sta_pass = pass;
}


// ===== RECONNECT =====
void handleWiFi() {
  if ((WiFi.status() != WL_CONNECTED) && (apClientCount == 0)) {
    if (millis() - lastReconnect > 60000){
      lastReconnect = millis();
      connectSTA();
    }
  }else if(WiFi.status() == WL_CONNECTED){

  }
}