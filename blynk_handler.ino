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
  
bool wifiConnected = false;

uint32_t uploadIntervalFast = 5;
uint32_t uploadIntervalSlow = 300;
uint8_t retryCount = 10;

void loadParams(){
  uploadIntervalFast = loadIntData(MEMORY_NAMESPACE_PARAMS, MEMORY_PARAM_FAST_INTERVAL, PARAM_FAST_INTEVAL);
  uploadIntervalSlow = loadIntData(MEMORY_NAMESPACE_PARAMS, MEMORY_PARAM_SLOW_INTERVAL, PARAM_SLOW_INTEVAL);
  retryCount = loadIntData(MEMORY_NAMESPACE_PARAMS, MEMORY_PARAM_COUNT_RETRY, PARAM_RETRY_COUNT);
  authToken = loadStringData(MEMORY_NAMESPACE_PARAMS, MEMORY_PARAM_AUTH_TOKEN, PARAM_AUTH_TOKEN);
}

void blynk_config(String data){
  char authToken_char[40];
  data.toCharArray(authToken_char, sizeof(authToken_char));
  if(strlen(authToken_char) < 10){
    Serial.println("Auth token empty");
    return;
  }
  Blynk.config(authToken_char);
  if(Blynk.connect(10000)){
    Serial.println("Blynk Connected");
    blynkConfigured = true;
    Blynk.syncVirtual(V1);
  }else{
    Serial.println("Blynk Failed");
    blynkConfigured = false;
  }
}

double setpointWrite = 0;
BLYNK_WRITE(V1){
  // get data setpoint from blynk server
  setpointWrite = param.asDouble();

  // ganti code mu disini [start]
  Serial.print("setpoint write: ");
  Serial.println(setpointWrite,2); // tampilkan 4 digit desimal
  // ganti code mu disini [finish]
}

void uploadData(){
  // temperature sensor
  Blynk.virtualWrite(V0, readSensor());
  // temperature setpoint (from microbath)
  Blynk.virtualWrite(V2, readSetpoint());  
}

