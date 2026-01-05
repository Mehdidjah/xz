# Complex Features Documentation

## Overview

This document describes the complex features added to XZ Utils Enhanced Edition v5.9.0.

## 1. Advanced Parallel Processing (`src/xz/parallel.*`)

### Features

- **Dynamic Thread Optimization**: Automatically determines optimal thread count based on file size and available memory
- **Adaptive Threading**: Dynamically adjusts thread count during processing
- **Load Balancing**: Distributes work evenly across threads
- **Performance Profiling**: Tracks compression/decompression speeds, CPU utilization, and memory efficiency
- **Block Size Optimization**: Calculates optimal block sizes for parallel processing

### API

```c
// Initialize parallel processing
void parallel_init(void);

// Configure parallel processing
void parallel_configure(const parallel_config_t *config);

// Get optimal thread count
uint32_t parallel_get_optimal_threads(uint64_t file_size, uint64_t available_memory);

// Get optimal block size
uint64_t parallel_get_optimal_block_size(uint32_t threads, uint64_t file_size);

// Advanced compression with parallel processing
lzma_ret parallel_compress_advanced(lzma_stream *strm, 
                                    const uint8_t *in, size_t in_size,
                                    uint8_t *out, size_t *out_size,
                                    const parallel_config_t *config);

// Advanced decompression with parallel processing
lzma_ret parallel_decompress_advanced(lzma_stream *strm,
                                      const uint8_t *in, size_t in_size,
                                      uint8_t *out, size_t *out_size,
                                      const parallel_config_t *config);

// Performance profiling
void parallel_profile_start(void);
void parallel_profile_end(void);
void parallel_profile_print_report(bool json_output);
```

### Example

```c
parallel_init();
parallel_config_t config = {
    .threads = 0,  // Auto-detect
    .block_size = 0,  // Auto-calculate
    .timeout_ms = 300,
    .adaptive_threading = true,
    .load_balancing = true
};
parallel_configure(&config);

uint32_t optimal_threads = parallel_get_optimal_threads(file_size, available_memory);
uint64_t optimal_block = parallel_get_optimal_block_size(optimal_threads, file_size);

parallel_profile_start();
// Perform compression/decompression
parallel_profile_end();
parallel_profile_print_report(false);
```

## 2. Compression Optimizer (`src/xz/optimizer.*`)

### Features

- **Preset Optimization**: Tests multiple presets to find optimal compression settings
- **Strategy-Based Optimization**: Optimize for speed, ratio, or balanced performance
- **Filter Chain Optimization**: Automatically selects best filter chain
- **Memory-Aware Optimization**: Considers memory limits when optimizing
- **Time-Limited Optimization**: Can limit optimization time

### API

```c
// Initialize optimizer
void optimizer_init(void);

// Analyze data and suggest optimal settings
optimization_result_t optimizer_analyze(const sample_data_t *sample,
                                        optimization_strategy_t strategy,
                                        uint64_t memory_limit_mb,
                                        double time_limit_seconds);

// Find best preset
optimization_result_t optimizer_find_best_preset(const sample_data_t *sample,
                                                  uint32_t min_preset,
                                                  uint32_t max_preset,
                                                  uint64_t memory_limit_mb);

// Optimize filter chain
void optimizer_optimize_filters(const sample_data_t *sample,
                                lzma_filter *filters,
                                uint32_t *preset,
                                optimization_strategy_t strategy);
```

### Example

```c
optimizer_init();
uint8_t data[1024];
// ... fill data ...

sample_data_t sample = {
    .data = data,
    .size = sizeof(data),
    .sample_size = sizeof(data)
};

optimization_result_t result = optimizer_analyze(&sample, 
                                                  OPT_STRATEGY_BALANCED,
                                                  100,  // 100 MB limit
                                                  10.0); // 10 second limit

printf("Optimal preset: %u\n", result.optimal_preset);
printf("Estimated ratio: %.2f%%\n", result.estimated_ratio * 100);
```

## 3. Advanced Error Recovery (`src/xz/recovery.*`)

### Features

- **Multiple Recovery Modes**: None, Partial, Aggressive, Maximum
- **Corrupted Block Recovery**: Attempts to recover data from corrupted blocks
- **File-Level Recovery**: Recovers entire corrupted files
- **Recovery Statistics**: Tracks recovery success rates
- **Data Validation**: Validates recovered data integrity

### API

```c
// Initialize recovery system
void recovery_init(void);

// Set recovery mode
void recovery_set_mode(recovery_mode_t mode);

// Attempt to recover corrupted data
bool recovery_attempt(const uint8_t *corrupted_data, size_t corrupted_size,
                     uint8_t *recovered_data, size_t *recovered_size,
                     recovery_mode_t mode);

// Recover file with multiple strategies
bool recovery_recover_file(const char *corrupted_file,
                          const char *output_file,
                          recovery_mode_t mode);

// Get recovery statistics
recovery_stats_t recovery_get_stats(void);

// Validate recovered data
bool recovery_validate(const uint8_t *data, size_t size, lzma_check check_type);
```

### Example

```c
recovery_init();
recovery_set_mode(RECOVERY_AGGRESSIVE);

bool success = recovery_recover_file("corrupted.xz", "recovered.xz", 
                                     RECOVERY_AGGRESSIVE);

if (success) {
    recovery_stats_t stats = recovery_get_stats();
    printf("Recovered %u blocks out of %u corrupted blocks\n",
           stats.recovered_blocks, stats.corrupted_blocks);
    printf("Recovery rate: %.2f%%\n", stats.recovery_rate * 100);
}
```

## Testing

All complex features are tested in `tests/test_complex_features.c`:

```bash
cd tests
make -f Makefile.mtestc test_complex_features
./test_complex_features -v -c -p
```

## Performance Considerations

- **Parallel Processing**: Can significantly improve performance on multi-core systems
- **Optimizer**: May take time to test multiple presets, but finds optimal settings
- **Recovery**: May be slower than normal decompression but can recover corrupted data

## Integration

These features can be used independently or together:

```c
// Use optimizer to find best settings
optimization_result_t opt = optimizer_analyze(&sample, OPT_STRATEGY_BALANCED, 100, 10.0);

// Configure parallel processing
parallel_config_t config = {
    .threads = parallel_get_optimal_threads(file_size, available_memory),
    .block_size = opt.optimal_dict_size,
    .adaptive_threading = true
};
parallel_configure(&config);

// Perform compression with optimized settings
parallel_compress_advanced(&strm, in_data, in_size, out_data, &out_size, &config);

// If corruption detected, use recovery
if (corruption_detected) {
    recovery_recover_file("file.xz", "recovered.xz", RECOVERY_AGGRESSIVE);
}
```

