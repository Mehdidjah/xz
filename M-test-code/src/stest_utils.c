#include "stest_utils.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

extern void (*stest_simple_test_result)(int passed, const char *reason,
                                        const char *function, unsigned int line);

stest_string_list_t* stest_string_list_create(void) {
  stest_string_list_t* list = (stest_string_list_t*)malloc(sizeof(stest_string_list_t));
  if (!list) return NULL;
  
  list->capacity = 16;
  list->count = 0;
  list->items = (char**)malloc(list->capacity * sizeof(char*));
  if (!list->items) {
    free(list);
    return NULL;
  }
  
  return list;
}

void stest_string_list_add(stest_string_list_t* list, const char* str) {
  if (!list || !str) return;
  
  if (list->count >= list->capacity) {
    list->capacity *= 2;
    list->items = (char**)realloc(list->items, list->capacity * sizeof(char*));
    if (!list->items) return;
  }
  
  list->items[list->count] = stest_string_duplicate(str);
  list->count++;
}

void stest_string_list_free(stest_string_list_t* list) {
  if (!list) return;
  
  for (size_t i = 0; i < list->count; i++) {
    free(list->items[i]);
  }
  free(list->items);
  free(list);
}

int stest_string_list_contains(stest_string_list_t* list, const char* str) {
  if (!list || !str) return 0;
  
  for (size_t i = 0; i < list->count; i++) {
    if (strcmp(list->items[i], str) == 0) {
      return 1;
    }
  }
  return 0;
}

void* stest_safe_malloc(size_t size) {
  void* ptr = malloc(size);
  assert_not_null(ptr);
  return ptr;
}

void stest_safe_free(void* ptr) {
  if (ptr) {
    free(ptr);
  }
}

char* stest_string_duplicate(const char* str) {
  if (!str) return NULL;
  size_t len = strlen(str) + 1;
  char* dup = (char*)malloc(len);
  if (dup) {
    memcpy(dup, str, len);
  }
  return dup;
}

void stest_fill_random_bytes(void* buffer, size_t size) {
  if (!buffer || size == 0) return;
  
  static int seeded = 0;
  if (!seeded) {
    srand((unsigned int)time(NULL));
    seeded = 1;
  }
  
  unsigned char* buf = (unsigned char*)buffer;
  for (size_t i = 0; i < size; i++) {
    buf[i] = (unsigned char)(rand() % 256);
  }
}

void stest_assert_memory_equal(const void* expected, const void* actual, 
                                size_t size, const char* function, unsigned int line) {
  char s[STEST_PRINT_BUFFER_SIZE];
  
  if (expected == NULL && actual == NULL) {
    stest_simple_test_result(1, "Both pointers are NULL", function, line);
    return;
  }
  
  if (expected == NULL || actual == NULL) {
    sprintf(s, "One pointer is NULL (expected: %p, actual: %p)", expected, actual);
    stest_simple_test_result(0, s, function, line);
    return;
  }
  
  int equal = memcmp(expected, actual, size) == 0;
  if (!equal) {
    sprintf(s, "Memory regions differ (size: %zu)", size);
  } else {
    sprintf(s, "Memory regions are equal (size: %zu)", size);
  }
  stest_simple_test_result(equal, s, function, line);
}

void stest_assert_memory_not_equal(const void* expected, const void* actual, 
                                    size_t size, const char* function, unsigned int line) {
  char s[STEST_PRINT_BUFFER_SIZE];
  
  if (expected == NULL || actual == NULL) {
    sprintf(s, "One pointer is NULL (expected: %p, actual: %p)", expected, actual);
    stest_simple_test_result(1, s, function, line);
    return;
  }
  
  int not_equal = memcmp(expected, actual, size) != 0;
  if (not_equal) {
    sprintf(s, "Memory regions differ (size: %zu)", size);
  } else {
    sprintf(s, "Memory regions are equal but should differ (size: %zu)", size);
  }
  stest_simple_test_result(not_equal, s, function, line);
}

stest_range_t stest_range_create(int min, int max) {
  stest_range_t range;
  range.min = min < max ? min : max;
  range.max = min < max ? max : min;
  range.value = 0;
  return range;
}

int stest_range_contains(stest_range_t range, int value) {
  return value >= range.min && value <= range.max;
}

void assert_in_range(int value, int min, int max) {
  char s[STEST_PRINT_BUFFER_SIZE];
  int in_range = value >= min && value <= max;
  sprintf(s, "Value %d is %sin range [%d, %d]", value, in_range ? "" : "not ", min, max);
  stest_simple_test_result(in_range, s, __func__, __LINE__);
}

void assert_not_in_range(int value, int min, int max) {
  char s[STEST_PRINT_BUFFER_SIZE];
  int not_in_range = value < min || value > max;
  sprintf(s, "Value %d is %sin range [%d, %d]", value, not_in_range ? "not " : "", min, max);
  stest_simple_test_result(not_in_range, s, __func__, __LINE__);
}

void stest_assert_approx_equal(double expected, double actual, double tolerance,
                                const char* function, unsigned int line) {
  char s[STEST_PRINT_BUFFER_SIZE];
  double diff = expected > actual ? expected - actual : actual - expected;
  int approx_equal = diff <= tolerance;
  
  if (approx_equal) {
    sprintf(s, "Values are approximately equal (expected: %.6f, actual: %.6f, diff: %.6f, tolerance: %.6f)",
            expected, actual, diff, tolerance);
  } else {
    sprintf(s, "Values are not approximately equal (expected: %.6f, actual: %.6f, diff: %.6f, tolerance: %.6f)",
            expected, actual, diff, tolerance);
  }
  stest_simple_test_result(approx_equal, s, function, line);
}

