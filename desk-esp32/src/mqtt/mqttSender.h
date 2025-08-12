#ifndef MQTT_SENDER_H
#define MQTT_SENDER_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

class MqttSender {
private:
  WiFiClient wifiClient;
  PubSubClient mqtt;
  bool connected;
  
  // Topics
  static const char* TOPIC_TEMP;
  static const char* TOPIC_HUM;
  static const char* TOPIC_WILL;

public:
  // Constructor
  MqttSender();
  
  // Public interface
  bool initialize();
  bool connect();
  void disconnect();
  bool isConnected();
  void publishTemperature(float tempC);
  void publishHumidity(float humidity);
  void publishTemperatureAndHumidity(float tempC, float humidity);
  void loop(); // Call this in main loop to maintain connection
  
private:
  // Private helper methods
  String generateClientId();
};

#endif // MQTT_SENDER_H
