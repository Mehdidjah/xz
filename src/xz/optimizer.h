// SPDX-License-Identifier: 0BSD
//
// Compression Optimizer - Advanced preset optimization and tuning
// Complex feature for XZ Utils Enhanced Edition

#ifndef XZ_OPTIMIZER_H
#define XZ_OPTIMIZER_H

#include "sysdefs.h"
#include "lzma.h"
#include <stdbool.h>
#include <stdint.h>

// Optimization strategy
typedef enum {
	OPT_STRATEGY_SPEED,      // Optimize for speed
	OPT_STRATEGY_RATIO,      // Optimize for compression ratio
	OPT_STRATEGY_BALANCED,   // Balanced optimization
	OPT_STRATEGY_CUSTOM      // Custom optimization
} optimization_strategy_t;

// Optimization result
typedef struct {
	uint32_t optimal_preset;
	uint64_t optimal_dict_size;
	lzma_filter filters[LZMA_FILTERS_MAX + 1];
	double estimated_ratio;
	double estimated_speed_mbps;
	uint64_t estimated_memory_mb;
} optimization_result_t;

// Sample data for optimization
typedef struct {
	const uint8_t *data;
	size_t size;
	size_t sample_size; // How much to sample (0 = all)
} sample_data_t;

// Initialize optimizer
void optimizer_init(void);

// Analyze data and suggest optimal settings
optimization_result_t optimizer_analyze(const sample_data_t *sample,
                                        optimization_strategy_t strategy,
                                        uint64_t memory_limit_mb,
                                        double time_limit_seconds);

// Test multiple presets and find best
optimization_result_t optimizer_find_best_preset(const sample_data_t *sample,
                                                  uint32_t min_preset,
                                                  uint32_t max_preset,
                                                  uint64_t memory_limit_mb);

// Optimize filter chain
void optimizer_optimize_filters(const sample_data_t *sample,
                                lzma_filter *filters,
                                uint32_t *preset,
                                optimization_strategy_t strategy);

// Get optimization statistics
void optimizer_get_stats(uint32_t *tests_run,
                        double *best_ratio,
                        double *average_time);

// Reset optimizer state
void optimizer_reset(void);

#endif // XZ_OPTIMIZER_H

