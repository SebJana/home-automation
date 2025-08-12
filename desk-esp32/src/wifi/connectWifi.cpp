#include <WiFi.h>
#include "wifi_config.h"

bool connectToWifi(){
    int retryCounter = 0;
    // WiFi Setup
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.printf("Connecting with WiFI \"%s\" …\n", WIFI_SSID);
    while (retryCounter < 5 && WiFi.status() != WL_CONNECTED) {
        retryCounter++;
        delay(500);
    }

    if(WiFi.status() == WL_CONNECTED){
        Serial.println("Connected!");
        Serial.printf("IP-Address: %s\n", WiFi.localIP().toString().c_str());
        return true;
    }
    return false;
}