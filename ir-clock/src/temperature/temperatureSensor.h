#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include <Arduino.h>
#include "DHT.h"


class TemperatureSensor {
private:
  DHT dht;
  bool initialized;

public:
  // Constructor
  TemperatureSensor(int temperaturePin = 19);
  
  // Public interface
  bool initialize();
  float getTemperature();
  float getHumidity();
  void stringOutputValues();
};

#endif