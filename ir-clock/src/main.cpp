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

void setup() {
  Serial.begin(115200);
  delay(1000);

  // WiFi Setup
  if(!connectToWifi()){
    Serial.println("Failed to connect to WiFi!");
    return;
  }

  // Initialize and set clock
  if (clockSetter.initialize()) {
    clockSetter.setToCurrentTime();
    Serial.println("Clock setup completed!");
  } else {
    Serial.println("Failed to initialize clock setter!");
  }

  if (temperatureSensor.initialize()){
    float temp = temperatureSensor.getTemperature();
    float hum = temperatureSensor.getHumidity();
    temperatureSensor.stringOutputValues();

  } else {
    Serial.println("Failed to initialize temperature sensor!");
  }
}

void loop() {
  // Main loop
}
