#ifndef STEST_UTILS_H
#define STEST_UTILS_H

#include "stest.h"
#include <stdlib.h>
#include <string.h>

#define STEST_UTILS_MAX_STRING_LEN 1024

typedef struct {
  char** items;
  size_t count;
  size_t capacity;
} stest_string_list_t;

stest_string_list_t* stest_string_list_create(void);
void stest_string_list_add(stest_string_list_t* list, const char* str);
void stest_string_list_free(stest_string_list_t* list);
int stest_string_list_contains(stest_string_list_t* list, const char* str);

void* stest_safe_malloc(size_t size);
void stest_safe_free(void* ptr);
char* stest_string_duplicate(const char* str);
void stest_fill_random_bytes(void* buffer, size_t size);

#define assert_memory_equal(expected, actual, size) \
  do { \
    stest_assert_memory_equal(expected, actual, size, __func__, __LINE__); \
  } while (0)

#define assert_memory_not_equal(expected, actual, size) \
  do { \
    stest_assert_memory_not_equal(expected, actual, size, __func__, __LINE__); \
  } while (0)

void stest_assert_memory_equal(const void* expected, const void* actual, 
                                size_t size, const char* function, unsigned int line);
void stest_assert_memory_not_equal(const void* expected, const void* actual, 
                                    size_t size, const char* function, unsigned int line);

typedef struct {
  int min;
  int max;
  int value;
} stest_range_t;

stest_range_t stest_range_create(int min, int max);
int stest_range_contains(stest_range_t range, int value);
void assert_in_range(int value, int min, int max);
void assert_not_in_range(int value, int min, int max);

#define assert_approx_equal(expected, actual, tolerance) \
  do { \
    stest_assert_approx_equal(expected, actual, tolerance, __func__, __LINE__); \
  } while (0)

void stest_assert_approx_equal(double expected, double actual, double tolerance,
                                const char* function, unsigned int line);

#endif

