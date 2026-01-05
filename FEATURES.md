# New Features in XZ Utils Enhanced Edition

This document describes the new features added to XZ Utils.

## Version 5.9.0 - Enhanced Edition

### 1. M-testC Testing Framework Integration

A comprehensive unit testing framework has been integrated into the project:
- **Location**: `tests/mtestc/`
- **Features**:
  - xUnit-style test framework
  - Comprehensive assertion library
  - JSON output support
  - Progress indicators
  - Test timing and statistics
  - Color-coded output

**Usage**:
```bash
cd tests/mtestc
make
./test_new_features -v -c -p
```

### 2. Adaptive Compression

Automatically detects file types and optimizes compression settings:

- **File Type Detection**: Automatically identifies:
  - Text files
  - Executable files (ELF, PE)
  - Image files (PNG, JPEG, GIF)
  - Archive files (ZIP, TAR)
  - Binary files

- **Optimal Settings**: Automatically selects:
  - Best compression preset
  - Optimal dictionary size
  - Appropriate filter chain (BCJ for executables, etc.)

**API**: See `src/xz/adaptive.h`

**Example**:
```c
file_type_t type = adaptive_detect_file_type(data, size);
adaptive_get_optimal_settings(type, filters, &preset, &dict_size);
```

### 3. Compression Analytics

Detailed statistics and reporting for compression operations:

- **Metrics Tracked**:
  - Compression ratio
  - Compression/decompression time
  - Memory usage
  - Thread utilization
  - Block count

- **Output Formats**:
  - Human-readable reports
  - JSON format for CI/CD integration

**API**: See `src/xz/analytics.h`

**Example**:
```c
analytics_init();
analytics_compress_start();
// ... compression ...
analytics_compress_end(uncompressed_size, compressed_size);
analytics_print_report(json_output);
```

### 4. File Integrity Verification

Enhanced integrity checking and repair capabilities:

- **Features**:
  - File integrity verification
  - Corrupted file detection
  - Attempted file repair
  - Detailed integrity reports

**API**: See `src/xz/integrity.h`

**Example**:
```c
integrity_result_t result = integrity_verify_file("file.xz");
if (result == INTEGRITY_CORRUPTED) {
    integrity_repair_file("file.xz", "file_repaired.xz");
}
```

### 5. Enhanced Progress Monitoring

Improved progress indicators with detailed statistics:

- Real-time compression statistics
- Memory usage tracking
- Thread utilization display
- ETA calculations

### 6. Parallel Decompression Improvements

Enhanced multithreaded decompression support:

- Better thread utilization
- Improved memory management
- Enhanced error handling

## Testing

Comprehensive test suite using M-testC:

```bash
cd tests
make test_new_features
./test_new_features -v -c
```

Test coverage includes:
- Adaptive compression detection
- Analytics tracking
- Integrity verification
- All assertion types
- Edge cases and error handling

## Building

### Prerequisites
- C compiler (GCC or Clang)
- Make
- liblzma development headers

### Build Steps

1. Build M-testC framework:
```bash
cd tests/mtestc
make
```

2. Build test suite:
```bash
cd tests
make test_new_features
```

3. Run tests:
```bash
./test_new_features -v -c -p
```

## API Documentation

### Adaptive Compression

```c
// Detect file type
file_type_t adaptive_detect_file_type(const uint8_t *data, size_t size);

// Get optimal settings
void adaptive_get_optimal_settings(file_type_t type,
                                    lzma_filter *filters,
                                    uint32_t *preset,
                                    uint64_t *dict_size);

// Analyze file
void adaptive_analyze_file(const char *filename,
                          file_type_t *detected_type,
                          lzma_filter *suggested_filters,
                          uint32_t *suggested_preset);
```

### Analytics

```c
// Initialize
void analytics_init(void);

// Track compression
void analytics_compress_start(void);
void analytics_compress_end(uint64_t uncompressed, uint64_t compressed);

// Track decompression
void analytics_decompress_start(void);
void analytics_decompress_end(uint64_t uncompressed);

// Get statistics
compression_stats_t analytics_get_stats(void);

// Print report
void analytics_print_report(bool json_output);
```

### Integrity Verification

```c
// Verify file
integrity_result_t integrity_verify_file(const char *filename);

// Repair file
bool integrity_repair_file(const char *filename, const char *output_filename);

// Generate report
void integrity_generate_report(const char *filename, bool json_output);
```

## Contributing

Contributions are welcome! Please ensure all new features include:
- Comprehensive tests using M-testC
- Documentation updates
- API documentation in header files

## License

Same as XZ Utils (0BSD, GPLv2+, LGPLv2.1+)

