# MehdiTest

## About
**MehdiTest** is an enhanced unit testing framework for C/C++ based on STest (which was based on Keith Nicholas's Seatest). **MehdiTest** is an xUnit-style unit testing framework designed to be portable, powerful, and easy to use. Installing **MehdiTest** is as simple as dropping the **stest.c** and **stest.h** files into your project.

**Owner:** Mehdi

## Features
- xUnit style asserts with comprehensive assertion library
- Easily extensible for custom asserts
- Optional color-coded output
- Cross-platform support for any C/C++ project
- Supports global set-up and tear-down functions
- Supports per-test set-up and tear-down functions
- Ability to selectively run tests and fixtures
- **NEW:** Test timing and performance metrics
- **NEW:** JSON output format for integration with CI/CD tools
- **NEW:** Progress indicators during test execution
- **NEW:** Enhanced pointer assertions (null checks, pointer equality)
- **NEW:** Assertion counting and detailed statistics
- **NEW:** Improved error messages and reporting

## Installation

Simply copy `src/stest.c` and `src/stest.h` into your project directory and include the header file in your test files.

### Using CMake

```cmake
cmake_minimum_required(VERSION 3.10)
project(myproject VERSION 1.0.0 LANGUAGES C)

include_directories(src)
add_executable(tests test_file.c src/stest.c)
```

## Asserts

### Basic Assertions
| Assert | Arguments | Meaning |
|--------|-----------|---------|
| `assert_true` | int test | Asserts test is non-zero |
| `assert_false` | int test | Asserts test is zero |
| `assert_fail` | char* message | Automatic failing test with a custom message |

### Integer Assertions
| Assert | Arguments | Meaning |
|--------|-----------|---------|
| `assert_int_equal` | int expected, int actual | Asserts expected == actual |
| `assert_ulong_equal` | unsigned long expected, unsigned long actual | Asserts expected == actual |

### Floating Point Assertions
| Assert | Arguments | Meaning |
|--------|-----------|---------|
| `assert_float_equal` | float expected, float actual, float delta | Asserts expected is within delta of actual |
| `assert_double_equal` | double expected, double actual, double delta | Asserts expected is within delta of actual |

### String Assertions
| Assert | Arguments | Meaning |
|--------|-----------|---------|
| `assert_string_equal` | char* expected, char* actual | Asserts all characters of expected equal all characters of actual |
| `assert_string_contains` | char* contained, char* container | Asserts contained is a substring of container |
| `assert_string_not_contains` | char* contained, char* container | Asserts contained is not a substring of container |
| `assert_string_starts_with` | char* prefix, char* string | Asserts string begins with prefix |
| `assert_string_ends_with` | char* suffix, char* string | Asserts string ends with suffix |

### Pointer Assertions (NEW)
| Assert | Arguments | Meaning |
|--------|-----------|---------|
| `assert_null` | void* ptr | Asserts pointer is NULL |
| `assert_not_null` | void* ptr | Asserts pointer is not NULL |
| `assert_ptr_equal` | void* expected, void* actual | Asserts both pointers point to the same memory location |
| `assert_ptr_not_equal` | void* expected, void* actual | Asserts pointers point to different memory locations |

### Array and Bit Assertions
| Assert | Arguments | Meaning |
|--------|-----------|---------|
| `assert_n_array_equal` | void* expected, void* actual, int n | Asserts first n elements from expected equal actual |
| `assert_bit_set` | int bit_number, int value | Asserts the bit_number in value is set to 1 |
| `assert_bit_not_set` | int bit_number, int value | Asserts the bit_number in value is set to 0 |
| `assert_bit_mask_matches` | size value, size mask | Asserts all 1 bits in mask are set to 1 in value |

## Command Line Arguments

The test runner supports various command line options for flexible test execution:

| Option | Meaning |
|--------|---------|
| `-d` | Display tests, do not run tests |
| `-v` | Run tests in verbose mode |
| `-vs` | Alternative display mode |
| `-t <testname>` | Only run tests that match `<testname>` |
| `-f <fixturename>` | Only run fixtures that match `<fixturename>` |
| `-m` | Output machine readable format |
| `-k <marker>` | Prepend `<marker>` before machine readable output |
| `-c` | Color code output (green success, red failure) |
| `-j` | **NEW:** Output test results in JSON format |
| `-p` | **NEW:** Show progress indicator during test run |
| `-h`, `--help` | Output help message |

## New Features

### Test Timing
MehdiTest automatically tracks execution time for each test and the entire test suite. Timing information is displayed in the summary:

```
Total time: 0.123s | Assertions: 45
```

You can also programmatically access timing information:
```c
double test_time = stest_get_test_time();    // Time for current test
double total_time = stest_get_total_time();  // Time for entire suite
```

### JSON Output
Enable JSON output for easy integration with CI/CD pipelines and test reporting tools:

```bash
./tests -j
```

Output format:
```json
{
  "summary": {
    "total": 10,
    "passed": 9,
    "failed": 1,
    "asserts": 45,
    "time": 0.123
  }
}
```

### Progress Indicators
Show real-time progress during test execution:

```bash
./tests -p
```

Displays: `Running: test_name (0.045s) [Tests: 5, Passed: 4, Failed: 1]`

### Assertion Counting
MehdiTest automatically counts all assertions executed during the test run. Access the count programmatically:

```c
int total_asserts = stest_get_total_asserts();
```

### Enhanced Pointer Assertions
New pointer assertion macros make testing pointer-based code easier and safer:

```c
void *ptr = malloc(100);
assert_not_null(ptr);

void *ptr2 = ptr;
assert_ptr_equal(ptr, ptr2);

free(ptr);
assert_null(ptr);
```

## Example Usage

### Basic Test Example

```c
#include "stest.h"

// Sample test
void test_addition() {
  int result = 2 + 2;
  assert_int_equal(4, result);
}

void test_string_operations() {
  char *str = "Hello World";
  assert_string_contains("Hello", str);
  assert_string_starts_with("Hello", str);
  assert_string_ends_with("World", str);
}

void test_pointers() {
  void *ptr = malloc(100);
  assert_not_null(ptr);
  
  void *ptr2 = ptr;
  assert_ptr_equal(ptr, ptr2);
  
  free(ptr);
}

// Sample fixture
void test_fixture_math() {
  test_fixture_start();
  run_test(test_addition);
  test_fixture_end();
}

void test_fixture_strings() {
  test_fixture_start();
  run_test(test_string_operations);
  test_fixture_end();
}

void test_fixture_memory() {
  test_fixture_start();
  run_test(test_pointers);
  test_fixture_end();
}

// Sample test suite
void run_all_tests() {
  test_fixture_math();
  test_fixture_strings();
  test_fixture_memory();
}

int main(int argc, char **argv) {
  return stest_testrunner(argc, argv, run_all_tests, NULL, NULL);
}
```

### Setup and Teardown Example

```c
static int test_value = 0;

void suite_setup_func(void) {
  // Called once before all tests
  test_value = 100;
}

void suite_teardown_func(void) {
  // Called once after all tests
  test_value = 0;
}

void fixture_setup_func(void) {
  // Called before each test in the fixture
  test_value += 10;
}

void fixture_teardown_func(void) {
  // Called after each test in the fixture
  test_value -= 10;
}

void test_fixture_example() {
  test_fixture_start();
  fixture_setup(fixture_setup_func);
  fixture_teardown(fixture_teardown_func);
  
  run_test(some_test);
  
  test_fixture_end();
}

int main(int argc, char **argv) {
  suite_setup(suite_setup_func);
  suite_teardown(suite_teardown_func);
  return stest_testrunner(argc, argv, run_all_tests, suite_setup_func, suite_teardown_func);
}
```

## Building and Running Tests

### Using CMake

```bash
mkdir build
cd build
cmake ..
make
./stests
```

### Running with Options

```bash
# Run all tests with color output
./stests -c

# Run only tests matching "math"
./stests -t math

# Run with progress indicator
./stests -p

# Run with JSON output
./stests -j

# Run in verbose mode
./stests -v

# Combine options
./stests -c -p -v
```

## API Reference

### Test Management Functions

- `stest_testrunner(int argc, char** argv, stest_void_void tests, stest_void_void setup, stest_void_void teardown)` - Main test runner function
- `run_tests(stest_void_void tests)` - Run a test suite
- `test_fixture_start()` - Start a test fixture
- `test_fixture_end()` - End a test fixture
- `run_test(test)` - Run a single test function
- `suite_setup(stest_void_void setup)` - Set global setup function
- `suite_teardown(stest_void_void teardown)` - Set global teardown function
- `fixture_setup(void (*setup)(void))` - Set fixture setup function
- `fixture_teardown(void (*teardown)(void))` - Set fixture teardown function

### Filtering Functions

- `test_filter(const char* filter)` - Filter tests by name
- `fixture_filter(const char* filter)` - Filter fixtures by name

### Timing Functions (NEW)

- `double stest_get_test_time(void)` - Get execution time of current test in seconds
- `double stest_get_total_time(void)` - Get total execution time of test suite in seconds

### Statistics Functions (NEW)

- `int stest_get_total_asserts(void)` - Get total number of assertions executed

### Output Control Functions (NEW)

- `void stest_set_json_output(int enable)` - Enable/disable JSON output
- `void stest_set_progress_enabled(int enable)` - Enable/disable progress indicators

## Contributing

Contributions are welcome! Please feel free to submit pull requests for bug fixes and new features.

## License

See license.txt for details.

## Credits

- Original Seatest framework by Keith Nicholas
- STest by Jia Tan
- Enhanced and improved by Mehdi

## Version History

### Version 2.0.0 (Current)
- Added test timing and performance metrics
- Added JSON output format
- Added progress indicators
- Added pointer assertion macros (null, not_null, ptr_equal, ptr_not_equal)
- Added assertion counting
- Improved error messages and statistics
- Enhanced documentation

### Version 1.0.0
- Initial release based on STest
