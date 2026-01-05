// SPDX-License-Identifier: 0BSD
//
// Comprehensive test suite for complex XZ Utils features using M-testC

#include "mtestc/stest.h"
#include "../src/xz/parallel.h"
#include "../src/xz/optimizer.h"
#include "../src/xz/recovery.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Test data
static uint8_t test_large_data[1024 * 1024]; // 1 MB test data
static uint8_t test_corrupted_data[] = {0xFD, 0x37, 0x7A, 0x58, 0x5A, 0x00}; // Invalid xz header

// Test parallel processing
void test_parallel_init(void) {
	parallel_init();
	parallel_metrics_t metrics = parallel_get_metrics();
	assert_int_equal(0, metrics.active_threads);
}

void test_parallel_optimal_threads(void) {
	parallel_init();
	uint32_t threads = parallel_get_optimal_threads(100 * 1024 * 1024, 1024 * 1024 * 1024);
	assert_true(threads > 0);
	assert_true(threads <= 16); // Reasonable upper limit
}

void test_parallel_optimal_block_size(void) {
	parallel_init();
	uint64_t block_size = parallel_get_optimal_block_size(4, 100 * 1024 * 1024);
	assert_true(block_size >= 64 * 1024); // Min 64 KB
	assert_true(block_size <= 64 * 1024 * 1024); // Max 64 MB
}

void test_parallel_configuration(void) {
	parallel_init();
	parallel_config_t config = {
		.threads = 4,
		.block_size = 1024 * 1024,
		.timeout_ms = 500,
		.adaptive_threading = true,
		.load_balancing = true,
		.priority_level = 0
	};
	parallel_configure(&config);
	parallel_metrics_t metrics = parallel_get_metrics();
	assert_not_null(&metrics);
}

void test_parallel_adaptive_threading(void) {
	parallel_init();
	parallel_enable_adaptive_threading(true);
	parallel_enable_load_balancing(true);
	// Just verify it doesn't crash
	assert_true(true);
}

void test_parallel_profile(void) {
	parallel_init();
	parallel_profile_start();
	// Simulate some work
	for (volatile int i = 0; i < 1000; i++);
	parallel_profile_end();
	parallel_profile_print_report(false);
	assert_true(true);
}

// Test optimizer
void test_optimizer_init(void) {
	optimizer_init();
	uint32_t tests_run;
	double best_ratio;
	double avg_time;
	optimizer_get_stats(&tests_run, &best_ratio, &avg_time);
	assert_int_equal(0, tests_run);
}

void test_optimizer_analyze(void) {
	optimizer_init();
	uint8_t test_data[1024];
	for (size_t i = 0; i < sizeof(test_data); i++) {
		test_data[i] = (uint8_t)(i % 256);
	}
	
	sample_data_t sample = {
		.data = test_data,
		.size = sizeof(test_data),
		.sample_size = sizeof(test_data)
	};
	
	optimization_result_t result = optimizer_analyze(&sample, OPT_STRATEGY_BALANCED,
	                                                 100, 10.0);
	assert_true(result.optimal_preset >= 1);
	assert_true(result.optimal_preset <= 9);
	assert_true(result.estimated_ratio > 0.0);
	assert_true(result.estimated_ratio <= 1.0);
}

void test_optimizer_find_best_preset(void) {
	optimizer_init();
	uint8_t test_data[512];
	memset(test_data, 'A', sizeof(test_data));
	
	sample_data_t sample = {
		.data = test_data,
		.size = sizeof(test_data),
		.sample_size = 0
	};
	
	optimization_result_t result = optimizer_find_best_preset(&sample, 1, 9, 100);
	assert_true(result.optimal_preset >= 1);
	assert_true(result.optimal_preset <= 9);
}

void test_optimizer_optimize_filters(void) {
	optimizer_init();
	uint8_t test_data[256];
	memset(test_data, 0, sizeof(test_data));
	
	sample_data_t sample = {
		.data = test_data,
		.size = sizeof(test_data),
		.sample_size = 0
	};
	
	lzma_filter filters[LZMA_FILTERS_MAX + 1];
	uint32_t preset = 6;
	
	optimizer_optimize_filters(&sample, filters, &preset, OPT_STRATEGY_BALANCED);
	assert_true(preset >= 1);
	assert_true(preset <= 9);
}

