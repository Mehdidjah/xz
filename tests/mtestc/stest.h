// SPDX-License-Identifier: 0BSD
// M-testC - Enhanced unit testing framework for C/C++
// Based on STest (which was based on Keith Nicholas's Seatest)

#ifndef STEST_H
#define STEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Test function type
typedef void (*stest_void_void)(void);

// Test runner function
int stest_testrunner(int argc, char **argv, stest_void_void tests,
                     stest_void_void setup, stest_void_void teardown);

// Test execution functions
void run_tests(stest_void_void tests);
void test_fixture_start(void);
void test_fixture_end(void);
void run_test(stest_void_void test);

// Setup and teardown
void suite_setup(stest_void_void setup);
void suite_teardown(stest_void_void teardown);
void fixture_setup(void (*setup)(void));
void fixture_teardown(void (*teardown)(void));

// Filtering
void test_filter(const char *filter);
void fixture_filter(const char *filter);

// Timing functions
double stest_get_test_time(void);
double stest_get_total_time(void);

// Statistics functions
int stest_get_total_asserts(void);

// Output control
void stest_set_json_output(int enable);
void stest_set_progress_enabled(int enable);

// Basic assertions
#define assert_true(test) stest_assert_true(test, __FILE__, __LINE__)
#define assert_false(test) stest_assert_false(test, __FILE__, __LINE__)
#define assert_fail(message) stest_assert_fail(message, __FILE__, __LINE__)

// Integer assertions
#define assert_int_equal(expected, actual) \
    stest_assert_int_equal(expected, actual, __FILE__, __LINE__)
#define assert_ulong_equal(expected, actual) \
    stest_assert_ulong_equal(expected, actual, __FILE__, __LINE__)

// Floating point assertions
#define assert_float_equal(expected, actual, delta) \
    stest_assert_float_equal(expected, actual, delta, __FILE__, __LINE__)
#define assert_double_equal(expected, actual, delta) \
    stest_assert_double_equal(expected, actual, delta, __FILE__, __LINE__)

// String assertions
#define assert_string_equal(expected, actual) \
    stest_assert_string_equal(expected, actual, __FILE__, __LINE__)
#define assert_string_contains(contained, container) \
    stest_assert_string_contains(contained, container, __FILE__, __LINE__)
#define assert_string_not_contains(contained, container) \
    stest_assert_string_not_contains(contained, container, __FILE__, __LINE__)
#define assert_string_starts_with(prefix, string) \
    stest_assert_string_starts_with(prefix, string, __FILE__, __LINE__)
#define assert_string_ends_with(suffix, string) \
    stest_assert_string_ends_with(suffix, string, __FILE__, __LINE__)

// Pointer assertions
#define assert_null(ptr) stest_assert_null(ptr, __FILE__, __LINE__)
#define assert_not_null(ptr) stest_assert_not_null(ptr, __FILE__, __LINE__)
#define assert_ptr_equal(expected, actual) \
    stest_assert_ptr_equal(expected, actual, __FILE__, __LINE__)
#define assert_ptr_not_equal(expected, actual) \
    stest_assert_ptr_not_equal(expected, actual, __FILE__, __LINE__)

// Array and bit assertions
#define assert_n_array_equal(expected, actual, n) \
    stest_assert_n_array_equal(expected, actual, n, __FILE__, __LINE__)
#define assert_bit_set(bit_number, value) \
    stest_assert_bit_set(bit_number, value, __FILE__, __LINE__)
#define assert_bit_not_set(bit_number, value) \
    stest_assert_bit_not_set(bit_number, value, __FILE__, __LINE__)
#define assert_bit_mask_matches(value, mask) \
    stest_assert_bit_mask_matches(value, mask, __FILE__, __LINE__)

// Internal assertion functions
void stest_assert_true(int test, const char *file, int line);
void stest_assert_false(int test, const char *file, int line);
void stest_assert_fail(const char *message, const char *file, int line);
void stest_assert_int_equal(int expected, int actual, const char *file, int line);
void stest_assert_ulong_equal(unsigned long expected, unsigned long actual,
                               const char *file, int line);
void stest_assert_float_equal(float expected, float actual, float delta,
                              const char *file, int line);
void stest_assert_double_equal(double expected, double actual, double delta,
                               const char *file, int line);
void stest_assert_string_equal(const char *expected, const char *actual,
                               const char *file, int line);
void stest_assert_string_contains(const char *contained, const char *container,
                                  const char *file, int line);
void stest_assert_string_not_contains(const char *contained, const char *container,
                                      const char *file, int line);
void stest_assert_string_starts_with(const char *prefix, const char *string,
                                     const char *file, int line);
void stest_assert_string_ends_with(const char *suffix, const char *string,
                                   const char *file, int line);
void stest_assert_null(void *ptr, const char *file, int line);
void stest_assert_not_null(void *ptr, const char *file, int line);
void stest_assert_ptr_equal(void *expected, void *actual,
                            const char *file, int line);
void stest_assert_ptr_not_equal(void *expected, void *actual,
                                const char *file, int line);
void stest_assert_n_array_equal(void *expected, void *actual, int n,
                                const char *file, int line);
void stest_assert_bit_set(int bit_number, int value, const char *file, int line);
void stest_assert_bit_not_set(int bit_number, int value, const char *file, int line);
void stest_assert_bit_mask_matches(size_t value, size_t mask,
                                   const char *file, int line);

#ifdef __cplusplus
}
#endif

#endif // STEST_H

