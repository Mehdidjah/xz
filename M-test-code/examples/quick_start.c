#include "../src/stest.h"
#include <stdlib.h>
#include <string.h>

void test_basic_math() {
  int sum = 2 + 2;
  assert_int_equal(4, sum);
  
  int product = 3 * 5;
  assert_int_equal(15, product);
}

void test_string_basics() {
  const char* greeting = "Hello, World!";
  assert_string_contains("Hello", greeting);
  assert_string_starts_with("Hello", greeting);
  assert_string_ends_with("World!", greeting);
}

void test_pointers() {
  void* ptr = malloc(100);
  assert_not_null(ptr);
  
  free(ptr);
  assert_null(ptr);
}

void test_floating_point() {
  double pi = 3.14159;
  assert_double_equal(3.14159, pi, 0.00001);
  
  float value = 1.5f;
  assert_float_equal(1.5f, value, 0.001f);
}

void test_fixture_basics() {
  test_fixture_start();
  run_test(test_basic_math);
  run_test(test_string_basics);
  run_test(test_pointers);
  run_test(test_floating_point);
  test_fixture_end();
}

int main(int argc, char** argv) {
  return stest_testrunner(argc, argv, test_fixture_basics, NULL, NULL);
}

