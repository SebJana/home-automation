#include <Arduino.h>
#include <time.h>
#include <WiFi.h>
#include "clock/clockSetter.h"
#include "temperature/temperatureSensor.h"
#include "wifi/connectWifi.h"

const int IR_PIN = 23;
const int TEMP_PIN = 19;

// Create global clock setter instance
ClockSetter clockSetter(IR_PIN);
TemperatureSensor temperatureSensor(TEMP_PIN);

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

  if (temperatureSensor.initialize()){
    float temp = temperatureSensor.getTemperature();
    float hum = temperatureSensor.getHumidity();
    temperatureSensor.stringOutputValues();

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
