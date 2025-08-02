#include <unity.h>
#include "timeutil.h"

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_zero_seconds(void) {
  TEST_ASSERT_EQUAL_INT(0, calculateHourDifference(0));
  TEST_ASSERT_EQUAL_INT(0, calculateMinuteDifference(0));
}

void test_exact_hours(void) {
  TEST_ASSERT_EQUAL_INT(1, calculateHourDifference(3600));
  TEST_ASSERT_EQUAL_INT(0, calculateMinuteDifference(3600));
  TEST_ASSERT_EQUAL_INT(5, calculateHourDifference(5 * 3600));
  TEST_ASSERT_EQUAL_INT(0, calculateMinuteDifference(5 * 3600));
}

void test_mixed_after_noon(void) {
  int secs = 1 * 3600 + 15 * 60;  // 1h15m
  TEST_ASSERT_EQUAL_INT(1, calculateHourDifference(secs));
  TEST_ASSERT_EQUAL_INT(15, calculateMinuteDifference(secs));
}

void test_mixed_before_noon(void) {
  int secs = -1 * 3600 + 15 * 60;  // 1h15m
  TEST_ASSERT_EQUAL_INT(-1, calculateHourDifference(secs));
  TEST_ASSERT_EQUAL_INT(-15, calculateMinuteDifference(secs));
}

void test_less_than_hour(void) {
  TEST_ASSERT_EQUAL_INT(0, calculateHourDifference(59 * 60));
  TEST_ASSERT_EQUAL_INT(59, calculateMinuteDifference(59 * 60));
}

void setup() {
  UNITY_BEGIN();
  RUN_TEST(test_zero_seconds);
  RUN_TEST(test_exact_hours);
  RUN_TEST(test_mixed_after_noon);
  RUN_TEST(test_mixed_before_noon);
  RUN_TEST(test_less_than_hour);
  UNITY_END();
}