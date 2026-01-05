// SPDX-License-Identifier: 0BSD
//
// Advanced Parallel Processing Implementation

#include "private.h"
#include "parallel.h"
#include "hardware.h"
#include "mytime.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

static parallel_config_t current_config = {
	.threads = 0,
	.block_size = 0,
	.timeout_ms = 300,
	.adaptive_threading = false,
	.load_balancing = true,
	.priority_level = 0
};

static parallel_metrics_t metrics = {0};
static bool parallel_initialized = false;
static uint64_t profile_start_time = 0;
static uint64_t profile_end_time = 0;

void parallel_init(void) {
	if (parallel_initialized) return;
	
	// Detect optimal defaults
	current_config.threads = hardware_threads_get();
	current_config.block_size = 0; // Let liblzma decide
	current_config.adaptive_threading = true;
	current_config.load_balancing = true;
	
	parallel_reset_metrics();
	parallel_initialized = true;
}

void parallel_configure(const parallel_config_t *config) {
	if (config == NULL) return;
	memcpy(&current_config, config, sizeof(parallel_config_t));
}

uint32_t parallel_get_optimal_threads(uint64_t file_size, uint64_t available_memory) {
	if (!parallel_initialized) parallel_init();
	
	uint32_t max_threads = hardware_threads_get();
	if (max_threads == 0) max_threads = 1;
	
	// Calculate based on file size
	uint32_t size_based_threads = 1;
	if (file_size > 100 * 1024 * 1024) { // > 100 MB
		size_based_threads = max_threads;
	} else if (file_size > 10 * 1024 * 1024) { // > 10 MB
		size_based_threads = max_threads / 2;
		if (size_based_threads == 0) size_based_threads = 1;
	}
	
	// Calculate based on available memory
	// Each thread needs ~3 * block_size memory
	uint64_t block_size = current_config.block_size;
	if (block_size == 0) block_size = 3 * 1024 * 1024; // Default 3 MB
	
	uint64_t memory_per_thread = 3 * block_size;
	uint32_t memory_based_threads = available_memory / memory_per_thread;
	if (memory_based_threads == 0) memory_based_threads = 1;
	if (memory_based_threads > max_threads) memory_based_threads = max_threads;
	
	// Use minimum of both constraints
	uint32_t optimal = size_based_threads < memory_based_threads ? 
	                   size_based_threads : memory_based_threads;
	
	return optimal;
}

uint64_t parallel_get_optimal_block_size(uint32_t threads, uint64_t file_size) {
	if (threads == 0) threads = 1;
	
	// Optimal block size depends on file size and thread count
	uint64_t optimal_size = file_size / (threads * 4);
	
	// Clamp to reasonable values
	if (optimal_size < 64 * 1024) optimal_size = 64 * 1024; // Min 64 KB
	if (optimal_size > 64 * 1024 * 1024) optimal_size = 64 * 1024 * 1024; // Max 64 MB
	
	return optimal_size;
}

void parallel_enable_adaptive_threading(bool enable) {
	current_config.adaptive_threading = enable;
}

void parallel_enable_load_balancing(bool enable) {
	current_config.load_balancing = enable;
}

parallel_metrics_t parallel_get_metrics(void) {
	return metrics;
}

void parallel_reset_metrics(void) {
	memset(&metrics, 0, sizeof(metrics));
}

lzma_ret parallel_compress_advanced(lzma_stream *strm,
                                    const uint8_t *in, size_t in_size,
                                    uint8_t *out, size_t *out_size,
                                    const parallel_config_t *config) {
	if (strm == NULL || in == NULL || out == NULL || out_size == NULL) {
		return LZMA_PROG_ERROR;
	}
	
	parallel_config_t use_config = current_config;
	if (config != NULL) {
		use_config = *config;
	}
	
	// Configure multithreaded encoder
	lzma_mt mt = {
		.flags = 0,
		.threads = use_config.threads > 0 ? use_config.threads : hardware_threads_get(),
		.block_size = use_config.block_size > 0 ? use_config.block_size : 0,
		.timeout = use_config.timeout_ms,
		.preset = LZMA_PRESET_DEFAULT,
		.filters = NULL,
		.check = LZMA_CHECK_CRC64
	};
	
	lzma_ret ret = lzma_stream_encoder_mt(strm, &mt);
	if (ret != LZMA_OK) {
		return ret;
	}
	
	// Perform compression
	strm->next_in = in;
	strm->avail_in = in_size;
	strm->next_out = out;
	strm->avail_out = *out_size;
	
	uint64_t start_time = mytime_get_elapsed();
	
	ret = lzma_code(strm, LZMA_FINISH);
	
	uint64_t end_time = mytime_get_elapsed();
	double elapsed_sec = (end_time > start_time) ? (double)(end_time - start_time) / 1000.0 : 0.0;
	
	*out_size -= strm->avail_out;
	
	// Update metrics
	if (elapsed_sec > 0) {
		metrics.compression_speed_mbps = (in_size / (1024.0 * 1024.0)) / elapsed_sec;
	}
	metrics.active_threads = mt.threads;
	metrics.total_blocks_processed++;
	
	return ret;
}

