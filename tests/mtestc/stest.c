// SPDX-License-Identifier: 0BSD
// M-testC - Enhanced unit testing framework for C/C++
// Based on STest (which was based on Keith Nicholas's Seatest)

#include "stest.h"
#include <stdarg.h>
#include <math.h>

// Test state
static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;
static int assertions_run = 0;
static int assertions_passed = 0;
static int assertions_failed = 0;

// Timing
static clock_t test_start_time = 0;
static clock_t suite_start_time = 0;
static double test_time = 0.0;
static double total_time = 0.0;

// Test context
static char current_test_name[256] = "";
static char current_fixture_name[256] = "";
static bool in_fixture = false;
static bool test_passed = true;

// Filters
static char *test_filter_str = NULL;
static char *fixture_filter_str = NULL;

// Output options
static bool json_output = false;
static bool progress_enabled = false;
static bool color_output = false;
static bool verbose_output = false;

// Setup/teardown functions
static stest_void_void suite_setup_func = NULL;
static stest_void_void suite_teardown_func = NULL;
static void (*fixture_setup_func)(void) = NULL;
static void (*fixture_teardown_func)(void) = NULL;

// ANSI color codes
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"

static void print_color(const char *color) {
    if (color_output) {
        printf("%s", color);
    }
}

static void print_reset(void) {
    if (color_output) {
        printf("%s", ANSI_COLOR_RESET);
    }
}

static bool matches_filter(const char *name, const char *filter) {
    if (filter == NULL || filter[0] == '\0') return true;
    return strstr(name, filter) != NULL;
}

static void start_test(const char *test_name) {
    strncpy(current_test_name, test_name, sizeof(current_test_name) - 1);
    current_test_name[sizeof(current_test_name) - 1] = '\0';
    test_passed = true;
    test_start_time = clock();
    assertions_run = 0;
    
    if (progress_enabled) {
        printf("Running: %s", test_name);
        fflush(stdout);
    } else if (verbose_output) {
        printf("  Test: %s\n", test_name);
    }
}

static void end_test(void) {
    test_time = ((double)(clock() - test_start_time)) / CLOCKS_PER_SEC;
    total_time += test_time;
    
    if (test_passed) {
        tests_passed++;
        if (progress_enabled) {
            printf(" [PASS] (%.3fs)\n", test_time);
        } else if (!verbose_output) {
            print_color(ANSI_COLOR_GREEN);
            printf(".");
            print_reset();
            fflush(stdout);
        }
    } else {
        tests_failed++;
        if (!progress_enabled && !verbose_output) {
            print_color(ANSI_COLOR_RED);
            printf("F");
            print_reset();
            fflush(stdout);
        }
    }
    
    tests_run++;
    
    if (fixture_teardown_func) {
        fixture_teardown_func();
    }
}

static void fail_test(const char *file, int line, const char *message) {
    test_passed = false;
    assertions_failed++;
    
    if (!progress_enabled) {
        printf("\n");
    }
    
    print_color(ANSI_COLOR_RED);
    printf("FAIL: %s:%d: %s\n", file, line, message);
    print_reset();
    
    if (verbose_output) {
        printf("  In test: %s\n", current_test_name);
        if (in_fixture) {
            printf("  In fixture: %s\n", current_fixture_name);
        }
    }
}

static void pass_assertion(void) {
    assertions_passed++;
    assertions_run++;
}

void stest_assert_true(int test, const char *file, int line) {
    if (!test) {
        fail_test(file, line, "assert_true failed");
    } else {
        pass_assertion();
    }
}

void stest_assert_false(int test, const char *file, int line) {
    if (test) {
        fail_test(file, line, "assert_false failed");
    } else {
        pass_assertion();
    }
}

void stest_assert_fail(const char *message, const char *file, int line) {
    fail_test(file, line, message);
}

