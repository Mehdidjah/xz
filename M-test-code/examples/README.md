# MehdiTest Examples

This directory contains example programs demonstrating how to use MehdiTest.

## Quick Start Example

`quick_start.c` - A simple introduction to MehdiTest showing basic assertions and test structure.

To compile and run:
```bash
gcc -I../src -o quick_start examples/quick_start.c ../src/stest.c
./quick_start
```

## Advanced Example

`advanced_example.c` - Demonstrates advanced features including:
- Setup and teardown functions
- String list utilities
- Memory comparison assertions
- Range assertions
- Approximate equality tests
- Random data generation

To compile and run:
```bash
gcc -I../src -o advanced examples/advanced_example.c ../src/stest.c ../src/stest_utils.c
./advanced
```

## Performance Test

`performance_test.c` - Shows how to measure and report performance metrics in your tests.

To compile and run:
```bash
gcc -I../src -o performance examples/performance_test.c ../src/stest.c ../src/stest_utils.c
./performance
```

## Utilities Library

The `stest_utils.h` and `stest_utils.c` files provide additional utility functions for common testing patterns:

- String list management
- Safe memory allocation
- Memory comparison assertions
- Range checking assertions
- Approximate equality for floating point numbers
- Random data generation

