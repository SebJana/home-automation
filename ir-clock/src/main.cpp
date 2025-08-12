#include <Arduino.h>
#include <time.h>
#include <WiFi.h>
#include "clock/clockSetter.h"
#include "temperature/temperatureSensor.h"
#include "mqtt/mqttSender.h"
#include "wifi/connectWifi.h"

const int IR_PIN = 23;
const int TEMP_PIN = 19;

// Create global instances
ClockSetter clockSetter(IR_PIN);
TemperatureSensor temperatureSensor(TEMP_PIN);
MqttSender mqttSender;

// RTC memory variable, survives deep sleep
RTC_DATA_ATTR bool firstRunDone = false;
const uint64_t SEC_TO_US = 1000000ULL; // Conversion factor: seconds → microseconds
const uint64_t SLEEP_DURATION = 1 * 30; // Sleep 30s

void setup() {
  Serial.begin(115200);
  delay(500);

  // WiFi Setup
  if(!connectToWifi()){
    Serial.println("Failed to connect to WiFi!");
    return;
  }

  // Only try to set clock upon first power on
  if(!firstRunDone){
    // Initialize and set clock
    firstRunDone = true;
    if (clockSetter.initialize()) {
      clockSetter.setToCurrentTime();
      Serial.println("Clock setup completed!");
    } else {
      Serial.println("Failed to initialize clock setter!");
    }
  }
  // Initialize temperature sensor
  if (temperatureSensor.initialize()){
    Serial.println("Temperature sensor initialized!");
    
    // Read sensor values
    float temp = temperatureSensor.getTemperature();
    float hum = temperatureSensor.getHumidity();
    
    Serial.printf("Temperature: %.2f°C, Humidity: %.2f%%\n", temp, hum);

    // Initialize and connect MQTT
    if (mqttSender.initialize()) {
      Serial.println("MQTT initialized!");
      if (mqttSender.connect()) {
        Serial.println("MQTT connected successfully!");
        
        // Publish sensor data if valid
        if (temp != -1 && hum != -1) {
          mqttSender.publishTemperatureAndHumidity(temp, hum);
          Serial.println("Sensor data published to MQTT!");
          
          // Allow time for MQTT messages to be sent before sleeping
          delay(500);
          mqttSender.loop(); // Process any pending MQTT operations
          delay(500);
          
          // Graceful disconnect
          mqttSender.disconnect();
          Serial.println("MQTT disconnected gracefully");
        }
      } else {
        Serial.println("MQTT connection failed!");
      }
    } else {
      Serial.println("MQTT initialization failed!");
    }

  } else {
    Serial.println("Failed to initialize temperature sensor!");
  }

  // Go to sleep
  esp_sleep_enable_timer_wakeup(SLEEP_DURATION * SEC_TO_US);
  esp_deep_sleep_start();
}

void loop() {
  // Main loop
}
