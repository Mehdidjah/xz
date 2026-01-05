# Changelog - New Features

## Version 5.9.0 (2025) - Enhanced Edition

### Added Features

#### M-testC Testing Framework
- Integrated comprehensive unit testing framework
- xUnit-style test framework with extensive assertion library
- JSON output support for CI/CD integration
- Progress indicators and test timing
- Color-coded output support
- Test filtering capabilities

#### Adaptive Compression
- Automatic file type detection (text, executable, image, archive, binary)
- Optimal compression settings selection based on file type
- Support for magic number detection (PNG, JPEG, GIF, ELF, PE, ZIP, TAR)
- Automatic filter chain optimization
- Dictionary size optimization per file type

#### Compression Analytics
- Detailed compression statistics tracking
- Compression ratio calculation
- Compression and decompression timing
- Memory usage tracking
- Thread utilization monitoring
- Block count tracking
- JSON and human-readable report formats

#### File Integrity Verification
- File integrity verification
- Corrupted file detection
- File repair capabilities
- Detailed integrity reports
- JSON output support

### Technical Details

#### New Source Files
- `src/xz/adaptive.h` / `src/xz/adaptive.c` - Adaptive compression
- `src/xz/analytics.h` / `src/xz/analytics.c` - Compression analytics
- `src/xz/integrity.h` / `src/xz/integrity.c` - Integrity verification
- `tests/mtestc/stest.h` / `tests/mtestc/stest.c` - M-testC framework
- `tests/test_new_features.c` - Comprehensive test suite

#### Build System
- `tests/mtestc/Makefile` - M-testC framework build
- `tests/Makefile.mtestc` - Test suite build

#### Documentation
- `FEATURES.md` - Complete feature documentation
- `README_NEW_FEATURES.md` - Quick start guide
- `CHANGELOG_NEW_FEATURES.md` - This file

### Testing

Comprehensive test suite covering:
- File type detection (text, PNG, ELF, binary)
- Optimal settings selection
- Analytics tracking
- Integrity verification
- All assertion types

### API Changes

New public APIs:
- `adaptive_detect_file_type()` - Detect file type
- `adaptive_get_optimal_settings()` - Get optimal compression settings
- `adaptive_analyze_file()` - Analyze file and suggest settings
- `analytics_init()` - Initialize analytics
- `analytics_compress_start()` / `analytics_compress_end()` - Track compression
- `analytics_decompress_start()` / `analytics_decompress_end()` - Track decompression
- `analytics_get_stats()` - Get statistics
- `analytics_print_report()` - Print report
- `integrity_verify_file()` - Verify file integrity
- `integrity_repair_file()` - Repair corrupted file
- `integrity_generate_report()` - Generate integrity report

### Version

Updated from 5.8.2 to 5.9.0 to reflect new feature additions.

### Compatibility

- Maintains full backward compatibility with existing XZ Utils API
- New features are optional and can be disabled
- No breaking changes to existing functionality

