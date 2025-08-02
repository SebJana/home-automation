#include "timeutil.h"
#include <cstdlib> 
#include <time.h>

int getSecondsSinceMidnight(struct tm time){
  int nowSec   = time.tm_hour * 3600  // Hours
               + time.tm_min  *   60  // Minutes
               + time.tm_sec  *    1; // Seconds 
  return nowSec;
}

int calculateHourDifference(int deltaSec){
  if(deltaSec == 0){ // Noon
    return 0;
  }
  if(deltaSec < 0){ // Before noon
    return -(std::abs(deltaSec) / 3600); // Difference is negative
  }
  // After noon
  return deltaSec / 3600;
}

int calculateMinuteDifference(int deltaSec){
  int remainingSeconds = std::abs(deltaSec) % 3600;

  if(remainingSeconds == 0){ // Full hour
    return 0;
  }
  if(deltaSec < 0){ // Before noon
    return -(std::abs(remainingSeconds) / 60); // Difference is negative
  }
  // After noon
  return remainingSeconds / 60;
}