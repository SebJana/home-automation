# MQTT Configuration Setup

## 📶 Required Configuration File

You need to create a `mqtt_config.h` file in this directory to configure your MQTT connection.

## 🔧 Setup Instructions

1. **Create the file**: `src/mqtt/mqtt_config.h`
2. **Copy the template below** and replace with your actual MQTT credentials:

```cpp
#ifndef MQTT_CONFIG_H
#define MQTT_CONFIG_H

// MQTT Access Credentials
const char* MQTT_HOST = "YOUR_HOST_IP";
const int MQTT_PORT = 1883; // Default Port
const char* MQTT_USER = "YOUR_MQTT_USERNAME";               
const char* MQTT_PASSW = "YOUR_MQTT_PASSWORD";


#endif
```

After creating this file, your ESP32 will be able to connect to your MQTT Broker.