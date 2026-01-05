// SPDX-License-Identifier: 0BSD
//
// Comprehensive test suite for Smart Compression Engine using M-testC

#include "mtestc/stest.h"
#include "../src/xz/smart_compress.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Test data
static uint8_t test_text_data[1024];
static uint8_t test_binary_data[1024];
static uint8_t test_repetitive_data[1024];

// Test smart compression initialization
void test_smart_compress_init(void) {
	smart_compress_init();
	assert_true(true); // Should not crash
}

// Test compression prediction
void test_smart_compress_predict(void) {
	smart_compress_init();
	memset(test_text_data, 'A', sizeof(test_text_data));
	
	compression_prediction_t pred = smart_compress_predict(test_text_data,
	                                                       sizeof(test_text_data),
	                                                       SMART_STRATEGY_BALANCED);
	
	assert_true(pred.predicted_ratio > 0.0);
	assert_true(pred.predicted_ratio <= 1.0);
	assert_true(pred.confidence_level > 0.0);
	assert_true(pred.confidence_level <= 1.0);
	assert_true(pred.recommended_preset >= 1);
	assert_true(pred.recommended_preset <= 9);
	assert_true(pred.estimated_output_size > 0);
	assert_true(pred.estimated_output_size <= sizeof(test_text_data));
}

// Test prediction for different strategies
void test_smart_compress_predict_strategies(void) {
	smart_compress_init();
	memset(test_text_data, 'A', sizeof(test_text_data));
	
	compression_prediction_t pred_speed = smart_compress_predict(test_text_data,
	                                                              sizeof(test_text_data),
	                                                              SMART_STRATEGY_SPEED);
	compression_prediction_t pred_ratio = smart_compress_predict(test_text_data,
	                                                              sizeof(test_text_data),
	                                                              SMART_STRATEGY_RATIO);
	
	assert_true(pred_speed.recommended_preset <= pred_ratio.recommended_preset);
	assert_true(pred_speed.estimated_time_seconds < pred_ratio.estimated_time_seconds);
}

// Test file prediction
void test_smart_compress_predict_file(void) {
	// Create a temporary test file
	const char *test_file = "/tmp/xz_test_file.txt";
	FILE *f = fopen(test_file, "wb");
	if (f != NULL) {
		fwrite(test_text_data, 1, sizeof(test_text_data), f);
		fclose(f);
		
		smart_compress_init();
		compression_prediction_t pred = smart_compress_predict_file(test_file,
		                                                            SMART_STRATEGY_BALANCED);
		
		assert_true(pred.predicted_ratio > 0.0);
		assert_true(pred.recommended_preset >= 1);
		assert_true(pred.recommended_preset <= 9);
		
		unlink(test_file);
	}
}

// Test benchmarking
void test_smart_compress_benchmark(void) {
	smart_compress_init();
	memset(test_repetitive_data, 0x42, sizeof(test_repetitive_data));
	
	uint32_t presets[] = {1, 3, 6, 9};
	benchmark_result_t results[4];
	
	size_t count = smart_compress_benchmark(test_repetitive_data,
	                                       sizeof(test_repetitive_data),
	                                       presets, 4, results,
	                                       SMART_STRATEGY_BALANCED);
	
	assert_true(count > 0);
	assert_true(count <= 4);
	
	for (size_t i = 0; i < count; i++) {
		assert_true(results[i].compression_ratio > 0.0);
		assert_true(results[i].compression_ratio <= 1.0);
		assert_true(results[i].compression_speed_mbps >= 0.0);
		assert_true(results[i].output_size_bytes > 0);
	}
}

