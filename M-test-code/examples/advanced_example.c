#include "../src/stest.h"
#include "../src/stest_utils.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

static int global_counter = 0;
static char* dynamic_string = NULL;

void suite_setup_example() {
  global_counter = 100;
}

void suite_teardown_example() {
  global_counter = 0;
}

void fixture_setup_example() {
  dynamic_string = stest_string_duplicate("Test String");
  assert_not_null(dynamic_string);
}

void fixture_teardown_example() {
  stest_safe_free(dynamic_string);
  dynamic_string = NULL;
}

void test_with_setup_teardown() {
  assert_int_equal(100, global_counter);
  assert_string_equal("Test String", dynamic_string);
  
  global_counter++;
  assert_int_equal(101, global_counter);
}

void test_string_list_operations() {
  stest_string_list_t* list = stest_string_list_create();
  assert_not_null(list);
  
  stest_string_list_add(list, "first");
  stest_string_list_add(list, "second");
  stest_string_list_add(list, "third");
  
  assert_true(stest_string_list_contains(list, "first"));
  assert_true(stest_string_list_contains(list, "second"));
  assert_false(stest_string_list_contains(list, "fourth"));
  
  stest_string_list_free(list);
}

void test_memory_operations() {
  char buffer1[100];
  char buffer2[100];
  
  memset(buffer1, 0xAA, 100);
  memset(buffer2, 0xAA, 100);
  
  assert_memory_equal(buffer1, buffer2, 100);
  
  buffer2[50] = 0xBB;
  assert_memory_not_equal(buffer1, buffer2, 100);
}

void test_range_assertions() {
  assert_in_range(5, 1, 10);
  assert_in_range(1, 1, 10);
  assert_in_range(10, 1, 10);
  assert_not_in_range(0, 1, 10);
  assert_not_in_range(11, 1, 10);
}

void test_approximate_equality() {
  double value1 = 1.0;
  double value2 = 1.0001;
  
  assert_approx_equal(value1, value2, 0.001);
  assert_double_equal(value1, value2, 0.001);
}

void test_random_data() {
  char buffer1[64];
  char buffer2[64];
  
  stest_fill_random_bytes(buffer1, 64);
  stest_fill_random_bytes(buffer2, 64);
  
  assert_memory_not_equal(buffer1, buffer2, 64);
}

void test_fixture_advanced() {
  test_fixture_start();
  fixture_setup(fixture_setup_example);
  fixture_teardown(fixture_teardown_example);
  
  run_test(test_with_setup_teardown);
  run_test(test_string_list_operations);
  run_test(test_memory_operations);
  run_test(test_range_assertions);
  run_test(test_approximate_equality);
  run_test(test_random_data);
  
  test_fixture_end();
}

int main(int argc, char** argv) {
  suite_setup(suite_setup_example);
  suite_teardown(suite_teardown_example);
  return stest_testrunner(argc, argv, test_fixture_advanced, 
                         suite_setup_example, suite_teardown_example);
}

