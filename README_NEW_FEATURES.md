# XZ Utils Enhanced Edition - New Features Guide

## Overview

This enhanced version of XZ Utils (version 5.9.0) includes several new features designed to improve usability, performance monitoring, and file handling capabilities.

## Quick Start

### Building and Testing

1. **Build M-testC framework**:
```bash
cd tests/mtestc
make
```

2. **Build and run tests**:
```bash
cd tests
make -f Makefile.mtestc
./test_new_features -v -c -p
```

## New Features

### 1. M-testC Testing Framework

A comprehensive unit testing framework integrated into the project.

**Location**: `tests/mtestc/`

**Features**:
- xUnit-style test framework
- Comprehensive assertion library (int, float, string, pointer, bit, array)
- JSON output support for CI/CD
- Progress indicators
- Test timing and statistics
- Color-coded output

**Usage Example**:
```c
#include "mtestc/stest.h"

void test_example(void) {
    assert_int_equal(42, 42);
    assert_string_equal("hello", "hello");
    assert_not_null(ptr);
}

void run_all_tests(void) {
    run_test(test_example);
}

int main(int argc, char **argv) {
    return stest_testrunner(argc, argv, run_all_tests, NULL, NULL);
}
```

### 2. Adaptive Compression

Automatically detects file types and optimizes compression settings.

**Supported File Types**:
- Text files
- Executable files (ELF, PE)
- Image files (PNG, JPEG, GIF)
- Archive files (ZIP, TAR)
- Binary files

**Usage**:
```c
file_type_t type = adaptive_detect_file_type(data, size);
lzma_filter filters[LZMA_FILTERS_MAX + 1];
uint32_t preset;
uint64_t dict_size;
adaptive_get_optimal_settings(type, filters, &preset, &dict_size);
```

### 3. Compression Analytics

Tracks detailed statistics during compression operations.

**Metrics**:
- Compression ratio
- Compression/decompression time
- Memory usage
- Thread utilization
- Block count

**Usage**:
```c
analytics_init();
analytics_compress_start();
// ... perform compression ...
analytics_compress_end(uncompressed_size, compressed_size);
analytics_print_report(false); // false = human-readable, true = JSON
```

### 4. File Integrity Verification

Enhanced integrity checking and repair capabilities.

**Features**:
- File integrity verification
- Corrupted file detection
- Attempted file repair
- Detailed integrity reports

**Usage**:
```c
integrity_result_t result = integrity_verify_file("file.xz");
if (result == INTEGRITY_CORRUPTED) {
    integrity_repair_file("file.xz", "file_repaired.xz");
}
integrity_generate_report("file.xz", false);
```

## API Reference

See `FEATURES.md` for complete API documentation.

## Testing

The test suite (`tests/test_new_features.c`) includes comprehensive tests for all new features:

- Adaptive compression detection (text, images, executables, binaries)
- Analytics tracking (compression/decompression timing, statistics)
- Integrity verification (file checking, repair attempts)
- All assertion types (string, pointer, integer, float, bit, array)

Run tests with:
```bash
cd tests
make -f Makefile.mtestc test
```

## Version Information

- **Version**: 5.9.0
- **Release Date**: 2025
- **License**: Same as XZ Utils (0BSD, GPLv2+, LGPLv2.1+)

## Contributing

When adding new features:
1. Include comprehensive tests using M-testC
2. Update API documentation in header files
3. Add examples to this README
4. Update FEATURES.md

## License

Same as XZ Utils (0BSD, GPLv2+, LGPLv2.1+)