// Test quality scoring
void test_smart_compress_quality_score(void) {
	smart_compress_init();
	
	benchmark_result_t result = {
		.preset = 6,
		.compression_ratio = 0.3,
		.compression_speed_mbps = 20.0,
		.memory_used_mb = 256
	};
	
	quality_score_t score = smart_compress_calculate_quality(&result,
	                                                          SMART_STRATEGY_BALANCED);
	
	assert_true(score.compression_ratio_score >= 0.0);
	assert_true(score.compression_ratio_score <= 100.0);
	assert_true(score.speed_score >= 0.0);
	assert_true(score.speed_score <= 100.0);
	assert_true(score.memory_efficiency_score >= 0.0);
	assert_true(score.memory_efficiency_score <= 100.0);
	assert_true(score.overall_score >= 0.0);
	assert_true(score.overall_score <= 100.0);
	assert_not_null(score.quality_grade);
	assert_true(strlen(score.quality_grade) > 0);
}

// Test recommendations
void test_smart_compress_recommendations(void) {
	const char *test_file = "/tmp/xz_test_recommend.txt";
	FILE *f = fopen(test_file, "wb");
	if (f != NULL) {
		fwrite(test_text_data, 1, sizeof(test_text_data), f);
		fclose(f);
		
		smart_compress_init();
		
		uint32_t preset;
		lzma_filter filters[LZMA_FILTERS_MAX + 1];
		uint64_t dict_size;
		uint32_t threads;
		
		smart_compress_get_recommendations(test_file, SMART_STRATEGY_BALANCED,
		                                   &preset, filters, &dict_size, &threads);
		
		assert_true(preset >= 1);
		assert_true(preset <= 9);
		assert_true(threads > 0);
		
		unlink(test_file);
	}
}

// Test statistics
void test_smart_compress_stats(void) {
	smart_compress_init();
	smart_compress_reset_stats();
	
	uint64_t files, bytes;
	double ratio, speed;
	
	smart_compress_get_stats(&files, &bytes, &ratio, &speed);
	
	assert_int_equal(0, files);
	assert_int_equal(0, bytes);
}

// Test statistics reset
void test_smart_compress_reset_stats(void) {
	smart_compress_init();
	smart_compress_reset_stats();
	
	uint64_t files;
	smart_compress_get_stats(&files, NULL, NULL, NULL);
	assert_int_equal(0, files);
}

// Test cleanup
void test_smart_compress_cleanup(void) {
	smart_compress_init();
	smart_compress_cleanup();
	assert_true(true); // Should not crash
}

// Test fixtures
void fixture_setup_smart_compress(void) {
	smart_compress_init();
	// Initialize test data
	memset(test_text_data, 'A', sizeof(test_text_data));
	memset(test_binary_data, 0, sizeof(test_binary_data));
	for (size_t i = 0; i < sizeof(test_binary_data); i++) {
		test_binary_data[i] = (uint8_t)(i % 256);
	}
	memset(test_repetitive_data, 0x42, sizeof(test_repetitive_data));
}

void fixture_teardown_smart_compress(void) {
	smart_compress_reset_stats();
}

// Test fixture
void test_fixture_smart_compress(void) {
	test_fixture_start();
	fixture_setup(fixture_setup_smart_compress);
	fixture_teardown(fixture_teardown_smart_compress);
	
	run_test(test_smart_compress_init);
	run_test(test_smart_compress_predict);
	run_test(test_smart_compress_predict_strategies);
	run_test(test_smart_compress_predict_file);
	run_test(test_smart_compress_benchmark);
	run_test(test_smart_compress_quality_score);
	run_test(test_smart_compress_recommendations);
	run_test(test_smart_compress_stats);
	run_test(test_smart_compress_reset_stats);
	run_test(test_smart_compress_cleanup);
	
	test_fixture_end();
}

// Main test suite
void run_all_tests(void) {
	test_fixture_smart_compress();
}

// Suite setup/teardown
void suite_setup_func(void) {
	// Global setup
}

void suite_teardown_func(void) {
	// Cleanup
	smart_compress_cleanup();
}

int main(int argc, char **argv) {
	return stest_testrunner(argc, argv, run_all_tests,
	                        suite_setup_func, suite_teardown_func);
}

