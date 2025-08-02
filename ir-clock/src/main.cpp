#include <Arduino.h>
#include <cstdlib> 
#include <IRremoteESP8266.h>
#include <IRsend.h>

#include <WiFi.h>
#include <time.h>
#include "wifi_config.h"
#include "timeutil.h"

#define IR_SEND_PIN 23  // IR-Sender Pin

IRsend irsend(IR_SEND_PIN);

struct IRCode {
  const char* name;
  uint32_t code;
};

// Every code the remote sends
const IRCode ir_codes[] = {
  {"ARROW_UP", 0xFD027F80},
  {"ARROW_LEFT", 0xFB047F80},
  {"ARROW_RIGHT", 0xF9067F80},
  {"ARROW_DOWN", 0xF7087F80},
  {"CLOCK", 0xF50A7F80},
  {"ALARM", 0xE01F7F80},
  {"TEMP", 0xF30C7F80},
  {"BRIGHT", 0xF10E7F80}
};

const int IR_CODES_COUNT = sizeof(ir_codes) / sizeof(ir_codes[0]);
const int NOON_SEC = 12 * 3600; // Seconds since Midnight that equal '12:00' 
const int WAITING_INTERVAL = 1000; // Milliseconds to wait in-between sending IR-codes

uint32_t getIRCode(const char* name);
uint32_t reverseBits(uint32_t value);

void setup() {
  Serial.begin(115200);
  delay(200);

  // WiFi Setup
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.printf("Connecting with WiFI \"%s\" …\n", WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println("Connected!");
  Serial.printf("IP-Address: %s\n", WiFi.localIP().toString().c_str());

  delay(500);

  // NTP konfigurieren
  configTime(0, 0, "pool.ntp.org");  
  setenv("TZ", "CET-1CEST,M3.5.0/2,M10.5.0/3", 1);
  tzset();
  Serial.println("NTP configured, waiting for time...");

  // Auf gültige Zeit warten
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) {
    delay(500);
  }
  Serial.println("Time synchronized!");

  delay(500);

  // Infrared Sender Setup
  irsend.begin();
  Serial.println("IR-Sender ready");
}

void loop() {
  struct tm time;
  if (!getLocalTime(&time)) {
    Serial.println("Time not available");
    return;
  }

  // Calc the offset from '12:00' (Noon)
  int nowSec = getSecondsSinceMidnight(time); // Seconds since Midnight
  int deltaSec = nowSec - NOON_SEC; // >0 after noon, <0 before noon

  int diffH = calculateHourDifference(deltaSec);
  int diffM = calculateMinuteDifference(deltaSec);

  // Need to set clock to one "less" hour if its before noon
  if (diffH < 0 && diffM != 0){
    diffH = diffH - 1;
  }

  // Debug
  Serial.printf("%d h, %d m\n", diffH, diffM);

  // Prepare codes
  uint32_t codeClock     = reverseBits(getIRCode("CLOCK"));
  uint32_t codeUp        = reverseBits(getIRCode("ARROW_UP"));
  uint32_t codeDown      = reverseBits(getIRCode("ARROW_DOWN"));
  uint32_t codeRight     = reverseBits(getIRCode("ARROW_RIGHT"));

  // Switch from AM/PM to 24h Clock
  irsend.sendNEC(codeUp, 32);
  delay(WAITING_INTERVAL);

  // Enable clock setting mode
  irsend.sendNEC(codeClock, 32);
  delay(WAITING_INTERVAL);

  // Set hours
  for (int i = 0; i < std::abs(diffH); i++) {
    if(diffH < 0){
      irsend.sendNEC(codeDown, 32);
    }
    if(diffH > 0){
      irsend.sendNEC(codeUp, 32);
    }
    delay(WAITING_INTERVAL);
  }

  delay(WAITING_INTERVAL);
  delay(WAITING_INTERVAL);
  // Switch to minute-setting mode
  irsend.sendNEC(codeRight, 32);
  delay(WAITING_INTERVAL);

  // Setting Minutes -40 mins is the same as setting +20 mins
  // Determine which one requires less steps
  int minuteSteps = diffM;
  int changedDirectionSteps = 0;
  if(diffM < 0){
    changedDirectionSteps = 60 + diffM; // e.g 60 + (-40) = +20
  }
  if(diffM > 0){
    changedDirectionSteps = -(60 - diffM); // e.g -(60 - (+40)) = -20
  }

  // Choose changed direction if it requires less steps
  if(std::abs(changedDirectionSteps) < std::abs(diffM)){
    minuteSteps = changedDirectionSteps;
  }

  // Set minutes
  for (int i = 0; i < std::abs(minuteSteps); i++) {
    if(minuteSteps < 0){
      irsend.sendNEC(codeDown, 32);
    }
    if(minuteSteps > 0){
      irsend.sendNEC(codeUp, 32);
    }
    delay(WAITING_INTERVAL);
  }

  // Leave clock settings (skip date setting for now)
  irsend.sendNEC(codeClock, 32);
  delay(WAITING_INTERVAL);
  irsend.sendNEC(codeClock, 32);
  delay(WAITING_INTERVAL);
  irsend.sendNEC(codeClock, 32);

  delay(100000000);
}


// Reverse the bits of the code to be sent
uint32_t reverseBits(uint32_t value) {
  uint32_t reversed = 0;
  for(int i = 0; i < 32; i++) {
    reversed = (reversed << 1) | ((value >> i) & 1);
  }
  return reversed;
}

// Look-up the corresponding code for a button
uint32_t getIRCode(const char* name) {
  for (int i = 0; i < IR_CODES_COUNT; i++) {
    if (strcmp(ir_codes[i].name, name) == 0) {
      return ir_codes[i].code;
    }
  }
  return 0; // No code found by that name
}
