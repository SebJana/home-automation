#include <Arduino.h>
#include <cstdlib> 
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <time.h>
#include "timeutil.h"
#include "clockSetter.h"

#define IR_SEND_PIN 23  // IR-Sender Pin

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

// Constructor
ClockSetter::ClockSetter(int irPin) : irsend(irPin), initialized(false) {
  codeClock = 0;
  codeUp = 0;
  codeDown = 0;
  codeRight = 0;
}

bool ClockSetter::initialize() {
  // Configure NTP
  configTime(0, 0, "pool.ntp.org");  
  setenv("TZ", "CET-1CEST,M3.5.0/2,M10.5.0/3", 1);
  tzset();
  Serial.println("NTP configured, waiting for time...");

  // Wait till a valid time is received
  int retryCounter = 0;
  struct tm time;
  while (retryCounter < 5 && !getLocalTime(&time)) {
    delay(500);
  }

  if(!getLocalTime(&time)){
    Serial.println("Failed to synchronize the time!");
    return false;
  }

  Serial.println("Time synchronized!");

  // Infrared Sender Setup
  irsend.begin();
  Serial.println("IR-Sender ready");

  // Prepare Infrared Codes
  codeClock = reverseBits(getIRCode("CLOCK"));
  codeUp    = reverseBits(getIRCode("ARROW_UP"));
  codeDown  = reverseBits(getIRCode("ARROW_DOWN"));
  codeRight = reverseBits(getIRCode("ARROW_RIGHT"));

  initialized = true;
  return true;
}

// Reverse the bits of the code to be sent
uint32_t ClockSetter::reverseBits(uint32_t value) {
  uint32_t reversed = 0;
  for(int i = 0; i < 32; i++) {
    reversed = (reversed << 1) | ((value >> i) & 1);
  }
  return reversed;
}

// Look-up the corresponding code for a button
uint32_t ClockSetter::getIRCode(const char* name) {
  for (int i = 0; i < IR_CODES_COUNT; i++) {
    if (strcmp(ir_codes[i].name, name) == 0) {
      return ir_codes[i].code;
    }
  }
  return 0; // No code found by that name
}

void ClockSetter::sendCode(uint32_t code){
  irsend.sendNEC(code, 32);
  delay(WAITING_INTERVAL);
}

ClockDifference ClockSetter::determineTimeOffset(struct tm time){
  int nowSec = getSecondsSinceMidnight(time); // Seconds since Midnight
  int deltaSec = nowSec - NOON_SEC; // >0 after noon, <0 before noon

  int diffH = calculateHourDifference(deltaSec);
  int diffM = calculateMinuteDifference(deltaSec);

  // Need to set clock to one "less" hour if its before noon 
  // e.g it's 10:40 then the full hour diff to 12:00 is only one --> hour clock needs to be changed by two hours
  if (diffH < 0 && diffM != 0){
    diffH = diffH - 1;
  }

  return {diffH, diffM};
}

void ClockSetter::setClockDigits(int amount, bool directionUp){
  // Determine clock setting direction code
  uint32_t code = 0;
  if(directionUp){
    code = codeUp;
  }
  else{
    code = codeDown;
  }

  // Set the clock by the amount specified
  for (int i = 0; i < std::abs(amount); i++) {
    sendCode(code);
  }
}

int ClockSetter::determineMinuteSteps(int diffM){
  // Setting Minutes -40 mins is the same as setting +20 mins
  // Determine which one requires less steps
  int changedDirectionSteps = 0;
  if(diffM < 0){
    changedDirectionSteps = 60 + diffM; // e.g 60 + (-40) = +20
  }
  if(diffM > 0){
    changedDirectionSteps = -(60 - diffM); // e.g -(60 - (+40)) = -20
  }

  // Choose changed direction if it requires less steps
  if(std::abs(changedDirectionSteps) < std::abs(diffM)){
    return changedDirectionSteps;
  }
  return diffM; // Default if the changed direction isn't faster
}

void ClockSetter::setToTime(struct tm time){
  if (!initialized) {
    Serial.println("ClockSetter not initialized!");
    return;
  }

  ClockDifference cd = determineTimeOffset(time);
  int diffH = cd.hourDifference;
  int diffM = cd.minuteDifference;

  // Debug
  Serial.printf("%d h, %d m\n", diffH, diffM);

  // Switch from AM/PM to 24h Clock
  sendCode(codeUp);
  // Enable clock setting mode
  sendCode(codeClock);

  // Set hours
  bool hourDirection = (diffH > 0);
  setClockDigits(diffH, hourDirection);

  // Switch to minute-setting mode
  sendCode(codeRight);

  // Set minutes
  int minuteDigitSteps = determineMinuteSteps(diffM);
  bool minuteDirection = (minuteDigitSteps > 0);
  setClockDigits(minuteDigitSteps, minuteDirection);

  // Leave clock settings (skip date setting for now)
  for(int i = 0; i < 3; i++){
    sendCode(codeClock);
  }
}

void ClockSetter::setToCurrentTime(){
  struct tm time;
  if (getLocalTime(&time)) {
    setToTime(time);
  } else {
    Serial.println("Failed to get current time!");
  }
}

bool ClockSetter::isInitialized() const {
  return initialized;
}
