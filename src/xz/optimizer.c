// SPDX-License-Identifier: 0BSD
//
// Compression Optimizer Implementation

#include "private.h"
#include "optimizer.h"
#include "adaptive.h"
#include "mytime.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

static bool optimizer_initialized = false;
static uint32_t optimizer_tests_run = 0;
static double optimizer_best_ratio = 0.0;
static double optimizer_average_time = 0.0;

void optimizer_init(void) {
	if (optimizer_initialized) return;
	optimizer_reset();
	optimizer_initialized = true;
}

static double test_preset_compression(const uint8_t *data, size_t size,
                                     uint32_t preset, size_t *compressed_size) {
	lzma_stream strm = LZMA_STREAM_INIT;
	lzma_ret ret = lzma_easy_encoder(&strm, preset, LZMA_CHECK_CRC32);
	
	if (ret != LZMA_OK) {
		lzma_end(&strm);
		return -1.0;
	}
	
	// Use sample if size is large
	size_t test_size = size;
	if (size > 1024 * 1024) { // > 1 MB, use 1 MB sample
		test_size = 1024 * 1024;
	}
	
	uint8_t *out = malloc(test_size * 2);
	if (out == NULL) {
		lzma_end(&strm);
		return -1.0;
	}
	
	strm.next_in = data;
	strm.avail_in = test_size;
	strm.next_out = out;
	strm.avail_out = test_size * 2;
	
	uint64_t start_time = mytime_get_elapsed();
	ret = lzma_code(&strm, LZMA_FINISH);
	uint64_t end_time = mytime_get_elapsed();
	
	double elapsed = (end_time > start_time) ? 
	                (double)(end_time - start_time) / 1000.0 : 0.0;
	
	if (ret == LZMA_STREAM_END) {
		*compressed_size = (test_size * 2) - strm.avail_out;
		double ratio = (double)*compressed_size / (double)test_size;
		free(out);
		lzma_end(&strm);
		return ratio;
	}
	
	free(out);
	lzma_end(&strm);
	return -1.0;
}

optimization_result_t optimizer_analyze(const sample_data_t *sample,
                                        optimization_strategy_t strategy,
                                        uint64_t memory_limit_mb,
                                        double time_limit_seconds) {
	optimization_result_t result = {0};
	
	if (sample == NULL || sample->data == NULL || sample->size == 0) {
		return result;
	}
	
	optimizer_init();
	
	size_t test_size = sample->size;
	if (sample->sample_size > 0 && sample->sample_size < sample->size) {
		test_size = sample->sample_size;
	}
	
	// Detect file type for initial filter selection
	file_type_t file_type = adaptive_detect_file_type(sample->data, test_size);
	
	// Determine preset range based on strategy
	uint32_t min_preset = 1;
	uint32_t max_preset = 9;
	
	switch (strategy) {
	case OPT_STRATEGY_SPEED:
		min_preset = 1;
		max_preset = 4;
		break;
	case OPT_STRATEGY_RATIO:
		min_preset = 7;
		max_preset = 9;
		break;
	case OPT_STRATEGY_BALANCED:
		min_preset = 4;
		max_preset = 7;
		break;
	case OPT_STRATEGY_CUSTOM:
		min_preset = 1;
		max_preset = 9;
		break;
	}
	
	// Test presets
	double best_ratio = 1.0;
	uint32_t best_preset = LZMA_PRESET_DEFAULT;
	size_t best_compressed = 0;
	
	for (uint32_t preset = min_preset; preset <= max_preset; preset++) {
		size_t compressed = 0;
		double ratio = test_preset_compression(sample->data, test_size, preset, &compressed);
		
		if (ratio > 0 && ratio < best_ratio) {
			best_ratio = ratio;
			best_preset = preset;
			best_compressed = compressed;
		}
		
		optimizer_tests_run++;
	}
	
	result.optimal_preset = best_preset;
	result.estimated_ratio = best_ratio;
	result.estimated_speed_mbps = 10.0; // Placeholder
	
	// Set up filters based on file type
	adaptive_get_optimal_settings(file_type, result.filters,
	                              &result.optimal_preset, &result.optimal_dict_size);
	
	// Override preset with found optimal
	result.optimal_preset = best_preset;
	
	if (optimizer_best_ratio == 0.0 || best_ratio < optimizer_best_ratio) {
		optimizer_best_ratio = best_ratio;
	}
	
	return result;
}

optimization_result_t optimizer_find_best_preset(const sample_data_t *sample,
                                                 uint32_t min_preset,
                                                 uint32_t max_preset,
                                                 uint64_t memory_limit_mb) {
	return optimizer_analyze(sample, OPT_STRATEGY_CUSTOM, memory_limit_mb, 0.0);
}

void optimizer_optimize_filters(const sample_data_t *sample,
                               lzma_filter *filters,
                               uint32_t *preset,
                               optimization_strategy_t strategy) {
	if (sample == NULL || filters == NULL || preset == NULL) return;
	
	file_type_t file_type = adaptive_detect_file_type(sample->data, sample->size);
	uint64_t dict_size;
	adaptive_get_optimal_settings(file_type, filters, preset, &dict_size);
	
	// Adjust based on strategy
	if (strategy == OPT_STRATEGY_SPEED && *preset > 4) {
		*preset = 4;
	} else if (strategy == OPT_STRATEGY_RATIO && *preset < 7) {
		*preset = 7;
	}
}

void optimizer_get_stats(uint32_t *tests_run,
                        double *best_ratio,
                        double *average_time) {
	if (tests_run) *tests_run = optimizer_tests_run;
	if (best_ratio) *best_ratio = optimizer_best_ratio;
	if (average_time) *average_time = optimizer_average_time;
}

void optimizer_reset(void) {
	optimizer_tests_run = 0;
	optimizer_best_ratio = 0.0;
	optimizer_average_time = 0.0;
}

