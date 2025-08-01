#include <Arduino.h>
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

#include <WiFi.h>
#include <time.h>
#include "wifi_config.h"

#define IR_SEND_PIN 23  // Passenden Pin für deinen IR LED-Sender wählen

IRsend irsend(IR_SEND_PIN);

uint32_t reverseBits(uint32_t value);

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

uint32_t getIRCode(const char* name);

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
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Time not available");
    return;
  }

  // 1) Berechne Sekunden seit Mitternacht und Noon-Offset
  int nowSec   = timeinfo.tm_hour * 3600 
               + timeinfo.tm_min  *   60 
               + timeinfo.tm_sec;
  const int noonSec = 12 * 3600;       // 12:00 Uhr

  int deltaSec = nowSec - noonSec;      // >0 nach Mittag, <0 vor Mittag
  bool afterNoon = (deltaSec >= 0);
  int absSec = afterNoon ? deltaSec : -deltaSec;

  // Stunden und Minuten Unterschied
  int diffH = absSec / 3600;
  int diffM = (absSec % 3600) / 60;

  // Debug
  if (afterNoon) {
    Serial.printf("Seit 12:00: %d h, %d m\n", diffH, diffM);
  } else {
    Serial.printf("Bis 12:00: %d h, %d m\n", diffH, diffM);
  }

  // 2) Codes vorbereiten
  uint32_t codeClock     = reverseBits(getIRCode("CLOCK"));
  uint32_t codeUp        = reverseBits(getIRCode("ARROW_UP"));
  uint32_t codeDown      = reverseBits(getIRCode("ARROW_DOWN"));
  uint32_t codeRight     = reverseBits(getIRCode("ARROW_RIGHT"));

  // 3) In Clock-Setting-Modus wechseln
  irsend.sendNEC(codeClock, 32);
  delay(200);

  // 4) Stunden einstellen
  for (int i = 0; i <= diffH; i++) {
    irsend.sendNEC(afterNoon ? codeUp : codeDown, 32);
    delay(100);
  }

  // 5) Auf Minuten-Modus umschalten
  irsend.sendNEC(codeRight, 32);
  delay(200);

  // 6) Minuten einstellen
  for (int i = 0; i <= diffM; i++) {
    irsend.sendNEC(afterNoon ? codeUp : codeDown, 32);
    delay(100);
  }

  // 7) Einstell-Modus verlassen / speichern
  irsend.sendNEC(codeClock, 32);
  delay(200);

  irsend.sendNEC(codeClock, 32);
  delay(200);

  // 8) Sleep oder nächste Iteration
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

uint32_t getIRCode(const char* name) {
  for (int i = 0; i < IR_CODES_COUNT; i++) {
    if (strcmp(ir_codes[i].name, name) == 0) {
      return ir_codes[i].code;
    }
  }
  return 0; // No code found by that name
}