void stest_assert_int_equal(int expected, int actual, const char *file, int line) {
    if (expected != actual) {
        char msg[256];
        snprintf(msg, sizeof(msg), "assert_int_equal failed: expected %d, got %d", expected, actual);
        fail_test(file, line, msg);
    } else {
        pass_assertion();
    }
}

void stest_assert_ulong_equal(unsigned long expected, unsigned long actual,
                              const char *file, int line) {
    if (expected != actual) {
        char msg[256];
        snprintf(msg, sizeof(msg), "assert_ulong_equal failed: expected %lu, got %lu", expected, actual);
        fail_test(file, line, msg);
    } else {
        pass_assertion();
    }
}

void stest_assert_float_equal(float expected, float actual, float delta,
                              const char *file, int line) {
    if (fabsf(expected - actual) > delta) {
        char msg[256];
        snprintf(msg, sizeof(msg), "assert_float_equal failed: expected %f, got %f (delta %f)", 
                expected, actual, delta);
        fail_test(file, line, msg);
    } else {
        pass_assertion();
    }
}

void stest_assert_double_equal(double expected, double actual, double delta,
                               const char *file, int line) {
    if (fabs(expected - actual) > delta) {
        char msg[256];
        snprintf(msg, sizeof(msg), "assert_double_equal failed: expected %f, got %f (delta %f)", 
                expected, actual, delta);
        fail_test(file, line, msg);
    } else {
        pass_assertion();
    }
}

void stest_assert_string_equal(const char *expected, const char *actual,
                              const char *file, int line) {
    if (expected == NULL && actual == NULL) {
        pass_assertion();
        return;
    }
    if (expected == NULL || actual == NULL || strcmp(expected, actual) != 0) {
        char msg[512];
        snprintf(msg, sizeof(msg), "assert_string_equal failed: expected \"%s\", got \"%s\"", 
                expected ? expected : "(null)", actual ? actual : "(null)");
        fail_test(file, line, msg);
    } else {
        pass_assertion();
    }
}

void stest_assert_string_contains(const char *contained, const char *container,
                                 const char *file, int line) {
    if (contained == NULL || container == NULL || strstr(container, contained) == NULL) {
        char msg[512];
        snprintf(msg, sizeof(msg), "assert_string_contains failed: \"%s\" not found in \"%s\"", 
                contained ? contained : "(null)", container ? container : "(null)");
        fail_test(file, line, msg);
    } else {
        pass_assertion();
    }
}

void stest_assert_string_not_contains(const char *contained, const char *container,
                                     const char *file, int line) {
    if (contained != NULL && container != NULL && strstr(container, contained) != NULL) {
        char msg[512];
        snprintf(msg, sizeof(msg), "assert_string_not_contains failed: \"%s\" found in \"%s\"", 
                contained, container);
        fail_test(file, line, msg);
    } else {
        pass_assertion();
    }
}

void stest_assert_string_starts_with(const char *prefix, const char *string,
                                    const char *file, int line) {
    if (prefix == NULL || string == NULL || strncmp(string, prefix, strlen(prefix)) != 0) {
        char msg[512];
        snprintf(msg, sizeof(msg), "assert_string_starts_with failed: \"%s\" does not start with \"%s\"", 
                string ? string : "(null)", prefix ? prefix : "(null)");
        fail_test(file, line, msg);
    } else {
        pass_assertion();
    }
}

void stest_assert_string_ends_with(const char *suffix, const char *string,
                                   const char *file, int line) {
    if (suffix == NULL || string == NULL) {
        char msg[512];
        snprintf(msg, sizeof(msg), "assert_string_ends_with failed: null pointer");
        fail_test(file, line, msg);
        return;
    }
    size_t suffix_len = strlen(suffix);
    size_t string_len = strlen(string);
    if (string_len < suffix_len || strcmp(string + string_len - suffix_len, suffix) != 0) {
        char msg[512];
        snprintf(msg, sizeof(msg), "assert_string_ends_with failed: \"%s\" does not end with \"%s\"", 
                string, suffix);
        fail_test(file, line, msg);
    } else {
        pass_assertion();
    }
}

