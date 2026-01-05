// SPDX-License-Identifier: 0BSD
//
// Advanced Parallel Processing - Enhanced multithreaded compression/decompression
// Complex feature for XZ Utils Enhanced Edition

#ifndef XZ_PARALLEL_H
#define XZ_PARALLEL_H

#include "sysdefs.h"
#include "lzma.h"
#include <stdbool.h>
#include <stdint.h>

// Parallel processing configuration
typedef struct {
	uint32_t threads;
	uint64_t block_size;
	uint32_t timeout_ms;
	bool adaptive_threading;
	bool load_balancing;
	uint32_t priority_level;
} parallel_config_t;

// Performance metrics for parallel operations
typedef struct {
	double compression_speed_mbps;
	double decompression_speed_mbps;
	double cpu_utilization;
	double memory_efficiency;
	uint32_t active_threads;
	uint64_t total_blocks_processed;
	double average_block_time;
} parallel_metrics_t;

// Initialize parallel processing system
void parallel_init(void);

// Configure parallel processing
void parallel_configure(const parallel_config_t *config);

// Get optimal thread count based on system resources
uint32_t parallel_get_optimal_threads(uint64_t file_size, uint64_t available_memory);

// Get optimal block size for given configuration
uint64_t parallel_get_optimal_block_size(uint32_t threads, uint64_t file_size);

// Enable adaptive threading (dynamically adjust threads)
void parallel_enable_adaptive_threading(bool enable);

// Enable load balancing across threads
void parallel_enable_load_balancing(bool enable);

// Get current parallel metrics
parallel_metrics_t parallel_get_metrics(void);

// Reset metrics
void parallel_reset_metrics(void);

// Advanced parallel compression with dynamic optimization
lzma_ret parallel_compress_advanced(lzma_stream *strm, 
                                    const uint8_t *in, size_t in_size,
                                    uint8_t *out, size_t *out_size,
                                    const parallel_config_t *config);

// Advanced parallel decompression with error recovery
lzma_ret parallel_decompress_advanced(lzma_stream *strm,
                                      const uint8_t *in, size_t in_size,
                                      uint8_t *out, size_t *out_size,
                                      const parallel_config_t *config);

// Profile parallel performance
void parallel_profile_start(void);
void parallel_profile_end(void);
void parallel_profile_print_report(bool json_output);

#endif // XZ_PARALLEL_H