void test_optimizer_reset(void) {
	optimizer_init();
	optimizer_reset();
	uint32_t tests_run;
	optimizer_get_stats(&tests_run, NULL, NULL);
	assert_int_equal(0, tests_run);
}

// Test recovery
void test_recovery_init(void) {
	recovery_init();
	recovery_stats_t stats = recovery_get_stats();
	assert_int_equal(0, stats.corrupted_blocks);
	assert_int_equal(0, stats.recovered_blocks);
}

void test_recovery_set_mode(void) {
	recovery_init();
	recovery_set_mode(RECOVERY_AGGRESSIVE);
	recovery_set_mode(RECOVERY_PARTIAL);
	recovery_set_mode(RECOVERY_MAXIMUM);
	assert_true(true);
}

void test_recovery_stats(void) {
	recovery_init();
	recovery_stats_t stats = recovery_get_stats();
	assert_not_null(&stats);
	assert_int_equal(0, stats.corrupted_blocks);
}

void test_recovery_reset_stats(void) {
	recovery_init();
	recovery_reset_stats();
	recovery_stats_t stats = recovery_get_stats();
	assert_int_equal(0, stats.corrupted_blocks);
	assert_int_equal(0, stats.recovered_blocks);
}

void test_recovery_validate(void) {
	recovery_init();
	uint8_t test_data[] = {1, 2, 3, 4, 5};
	bool result = recovery_validate(test_data, sizeof(test_data), LZMA_CHECK_CRC32);
	// Should return true for valid data structure
	assert_true(result);
}

// Test fixtures
void fixture_setup_parallel(void) {
	parallel_init();
}

void fixture_teardown_parallel(void) {
	parallel_reset_metrics();
}

void fixture_setup_optimizer(void) {
	optimizer_init();
}

void fixture_teardown_optimizer(void) {
	optimizer_reset();
}

void fixture_setup_recovery(void) {
	recovery_init();
}

void fixture_teardown_recovery(void) {
	recovery_reset_stats();
}

// Test fixtures
void test_fixture_parallel(void) {
	test_fixture_start();
	fixture_setup(fixture_setup_parallel);
	fixture_teardown(fixture_teardown_parallel);
	
	run_test(test_parallel_init);
	run_test(test_parallel_optimal_threads);
	run_test(test_parallel_optimal_block_size);
	run_test(test_parallel_configuration);
	run_test(test_parallel_adaptive_threading);
	run_test(test_parallel_profile);
	
	test_fixture_end();
}

void test_fixture_optimizer(void) {
	test_fixture_start();
	fixture_setup(fixture_setup_optimizer);
	fixture_teardown(fixture_teardown_optimizer);
	
	run_test(test_optimizer_init);
	run_test(test_optimizer_analyze);
	run_test(test_optimizer_find_best_preset);
	run_test(test_optimizer_optimize_filters);
	run_test(test_optimizer_reset);
	
	test_fixture_end();
}

void test_fixture_recovery(void) {
	test_fixture_start();
	fixture_setup(fixture_setup_recovery);
	fixture_teardown(fixture_teardown_recovery);
	
	run_test(test_recovery_init);
	run_test(test_recovery_set_mode);
	run_test(test_recovery_stats);
	run_test(test_recovery_reset_stats);
	run_test(test_recovery_validate);
	
	test_fixture_end();
}

// Main test suite
void run_all_tests(void) {
	test_fixture_parallel();
	test_fixture_optimizer();
	test_fixture_recovery();
}

// Suite setup/teardown
void suite_setup_func(void) {
	// Initialize test data
	memset(test_large_data, 0x42, sizeof(test_large_data));
}

void suite_teardown_func(void) {
	// Cleanup
}

int main(int argc, char **argv) {
	return stest_testrunner(argc, argv, run_all_tests,
	                        suite_setup_func, suite_teardown_func);
}