void stest_assert_null(void *ptr, const char *file, int line) {
    if (ptr != NULL) {
        char msg[256];
        snprintf(msg, sizeof(msg), "assert_null failed: pointer is not NULL");
        fail_test(file, line, msg);
    } else {
        pass_assertion();
    }
}

void stest_assert_not_null(void *ptr, const char *file, int line) {
    if (ptr == NULL) {
        char msg[256];
        snprintf(msg, sizeof(msg), "assert_not_null failed: pointer is NULL");
        fail_test(file, line, msg);
    } else {
        pass_assertion();
    }
}

void stest_assert_ptr_equal(void *expected, void *actual, const char *file, int line) {
    if (expected != actual) {
        char msg[256];
        snprintf(msg, sizeof(msg), "assert_ptr_equal failed: pointers differ");
        fail_test(file, line, msg);
    } else {
        pass_assertion();
    }
}

void stest_assert_ptr_not_equal(void *expected, void *actual, const char *file, int line) {
    if (expected == actual) {
        char msg[256];
        snprintf(msg, sizeof(msg), "assert_ptr_not_equal failed: pointers are equal");
        fail_test(file, line, msg);
    } else {
        pass_assertion();
    }
}

void stest_assert_n_array_equal(void *expected, void *actual, int n,
                                const char *file, int line) {
    if (expected == NULL || actual == NULL) {
        char msg[256];
        snprintf(msg, sizeof(msg), "assert_n_array_equal failed: null pointer");
        fail_test(file, line, msg);
        return;
    }
    if (memcmp(expected, actual, n) != 0) {
        char msg[256];
        snprintf(msg, sizeof(msg), "assert_n_array_equal failed: arrays differ");
        fail_test(file, line, msg);
    } else {
        pass_assertion();
    }
}

void stest_assert_bit_set(int bit_number, int value, const char *file, int line) {
    if ((value & (1U << bit_number)) == 0) {
        char msg[256];
        snprintf(msg, sizeof(msg), "assert_bit_set failed: bit %d is not set in 0x%x", bit_number, value);
        fail_test(file, line, msg);
    } else {
        pass_assertion();
    }
}

void stest_assert_bit_not_set(int bit_number, int value, const char *file, int line) {
    if ((value & (1U << bit_number)) != 0) {
        char msg[256];
        snprintf(msg, sizeof(msg), "assert_bit_not_set failed: bit %d is set in 0x%x", bit_number, value);
        fail_test(file, line, msg);
    } else {
        pass_assertion();
    }
}

void stest_assert_bit_mask_matches(size_t value, size_t mask,
                                   const char *file, int line) {
    if ((value & mask) != mask) {
        char msg[256];
        snprintf(msg, sizeof(msg), "assert_bit_mask_matches failed: mask 0x%zx does not match value 0x%zx", mask, value);
        fail_test(file, line, msg);
    } else {
        pass_assertion();
    }
}

void test_fixture_start(void) {
    in_fixture = true;
    if (fixture_setup_func) {
        fixture_setup_func();
    }
}

void test_fixture_end(void) {
    in_fixture = false;
    current_fixture_name[0] = '\0';
}

void run_test(stest_void_void test) {
    if (test == NULL) return;
    
    const char *test_name = "unknown";
    // Try to get function name (platform dependent, simplified here)
    char test_name_buf[256];
    snprintf(test_name_buf, sizeof(test_name_buf), "test_%p", (void*)test);
    test_name = test_name_buf;
    
    if (test_filter_str != NULL && !matches_filter(test_name, test_filter_str)) {
        return;
    }
    
    start_test(test_name);
    test();
    end_test();
}

void suite_setup(stest_void_void setup) {
    suite_setup_func = setup;
}

void suite_teardown(stest_void_void teardown) {
    suite_teardown_func = teardown;
}

void fixture_setup(void (*setup)(void)) {
    fixture_setup_func = setup;
}

