# Smart Compression Engine - Professional Feature Documentation

## Overview

The **Smart Compression Engine** is a sophisticated, professional-grade compression intelligence system that brings advanced capabilities to XZ Utils. This feature provides compression prediction, multi-stream processing, benchmarking, and quality scoring without requiring actual compression operations.

**Version**: 5.9.0  
**Status**: Production Ready  
**License**: Same as XZ Utils (0BSD, GPLv2+, LGPLv2.1+)

## Key Features

### 1. Compression Prediction
Predict compression ratios, output sizes, and performance metrics **before** compressing files. Uses entropy analysis and file type detection to provide accurate estimates.

### 2. Multi-Stream Compression
Efficiently compress multiple files in a single operation with optimized resource management and parallel processing support.

### 3. Compression Benchmarking
Compare different compression presets and strategies to find the optimal settings for your specific use case.

### 4. Quality Scoring System
Get letter-grade quality scores (A+, A, B+, etc.) based on compression ratio, speed, and memory efficiency.

### 5. Intelligent Recommendations
Automatically recommends optimal compression settings based on file analysis and selected strategy.

## Architecture

The Smart Compression Engine integrates seamlessly with existing XZ Utils features:

- **Adaptive Compression**: File type detection and optimization
- **Compression Optimizer**: Preset and filter optimization
- **Parallel Processing**: Multi-threaded compression support
- **Analytics**: Performance tracking and statistics

## API Reference

### Initialization

```c
void smart_compress_init(void);
```

Initialize the Smart Compression Engine. Must be called before using other functions.

### Compression Prediction

```c
compression_prediction_t smart_compress_predict(
    const uint8_t *data,
    size_t data_size,
    smart_strategy_t strategy
);
```

Predict compression characteristics without actually compressing.

**Parameters**:
- `data`: Input data buffer
- `data_size`: Size of input data
- `strategy`: Compression strategy (SPEED, RATIO, BALANCED, MEMORY_EFFICIENT)

**Returns**: `compression_prediction_t` structure containing:
- `predicted_ratio`: Estimated compression ratio (0.0-1.0)
- `confidence_level`: Confidence in prediction (0.0-1.0)
- `estimated_output_size`: Estimated compressed size in bytes
- `recommended_preset`: Recommended compression preset (1-9)
- `estimated_time_seconds`: Estimated compression time
- `estimated_memory_mb`: Estimated memory usage

**Example**:
```c
uint8_t data[1024 * 1024]; // 1 MB of data
// ... fill data ...

compression_prediction_t pred = smart_compress_predict(
    data, sizeof(data), SMART_STRATEGY_BALANCED
);

printf("Predicted ratio: %.2f%%\n", pred.predicted_ratio * 100);
printf("Estimated output: %llu bytes\n", 
       (unsigned long long)pred.estimated_output_size);
printf("Recommended preset: %u\n", pred.recommended_preset);
```

### File Prediction

```c
compression_prediction_t smart_compress_predict_file(
    const char *filename,
    smart_strategy_t strategy
);
```

Predict compression for a file without reading it entirely into memory.

**Example**:
```c
compression_prediction_t pred = smart_compress_predict_file(
    "large_file.dat", SMART_STRATEGY_RATIO
);
```

### Multi-Stream Compression

```c
lzma_ret smart_compress_multi_stream(
    const multi_stream_config_t *config
);
```

Compress multiple files efficiently in a single operation.

**Configuration Structure**:
```c
typedef struct {
    const char **input_files;    // Array of input file paths
    size_t num_files;            // Number of files
    const char *output_file;     // Output file path
    uint32_t threads;            // Number of threads
    bool preserve_order;         // Preserve file order
    bool merge_streams;          // Merge into single stream
    uint32_t buffer_size_mb;    // Buffer size per file
} multi_stream_config_t;
```

**Example**:
```c
const char *files[] = {"file1.txt", "file2.txt", "file3.txt"};
multi_stream_config_t config = {
    .input_files = files,
    .num_files = 3,
    .output_file = "output.xz",
    .threads = 4,
    .preserve_order = true,
    .merge_streams = true,
    .buffer_size_mb = 64
};

lzma_ret ret = smart_compress_multi_stream(&config);
```

### Compression Benchmarking

```c
size_t smart_compress_benchmark(
    const uint8_t *data,
    size_t data_size,
    uint32_t *presets,
    size_t num_presets,
    benchmark_result_t *results,
    smart_strategy_t strategy
);
```

Benchmark different compression presets to find optimal settings.

**Example**:
```c
uint8_t data[1024 * 1024];
uint32_t presets[] = {1, 3, 6, 9};
benchmark_result_t results[4];

size_t count = smart_compress_benchmark(
    data, sizeof(data), presets, 4, results,
    SMART_STRATEGY_BALANCED
);

for (size_t i = 0; i < count; i++) {
    printf("Preset %u: Ratio=%.2f%%, Speed=%.2f MB/s\n",
           results[i].preset,
           results[i].compression_ratio * 100,
           results[i].compression_speed_mbps);
}
```

### Quality Scoring

```c
quality_score_t smart_compress_calculate_quality(
    const benchmark_result_t *result,
    smart_strategy_t strategy
);
```

Calculate quality score for compression results.

**Returns**: `quality_score_t` structure containing:
- `compression_ratio_score`: Score based on ratio (0-100)
- `speed_score`: Score based on speed (0-100)
- `memory_efficiency_score`: Score based on memory (0-100)
- `overall_score`: Overall quality score (0-100)
- `quality_grade`: Letter grade (A+, A, B+, B, C+, C, D, F)