lzma_ret parallel_decompress_advanced(lzma_stream *strm,
                                     const uint8_t *in, size_t in_size,
                                     uint8_t *out, size_t *out_size,
                                     const parallel_config_t *config) {
	if (strm == NULL || in == NULL || out == NULL || out_size == NULL) {
		return LZMA_PROG_ERROR;
	}
	
	parallel_config_t use_config = current_config;
	if (config != NULL) {
		use_config = *config;
	}
	
	// Configure multithreaded decoder
	lzma_mt mt = {
		.flags = LZMA_TELL_NO_CHECK | LZMA_TELL_UNSUPPORTED_CHECK | LZMA_CONCATENATED,
		.threads = use_config.threads > 0 ? use_config.threads : hardware_threads_get(),
		.block_size = use_config.block_size > 0 ? use_config.block_size : 0,
		.timeout = use_config.timeout_ms,
		.memlimit_threading = 0, // Use default
		.memlimit_stop = 0 // Use default
	};
	
	lzma_ret ret = lzma_stream_decoder_mt(&mt, strm);
	if (ret != LZMA_OK) {
		return ret;
	}
	
	// Perform decompression
	strm->next_in = in;
	strm->avail_in = in_size;
	strm->next_out = out;
	strm->avail_out = *out_size;
	
	uint64_t start_time = mytime_get_elapsed();
	
	ret = lzma_code(strm, LZMA_FINISH);
	
	uint64_t end_time = mytime_get_elapsed();
	double elapsed_sec = (end_time > start_time) ? (double)(end_time - start_time) / 1000.0 : 0.0;
	
	*out_size -= strm->avail_out;
	
	// Update metrics
	if (elapsed_sec > 0) {
		metrics.decompression_speed_mbps = (in_size / (1024.0 * 1024.0)) / elapsed_sec;
	}
	metrics.active_threads = mt.threads;
	
	return ret;
}

void parallel_profile_start(void) {
	profile_start_time = mytime_get_elapsed();
	parallel_reset_metrics();
}

void parallel_profile_end(void) {
	profile_end_time = mytime_get_elapsed();
}

void parallel_profile_print_report(bool json_output) {
	double total_time = (profile_end_time > profile_start_time) ?
	                    (double)(profile_end_time - profile_start_time) / 1000.0 : 0.0;
	
	if (json_output) {
		printf("{\n");
		printf("  \"parallel_performance\": {\n");
		printf("    \"compression_speed_mbps\": %.2f,\n", metrics.compression_speed_mbps);
		printf("    \"decompression_speed_mbps\": %.2f,\n", metrics.decompression_speed_mbps);
		printf("    \"cpu_utilization\": %.2f,\n", metrics.cpu_utilization);
		printf("    \"memory_efficiency\": %.2f,\n", metrics.memory_efficiency);
		printf("    \"active_threads\": %u,\n", metrics.active_threads);
		printf("    \"total_blocks\": %llu,\n", (unsigned long long)metrics.total_blocks_processed);
		printf("    \"average_block_time\": %.3f,\n", metrics.average_block_time);
		printf("    \"total_time\": %.3f\n", total_time);
		printf("  }\n");
		printf("}\n");
	} else {
		printf("\n=== Parallel Processing Performance ===\n");
		printf("Compression speed:    %.2f MB/s\n", metrics.compression_speed_mbps);
		printf("Decompression speed:  %.2f MB/s\n", metrics.decompression_speed_mbps);
		printf("CPU utilization:      %.2f%%\n", metrics.cpu_utilization);
		printf("Memory efficiency:    %.2f%%\n", metrics.memory_efficiency);
		printf("Active threads:       %u\n", metrics.active_threads);
		printf("Total blocks:         %llu\n", (unsigned long long)metrics.total_blocks_processed);
		printf("Average block time:   %.3f s\n", metrics.average_block_time);
		printf("Total time:           %.3f s\n", total_time);
		printf("========================================\n");
	}
}

