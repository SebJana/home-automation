#ifndef CLOCK_SETTER_H
#define CLOCK_SETTER_H

#include <Arduino.h>
#include <IRsend.h>
#include <time.h>

// Structure to hold IR remote control codes
struct IRCode {
  const char* name;
  uint32_t code;
};

// Structure to hold time difference for clock adjustment
struct ClockDifference { 
  int hourDifference;
  int minuteDifference;
};

class ClockSetter {
private:
  IRsend irsend;
  uint32_t codeClock;
  uint32_t codeUp;
  uint32_t codeDown;
  uint32_t codeRight;
  bool initialized;
  
  // Static constants
  static const int NOON_SEC = 12 * 3600; // Seconds since Midnight that equal '12:00'
  static const int WAITING_INTERVAL = 25; // Milliseconds to wait in-between sending IR-codes
  
  // Private helper methods
  uint32_t getIRCode(const char* name);
  uint32_t reverseBits(uint32_t value);
  void sendCode(uint32_t code);
  ClockDifference determineTimeOffset(struct tm time);
  int determineMinuteSteps(int diffM);
  void setClockDigits(int amount, bool directionUp);

public:
  // Constructor
  ClockSetter(int irPin = 23);
  
  // Public interface
  bool initialize();
  void setToCurrentTime();
  void setToTime(struct tm time);
  bool isInitialized() const;
};

#endif // CLOCK_SETTER_H