**Example**:
```c
benchmark_result_t result = { /* ... */ };
quality_score_t score = smart_compress_calculate_quality(
    &result, SMART_STRATEGY_BALANCED
);

printf("Overall Score: %.1f/100\n", score.overall_score);
printf("Grade: %s\n", score.quality_grade);
```

### Smart Compression with Recommendations

```c
lzma_ret smart_compress_file(
    const char *input_file,
    const char *output_file,
    smart_strategy_t strategy,
    quality_score_t *quality_out
);
```

Compress a file using intelligent recommendations with optional quality scoring.

**Example**:
```c
quality_score_t quality;
lzma_ret ret = smart_compress_file(
    "input.txt", "output.xz",
    SMART_STRATEGY_BALANCED, &quality
);

if (ret == LZMA_OK) {
    printf("Compression successful!\n");
    printf("Quality Grade: %s\n", quality.quality_grade);
}
```

### Get Recommendations

```c
void smart_compress_get_recommendations(
    const char *filename,
    smart_strategy_t strategy,
    uint32_t *preset,
    lzma_filter *filters,
    uint64_t *dict_size,
    uint32_t *threads
);
```

Get recommended compression settings for a file.

**Example**:
```c
uint32_t preset;
lzma_filter filters[LZMA_FILTERS_MAX + 1];
uint64_t dict_size;
uint32_t threads;

smart_compress_get_recommendations(
    "file.dat", SMART_STRATEGY_RATIO,
    &preset, filters, &dict_size, &threads
);

printf("Recommended preset: %u\n", preset);
printf("Recommended threads: %u\n", threads);
```

### Statistics

```c
void smart_compress_get_stats(
    uint64_t *total_files_processed,
    uint64_t *total_bytes_compressed,
    double *average_ratio,
    double *average_speed_mbps
);

void smart_compress_reset_stats(void);
```

Get and reset engine statistics.

## Compression Strategies

- **SMART_STRATEGY_AUTO**: Automatically choose best strategy
- **SMART_STRATEGY_SPEED**: Optimize for compression speed
- **SMART_STRATEGY_RATIO**: Optimize for compression ratio
- **SMART_STRATEGY_BALANCED**: Balance speed and ratio
- **SMART_STRATEGY_MEMORY_EFFICIENT**: Optimize for memory usage
- **SMART_STRATEGY_CUSTOM**: Custom strategy

## Use Cases

### 1. Pre-Compression Analysis
```c
// Predict compression before committing resources
compression_prediction_t pred = smart_compress_predict_file(
    "large_file.dat", SMART_STRATEGY_BALANCED
);

if (pred.predicted_ratio > 0.5) {
    printf("File may not compress well. Consider alternatives.\n");
}
```

### 2. Batch Compression Optimization
```c
// Find best preset for multiple files
uint32_t presets[] = {1, 3, 6, 9};
benchmark_result_t results[4];

smart_compress_benchmark(data, size, presets, 4, results,
                        SMART_STRATEGY_BALANCED);

// Use preset with best ratio
uint32_t best_preset = results[0].preset;
```

### 3. Quality Assurance
```c
// Ensure compression meets quality standards
quality_score_t quality;
smart_compress_file("input.txt", "output.xz",
                   SMART_STRATEGY_BALANCED, &quality);

if (quality.overall_score < 70) {
    printf("Warning: Low quality compression\n");
}
```

## Performance Considerations

- **Prediction**: Very fast, uses sampling for large files
- **Benchmarking**: May take time as it tests multiple presets
- **Multi-stream**: Efficient for batch operations
- **Memory**: Uses reasonable memory buffers, configurable

## Integration with Existing Features

The Smart Compression Engine integrates with:

- **Adaptive Compression**: Uses file type detection
- **Compression Optimizer**: Leverages optimization algorithms
- **Parallel Processing**: Supports multi-threaded operations
- **Analytics**: Tracks performance metrics

## Testing

Comprehensive test suite available in `tests/test_smart_compress.c`:

```bash
cd tests
make -f Makefile.mtestc test_smart_compress
./test_smart_compress -v -c
```

## Error Handling

All functions return appropriate error codes:
- `LZMA_OK`: Success
- `LZMA_PROG_ERROR`: Programming error (invalid parameters)
- `LZMA_MEM_ERROR`: Memory allocation error
- `LZMA_DATA_ERROR`: Data error

## Best Practices

1. **Always initialize**: Call `smart_compress_init()` before use
2. **Use appropriate strategy**: Choose strategy based on your priorities
3. **Check return values**: Always verify return codes
4. **Cleanup resources**: Call `smart_compress_cleanup()` when done
5. **Monitor statistics**: Use statistics to track performance

## Examples

See `tests/test_smart_compress.c` for comprehensive examples.

## Contributing

When contributing to the Smart Compression Engine:

1. Follow existing code style
2. Add comprehensive tests
3. Update documentation
4. Ensure backward compatibility

## License

Same as XZ Utils (0BSD, GPLv2+, LGPLv2.1+)

## Version History

- **5.9.0**: Initial release with full feature set

## Support

For issues and questions:
- GitHub Issues: https://github.com/Mehdidjah/xz/issues
- Documentation: See `FEATURES.md` and `COMPLEX_FEATURES.md`

