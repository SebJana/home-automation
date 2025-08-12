# WiFi Configuration Setup

## 📶 Required Configuration File

You need to create a `wifi_config.h` file in this directory to configure your WiFi connection.

## 🔧 Setup Instructions

1. **Create the file**: `src/wifi/wifi_config.h`
2. **Copy the template below** and replace with your actual WiFi credentials:

```cpp
#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

// WIFI Access Credentials
static const char* WIFI_SSID     = "YOUR_WIFI_NETWORK_NAME";
static const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

#endif
```

After creating this file, your ESP32 will be able to connect to your WiFi network.