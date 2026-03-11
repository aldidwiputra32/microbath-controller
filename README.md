# Microbath Controller (ESP32)

Firmware **ESP32-based Microbath Controller** untuk sistem monitoring dan kontrol berbasis IoT yang dilengkapi dengan:

- Web Dashboard
- WiFi Configuration
- Blynk Cloud Integration
- Dynamic Sensor Upload Interval
- Persistent Configuration Storage (NVS)
- mDNS Device Discovery
- LittleFS Web Interface

Project ini dirancang sebagai **firmware dasar untuk perangkat microbath / temperature control system** yang membutuhkan monitoring lokal melalui web dashboard serta monitoring jarak jauh melalui cloud.

# Main Features

## Web Dashboard

ESP32 menyediakan **web interface** yang dapat diakses melalui browser untuk:

- Monitoring status perangkat
- Monitoring sensor
- Konfigurasi parameter
- Scan dan konfigurasi WiFi
- Monitoring koneksi Blynk

Dashboard disimpan di **LittleFS filesystem**.

Lokasi file web:

```
/data/dashboard.html
/data/app.js
/data/index.min.css
```

# WiFi System

Device mendukung dua mode koneksi:

## Station Mode

ESP32 terhubung ke jaringan WiFi router.

Digunakan untuk:

- akses internet
- koneksi Blynk
- remote monitoring

## Access Point Mode

Jika WiFi gagal terhubung, device akan membuat hotspot sendiri.

Default configuration:

```
SSID : calibration_system
PASS : xbrasikds123
```

User dapat mengakses dashboard melalui hotspot ini untuk melakukan konfigurasi.

File terkait:

```
wifi.ino
```

# Web Server

Web server menggunakan **ESPAsyncWebServer** untuk performa yang lebih baik dibanding webserver standar.

Fitur server:

- serve dashboard dari LittleFS
- API endpoint
- WiFi scanning
- parameter configuration
- sensor monitoring

Library yang digunakan:

```
ESPAsyncWebServer
AsyncTCP
```

Inisialisasi server:

```cpp
AsyncWebServer server(80);
```

File terkait:

```
webserver.ino
```

# Blynk Integration

Firmware mendukung integrasi dengan **Blynk IoT platform** untuk monitoring cloud.

Parameter yang digunakan:

- Auth Token
- Upload Interval Fast
- Upload Interval Slow
- Retry Count

Data sensor dapat diupload secara periodik ke **Blynk Virtual Pin**.

File terkait:

```
blynk_handler.ino
```

# Sensor System

Sensor handler membaca nilai sensor dan menentukan **interval upload data secara dinamis**.

Logika dasar:

- Jika nilai sensor mendekati setpoint → upload lebih cepat
- Jika jauh dari setpoint → upload lebih lambat

Contoh logika:

```cpp
error = abs(setPointRead - temperature);

if(error <= TRESHOLD){
    timer.changeInterval(idTimerUploadData, uploadIntervalFast);
}else{
    timer.changeInterval(idTimerUploadData, uploadIntervalSlow);
}
```

File terkait:

```
sensor.ino
```

# Memory Storage

Parameter perangkat disimpan menggunakan **ESP32 Preferences (NVS)** sehingga tetap tersimpan setelah restart.

Parameter yang disimpan:

- WiFi SSID
- WiFi Password
- Blynk Auth Token
- Upload Interval
- Retry Count

Contoh namespace:

```
wifi
params
```

File terkait:

```
memory.ino
```

# mDNS Support

Device dapat diakses melalui hostname tanpa mengetahui IP address.

Contoh akses:

```
http://microbath-controller.local
```

Ini mempermudah akses device di jaringan lokal.

File terkait:

```
mdns.ino
```

# Project Structure

```
microbath_controller
│
├── microbath_controller.ino    # Main firmware
│
├── wifi.ino                    # WiFi management
├── webserver.ino               # Web server
├── blynk_handler.ino           # Blynk integration
├── sensor.ino                  # Sensor processing
├── memory.ino                  # NVS storage
├── mdns.ino                    # mDNS service
│
└── data                        # Web dashboard
    ├── dashboard.html
    ├── app.js
    └── index.min.css
```

# Default Configuration

Default parameter pada firmware:

```cpp
#define AP_SSID_DEFAULT "calibration_system"
#define AP_PASS_DEFAULT "xbrasikds123"

#define PARAM_FAST_INTEVAL 5
#define PARAM_SLOW_INTEVAL 300
#define PARAM_RETRY_COUNT 3
```

Parameter ini dapat diubah melalui dashboard web.

# Upload Web Files

Sebelum upload firmware, upload file web ke **LittleFS**.

Gunakan plugin:

```
ESP32 LittleFS Data Upload
```

Folder yang akan diupload:

```
/data
```

# Required Libraries

Pastikan library berikut sudah terinstall di Arduino IDE:

```
ESPAsyncWebServer
AsyncTCP
ArduinoJson
Blynk
ESPmDNS
LittleFS
Preferences
```

# Typical Workflow

1. Flash firmware ke ESP32
2. Upload file web ke LittleFS
3. Hubungkan ke Access Point device
4. Konfigurasi WiFi melalui dashboard
5. Device akan terkoneksi ke router
6. Monitoring bisa dilakukan melalui:
   - Web dashboard
   - Blynk cloud

# Example Applications

Firmware ini cocok digunakan untuk:

- Microbath temperature controller
- Industrial calibration system
- IoT monitoring device
- Remote temperature monitoring
- Embedded web-enabled controller

Firmware developed for **ESP32 Microbath Controller System**
