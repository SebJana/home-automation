
#include <Arduino.h>
#include "DHT.h"
#include "temperatureSensor.h"

TemperatureSensor::TemperatureSensor(int temperaturePin) : dht(temperaturePin, DHT11), initialized(false) {}

bool TemperatureSensor::initialize() {
  // Temp Sensor Setup
  dht.begin();
  Serial.println("Sensor ready");
  initialized = true;
  return true;
}

float TemperatureSensor::getTemperature() {
  if (!initialized) {
    Serial.println("❌ TemperatureSensor not initialized!");
    return -1;
  }

  float temp = dht.readTemperature(); // Celsius

  if (isnan(temp)) {
      Serial.println("❌ Error reading temperature from sensor");
      return -1;
  } else {
      return temp;
  }
}

float TemperatureSensor::getHumidity() {
  if (!initialized) {
    Serial.println("❌ TemperatureSensor not initialized!");
    return -1;
  }

  float hum = dht.readHumidity();    // %
  if (isnan(hum)) {
      Serial.println("❌ Error reading humidity from sensor");
      return -1;
  } else {
      return hum;
  }
}

void TemperatureSensor::stringOutputValues() {
  float temp = getTemperature();
  float hum = getHumidity();

  Serial.print("🌡 Temperature: ");
  Serial.print(temp);
  Serial.print(" °C  💧 Humidity: ");
  Serial.print(hum);
  Serial.println(" %");
}
