// SPDX-License-Identifier: 0BSD
//
// Compression Analytics Implementation

#include "private.h"
#include "analytics.h"
#include "mytime.h"
#include <stdio.h>
#include <string.h>

static compression_stats_t stats = {0};
static uint64_t compress_start_time = 0;
static uint64_t decompress_start_time = 0;
static bool analytics_enabled = false;

void analytics_init(void) {
	analytics_enabled = true;
	analytics_reset();
}

void analytics_reset(void) {
	memset(&stats, 0, sizeof(stats));
	compress_start_time = 0;
	decompress_start_time = 0;
}

void analytics_compress_start(void) {
	if (!analytics_enabled) return;
	compress_start_time = mytime_get_elapsed();
}

void analytics_compress_end(uint64_t uncompressed, uint64_t compressed) {
	if (!analytics_enabled) return;
	
	uint64_t elapsed = mytime_get_elapsed();
	stats.compression_time = (elapsed > compress_start_time) ? 
		(double)(elapsed - compress_start_time) / 1000.0 : 0.0;
	stats.uncompressed_size = uncompressed;
	stats.compressed_size = compressed;
	
	if (uncompressed > 0) {
		stats.compression_ratio = (double)compressed / (double)uncompressed;
	}
}

void analytics_decompress_start(void) {
	if (!analytics_enabled) return;
	decompress_start_time = mytime_get_elapsed();
}

void analytics_decompress_end(uint64_t uncompressed) {
	if (!analytics_enabled) return;
	
	uint64_t elapsed = mytime_get_elapsed();
	stats.decompression_time = (elapsed > decompress_start_time) ?
		(double)(elapsed - decompress_start_time) / 1000.0 : 0.0;
	stats.uncompressed_size = uncompressed;
}

compression_stats_t analytics_get_stats(void) {
	return stats;
}

void analytics_print_report(bool json_output) {
	if (!analytics_enabled) return;
	
	if (json_output) {
		printf("{\n");
		printf("  \"compression\": {\n");
		printf("    \"uncompressed_size\": %llu,\n", 
		       (unsigned long long)stats.uncompressed_size);
		printf("    \"compressed_size\": %llu,\n",
		       (unsigned long long)stats.compressed_size);
		printf("    \"ratio\": %.4f,\n", stats.compression_ratio);
		printf("    \"time\": %.3f,\n", stats.compression_time);
		printf("    \"decompression_time\": %.3f,\n", stats.decompression_time);
		printf("    \"blocks\": %u,\n", stats.blocks_count);
		printf("    \"memory_mb\": %llu,\n",
		       (unsigned long long)(stats.memory_used / (1024 * 1024)));
		printf("    \"threads\": %u\n", stats.threads_used);
		printf("  }\n");
		printf("}\n");
	} else {
		printf("\n=== Compression Analytics ===\n");
		printf("Uncompressed size: %llu bytes (%.2f MB)\n",
		       (unsigned long long)stats.uncompressed_size,
		       stats.uncompressed_size / (1024.0 * 1024.0));
		printf("Compressed size:   %llu bytes (%.2f MB)\n",
		       (unsigned long long)stats.compressed_size,
		       stats.compressed_size / (1024.0 * 1024.0));
		printf("Compression ratio: %.2f%%\n", stats.compression_ratio * 100.0);
		printf("Compression time:  %.3f seconds\n", stats.compression_time);
		printf("Decompression time: %.3f seconds\n", stats.decompression_time);
		printf("Blocks:            %u\n", stats.blocks_count);
		printf("Memory used:       %llu MB\n",
		       (unsigned long long)(stats.memory_used / (1024 * 1024)));
		printf("Threads used:      %u\n", stats.threads_used);
		printf("===========================\n");
	}
}

