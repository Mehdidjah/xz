// SPDX-License-Identifier: 0BSD
//
// Compression Analytics - Detailed statistics and reporting
// New feature for XZ Utils

#ifndef XZ_ANALYTICS_H
#define XZ_ANALYTICS_H

#include "sysdefs.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct {
	uint64_t uncompressed_size;
	uint64_t compressed_size;
	double compression_ratio;
	double compression_time;
	double decompression_time;
	uint32_t blocks_count;
	uint64_t memory_used;
	uint32_t threads_used;
} compression_stats_t;

// Initialize analytics
void analytics_init(void);

// Record compression start
void analytics_compress_start(void);

// Record compression end
void analytics_compress_end(uint64_t uncompressed, uint64_t compressed);

// Record decompression start
void analytics_decompress_start(void);

// Record decompression end(uint64_t uncompressed);

// Get current statistics
compression_stats_t analytics_get_stats(void);

// Print statistics report
void analytics_print_report(bool json_output);

// Reset statistics
void analytics_reset(void);

#endif // XZ_ANALYTICS_H