void fixture_teardown(void (*teardown)(void)) {
    fixture_teardown_func = teardown;
}

void test_filter(const char *filter) {
    if (test_filter_str) {
        free(test_filter_str);
    }
    test_filter_str = filter ? strdup(filter) : NULL;
}

void fixture_filter(const char *filter) {
    if (fixture_filter_str) {
        free(fixture_filter_str);
    }
    fixture_filter_str = filter ? strdup(filter) : NULL;
}

double stest_get_test_time(void) {
    return test_time;
}

double stest_get_total_time(void) {
    return total_time;
}

int stest_get_total_asserts(void) {
    return assertions_run;
}

void stest_set_json_output(int enable) {
    json_output = enable != 0;
}

void stest_set_progress_enabled(int enable) {
    progress_enabled = enable != 0;
}

void run_tests(stest_void_void tests) {
    if (tests == NULL) return;
    
    suite_start_time = clock();
    total_time = 0.0;
    tests_run = 0;
    tests_passed = 0;
    tests_failed = 0;
    assertions_run = 0;
    assertions_passed = 0;
    assertions_failed = 0;
    
    if (suite_setup_func) {
        suite_setup_func();
    }
    
    tests();
    
    total_time = ((double)(clock() - suite_start_time)) / CLOCKS_PER_SEC;
    
    if (suite_teardown_func) {
        suite_teardown_func();
    }
}

static void print_summary(void) {
    if (json_output) {
        printf("{\n");
        printf("  \"summary\": {\n");
        printf("    \"total\": %d,\n", tests_run);
        printf("    \"passed\": %d,\n", tests_passed);
        printf("    \"failed\": %d,\n", tests_failed);
        printf("    \"asserts\": %d,\n", assertions_run);
        printf("    \"time\": %.3f\n", total_time);
        printf("  }\n");
        printf("}\n");
    } else {
        printf("\n");
        printf("========================================\n");
        printf("Test Results Summary\n");
        printf("========================================\n");
        printf("Tests run:     %d\n", tests_run);
        print_color(tests_passed > 0 ? ANSI_COLOR_GREEN : ANSI_COLOR_RESET);
        printf("Tests passed:  %d\n", tests_passed);
        print_reset();
        print_color(tests_failed > 0 ? ANSI_COLOR_RED : ANSI_COLOR_RESET);
        printf("Tests failed:  %d\n", tests_failed);
        print_reset();
        printf("Assertions:    %d\n", assertions_run);
        printf("Total time:    %.3fs\n", total_time);
        printf("========================================\n");
    }
}

int stest_testrunner(int argc, char **argv, stest_void_void tests,
                     stest_void_void setup, stest_void_void teardown) {
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose_output = true;
        } else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--color") == 0) {
            color_output = true;
        } else if (strcmp(argv[i], "-j") == 0 || strcmp(argv[i], "--json") == 0) {
            json_output = true;
        } else if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--progress") == 0) {
            progress_enabled = true;
        } else if (strcmp(argv[i], "-t") == 0 && i + 1 < argc) {
            test_filter(argv[++i]);
        } else if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            fixture_filter(argv[++i]);
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printf("M-testC Test Runner\n");
            printf("Usage: %s [options]\n", argv[0]);
            printf("Options:\n");
            printf("  -v, --verbose     Verbose output\n");
            printf("  -c, --color       Color-coded output\n");
            printf("  -j, --json        JSON output format\n");
            printf("  -p, --progress    Show progress indicator\n");
            printf("  -t <name>         Run only tests matching <name>\n");
            printf("  -f <name>         Run only fixtures matching <name>\n");
            printf("  -h, --help        Show this help message\n");
            return 0;
        }
    }
    
    if (setup) {
        suite_setup(setup);
    }
    if (teardown) {
        suite_teardown(teardown);
    }
    
    run_tests(tests);
    print_summary();
    
    return tests_failed > 0 ? 1 : 0;
}

