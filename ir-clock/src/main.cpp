#include <Arduino.h>
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

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
  irsend.begin();
  Serial.println("IR sender ready.");
}

void loop() {  
  uint32_t code = 0xFD027F80;
  
  // Test 2: Bit-reversed
  uint32_t reversed_code = reverseBits(code);
  irsend.sendNEC(reversed_code, 32);

  delay(1000);
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