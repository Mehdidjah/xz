#include "../src/stest.h"
#include "../src/stest_utils.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define PERFORMANCE_TEST_SIZE 10000

void test_array_operations_performance() {
  int* array = (int*)malloc(PERFORMANCE_TEST_SIZE * sizeof(int));
  assert_not_null(array);
  
  clock_t start = clock();
  
  for (int i = 0; i < PERFORMANCE_TEST_SIZE; i++) {
    array[i] = i * 2;
  }
  
  clock_t end = clock();
  double elapsed = ((double)(end - start)) / CLOCKS_PER_SEC;
  
  assert_int_equal(0, array[0]);
  assert_int_equal(19998, array[PERFORMANCE_TEST_SIZE - 1]);
  
  printf("  Array operations took: %.6f seconds\n", elapsed);
  
  free(array);
}

void test_string_operations_performance() {
  char* str = (char*)malloc(PERFORMANCE_TEST_SIZE + 1);
  assert_not_null(str);
  
  clock_t start = clock();
  
  for (int i = 0; i < PERFORMANCE_TEST_SIZE; i++) {
    str[i] = 'A' + (i % 26);
  }
  str[PERFORMANCE_TEST_SIZE] = '\0';
  
  clock_t end = clock();
  double elapsed = ((double)(end - start)) / CLOCKS_PER_SEC;
  
  assert_int_equal('A', str[0]);
  assert_int_equal((int)strlen(str), PERFORMANCE_TEST_SIZE);
  
  printf("  String operations took: %.6f seconds\n", elapsed);
  
  free(str);
}

void test_memory_operations_performance() {
  void* buffer1 = malloc(PERFORMANCE_TEST_SIZE);
  void* buffer2 = malloc(PERFORMANCE_TEST_SIZE);
  assert_not_null(buffer1);
  assert_not_null(buffer2);
  
  memset(buffer1, 0xAA, PERFORMANCE_TEST_SIZE);
  
  clock_t start = clock();
  memcpy(buffer2, buffer1, PERFORMANCE_TEST_SIZE);
  clock_t end = clock();
  double elapsed = ((double)(end - start)) / CLOCKS_PER_SEC;
  
  assert_memory_equal(buffer1, buffer2, PERFORMANCE_TEST_SIZE);
  
  printf("  Memory operations took: %.6f seconds\n", elapsed);
  
  free(buffer1);
  free(buffer2);
}

void test_fixture_performance() {
  test_fixture_start();
  run_test(test_array_operations_performance);
  run_test(test_string_operations_performance);
  run_test(test_memory_operations_performance);
  test_fixture_end();
}

int main(int argc, char** argv) {
  printf("Running performance tests...\n");
  printf("Test size: %d elements\n\n", PERFORMANCE_TEST_SIZE);
  
  return stest_testrunner(argc, argv, test_fixture_performance, NULL, NULL);
}

