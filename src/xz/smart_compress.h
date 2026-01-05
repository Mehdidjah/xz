// SPDX-License-Identifier: 0BSD
//
// Smart Compression Engine - Advanced compression intelligence and optimization
// Professional-grade feature for XZ Utils Enhanced Edition
//
// Author: XZ Utils Enhanced Edition Team
// Version: 5.9.0

#ifndef XZ_SMART_COMPRESS_H
#define XZ_SMART_COMPRESS_H

#include "sysdefs.h"
#include "lzma.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// Compression prediction result
typedef struct {
	double predicted_ratio;           // Predicted compression ratio (0.0-1.0)
	double confidence_level;          // Confidence in prediction (0.0-1.0)
	uint64_t estimated_output_size;   // Estimated compressed size in bytes
	uint32_t recommended_preset;      // Recommended compression preset
	double estimated_time_seconds;    // Estimated compression time
	uint64_t estimated_memory_mb;     // Estimated memory usage in MB
} compression_prediction_t;

// Multi-stream compression configuration
typedef struct {
	const char **input_files;         // Array of input file paths
	size_t num_files;                 // Number of files
	const char *output_file;          // Output file path
	uint32_t threads;                 // Number of threads to use
	bool preserve_order;              // Preserve file order in output
	bool merge_streams;               // Merge into single stream
	uint32_t buffer_size_mb;          // Buffer size per file in MB
} multi_stream_config_t;

// Compression benchmark result
typedef struct {
	uint32_t preset;                  // Preset tested
	double compression_ratio;         // Actual compression ratio
	double compression_speed_mbps;    // Compression speed in MB/s
	double decompression_speed_mbps;  // Decompression speed in MB/s
	uint64_t memory_used_mb;          // Memory used in MB
	double compression_time_sec;      // Compression time in seconds
	double decompression_time_sec;    // Decompression time in seconds
	uint64_t output_size_bytes;       // Output size in bytes
} benchmark_result_t;

// Quality score metrics
typedef struct {
	double compression_ratio_score;   // Score based on ratio (0-100)
	double speed_score;                // Score based on speed (0-100)
	double memory_efficiency_score;    // Score based on memory (0-100)
	double overall_score;              // Overall quality score (0-100)
	char quality_grade[3];            // Letter grade (A+, A, B+, etc.)
} quality_score_t;

// Compression strategy
typedef enum {
	SMART_STRATEGY_AUTO,              // Automatically choose best strategy
	SMART_STRATEGY_SPEED,             // Optimize for speed
	SMART_STRATEGY_RATIO,              // Optimize for compression ratio
	SMART_STRATEGY_BALANCED,           // Balance speed and ratio
	SMART_STRATEGY_MEMORY_EFFICIENT,   // Optimize for memory usage
	SMART_STRATEGY_CUSTOM              // Custom strategy
} smart_strategy_t;

// Initialize smart compression engine
void smart_compress_init(void);

// Predict compression ratio without actually compressing
compression_prediction_t smart_compress_predict(const uint8_t *data,
                                                 size_t data_size,
                                                 smart_strategy_t strategy);

// Predict compression for a file
compression_prediction_t smart_compress_predict_file(const char *filename,
                                                     smart_strategy_t strategy);

// Compress multiple files efficiently
lzma_ret smart_compress_multi_stream(const multi_stream_config_t *config);

// Benchmark different compression settings
size_t smart_compress_benchmark(const uint8_t *data,
                                size_t data_size,
                                uint32_t *presets,
                                size_t num_presets,
                                benchmark_result_t *results,
                                smart_strategy_t strategy);

// Calculate quality score for compression result
quality_score_t smart_compress_calculate_quality(const benchmark_result_t *result,
                                                 smart_strategy_t strategy);

// Get recommended settings based on file analysis
void smart_compress_get_recommendations(const char *filename,
                                       smart_strategy_t strategy,
                                       uint32_t *preset,
                                       lzma_filter *filters,
                                       uint64_t *dict_size,
                                       uint32_t *threads);

// Smart compression with automatic optimization
lzma_ret smart_compress_file(const char *input_file,
                             const char *output_file,
                             smart_strategy_t strategy,
                             quality_score_t *quality_out);

// Get engine statistics
void smart_compress_get_stats(uint64_t *total_files_processed,
                              uint64_t *total_bytes_compressed,
                              double *average_ratio,
                              double *average_speed_mbps);

// Reset engine statistics
void smart_compress_reset_stats(void);

// Cleanup resources
void smart_compress_cleanup(void);

#endif // XZ_SMART_COMPRESS_H

