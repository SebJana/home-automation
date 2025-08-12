#include "mqttSender.h"
#include "mqtt_config.h"

// Static topic definitions
const char* MqttSender::TOPIC_TEMP = "esp32/desk/temperature";
const char* MqttSender::TOPIC_HUM = "esp32/desk/humidity";
const char* MqttSender::TOPIC_WILL = "esp32/desk/state";

// Constructor
MqttSender::MqttSender() : mqtt(wifiClient), connected(false) {
}

bool MqttSender::initialize() {
  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  return true;
}

bool MqttSender::connect() {
  if (mqtt.connected()) {
    connected = true;
    return true;
  }

  String clientId = generateClientId();
  const char* willMsg = "offline";
  bool cleanSession = true;

  Serial.printf("MQTT: connect to %s:%u\n", MQTT_HOST, MQTT_PORT);
  bool ok = mqtt.connect(
    clientId.c_str(),
    MQTT_USER, MQTT_PASSW,
    TOPIC_WILL, 1, true,      // willTopic, qos=1, retain=true
    willMsg,
    cleanSession
  );

  if (ok) {
    Serial.println("MQTT connected");
    connected = true;
    // Publish initial state
    mqtt.publish(TOPIC_WILL, "online", true);
  } else {
    Serial.printf("MQTT connect failed, rc=%d\n", mqtt.state());
    connected = false;
  }
  
  return ok;
}

void MqttSender::disconnect() {
  if (mqtt.connected()) {
    mqtt.publish(TOPIC_WILL, "offline", true);
    mqtt.disconnect();
  }
  connected = false;
}

bool MqttSender::isConnected() {
  return connected && mqtt.connected();
}

void MqttSender::publishTemperature(float tempC) {
  if (!isConnected()) {
    Serial.println("MQTT not connected, cannot publish temperature");
    return;
  }

  char buf[32];
  dtostrf(tempC, 0, 2, buf);
  
  bool result = mqtt.publish(TOPIC_TEMP, buf, false);
  if (result) {
    Serial.println("Temperature published successfully");
  } else {
    Serial.println("Failed to publish temperature");
  }
}

void MqttSender::publishHumidity(float humidity) {
  if (!isConnected()) {
    Serial.println("MQTT not connected, cannot publish humidity");
    return;
  }

  char buf[32];
  dtostrf(humidity, 0, 2, buf);

  bool result = mqtt.publish(TOPIC_HUM, buf, false);
  if (result) {
    Serial.println("Humidity published successfully");
  } else {
    Serial.println("Failed to publish humidity");
  }
}

void MqttSender::publishTemperatureAndHumidity(float tempC, float humidity) {
  if (!isConnected()) {
    Serial.println("MQTT not connected, cannot publish data");
    return;
  }

  publishTemperature(tempC);
  delay(100); // Small delay between publishes
  publishHumidity(humidity);
  Serial.println("Both values published");
}

void MqttSender::loop() {
  mqtt.loop(); // Maintain MQTT connection
}

String MqttSender::generateClientId() {
  return String("esp32-") + String((uint32_t)ESP.getEfuseMac(), HEX);
}






