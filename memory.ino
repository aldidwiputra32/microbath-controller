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

Preferences prefs;

// ==========================================
// FUNGSI GENERAL (SERBAGUNA)
// ==========================================

/**
 * Fungsi Umum untuk MENYIMPAN data Integer
 * @param namespace Nama kelompok data (contoh: "wifi", "monitoring")
 * @param key       Nama variabel (contoh: "ssid", "threshold")
 * @param value     Nilai yang ingin disimpan
 */
void saveIntData(const char* namespaceName, const char* key, int value) {
  prefs.begin(namespaceName, false); // false = Mode Tulis
  prefs.putInt(key, value);
  prefs.end();
}

/**
 * Fungsi Umum untuk MEMBACA data Integer
 * @param namespace Nama kelompok data
 * @param key       Nama variabel
 * @param defaultVal Nilai default jika data belum ada
 * @return          Nilai yang tersimpan atau defaultVal
 */
int loadIntData(const char* namespaceName, const char* key, int defaultVal) {
  prefs.begin(namespaceName, true); // true = Mode Baca Saja (Aman)
  int value = prefs.getInt(key, defaultVal);
  prefs.end();
  return value;
}

/**
 * Fungsi Umum untuk MENYIMPAN data String
 */
void saveStringData(const char* namespaceName, const char* key, const char* value) {
  prefs.begin(namespaceName, false);
  prefs.putString(key, value);
  prefs.end();
}

/**
 * Fungsi Umum untuk MEMBACA data String
 */
String loadStringData(const char* namespaceName, const char* key, const char* defaultVal) {
  prefs.begin(namespaceName, true);
  String value = prefs.getString(key, defaultVal);
  prefs.end();
  return value;
}

