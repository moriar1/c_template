#include "math_utils.h"
#include "unity.h"

void setUp(void) {}

void tearDown(void) {}

static void test_add_two_numbers(void) { TEST_ASSERT_EQUAL_INT(2, add(2, 3)); }

static void test_add_negative_numbers(void) {
  TEST_ASSERT_EQUAL_INT(-5, add(-2, -3));
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_add_two_numbers);
  RUN_TEST(test_add_negative_numbers);

  return UNITY_END();
}
