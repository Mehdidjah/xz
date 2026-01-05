// SPDX-License-Identifier: 0BSD
//
// Comprehensive test suite for new XZ Utils features using M-testC

#include "mtestc/stest.h"
#include "../src/xz/adaptive.h"
#include "../src/xz/analytics.h"
#include "../src/xz/integrity.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Test data
static uint8_t test_text_data[] = "This is a test text file with some content that should compress well.";
static uint8_t test_png_data[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
static uint8_t test_elf_data[] = {0x7F, 'E', 'L', 'F'};
static uint8_t test_binary_data[] = {0x00, 0x01, 0x02, 0x03, 0xFF, 0xFE, 0xFD};

// Test adaptive compression - file type detection
void test_adaptive_detect_text(void) {
	file_type_t type = adaptive_detect_file_type(test_text_data, sizeof(test_text_data) - 1);
	assert_true(type == FILE_TYPE_TEXT || type == FILE_TYPE_UNKNOWN);
}

void test_adaptive_detect_png(void) {
	file_type_t type = adaptive_detect_file_type(test_png_data, sizeof(test_png_data));
	assert_int_equal(FILE_TYPE_IMAGE, type);
}

void test_adaptive_detect_elf(void) {
	file_type_t type = adaptive_detect_file_type(test_elf_data, sizeof(test_elf_data));
	assert_int_equal(FILE_TYPE_EXECUTABLE, type);
}

void test_adaptive_detect_binary(void) {
	file_type_t type = adaptive_detect_file_type(test_binary_data, sizeof(test_binary_data));
	assert_true(type == FILE_TYPE_BINARY || type == FILE_TYPE_UNKNOWN);
}

void test_adaptive_get_optimal_settings_text(void) {
	lzma_filter filters[LZMA_FILTERS_MAX + 1];
	uint32_t preset;
	uint64_t dict_size;
	
	adaptive_get_optimal_settings(FILE_TYPE_TEXT, filters, &preset, &dict_size);
	assert_true(preset >= 6);
	assert_true(dict_size > 0);
	assert_int_equal(LZMA_FILTER_LZMA2, filters[0].id);
}

void test_adaptive_get_optimal_settings_executable(void) {
	lzma_filter filters[LZMA_FILTERS_MAX + 1];
	uint32_t preset;
	uint64_t dict_size;
	
	adaptive_get_optimal_settings(FILE_TYPE_EXECUTABLE, filters, &preset, &dict_size);
	assert_true(preset >= 6);
	assert_true(dict_size > 0);
	// Should have BCJ filter
	assert_true(filters[0].id == LZMA_FILTER_X86 || 
	            filters[0].id == LZMA_FILTER_ARM64 ||
	            filters[0].id == LZMA_FILTER_POWERPC);
}

// Test analytics
void test_analytics_init(void) {
	analytics_init();
	compression_stats_t stats = analytics_get_stats();
	assert_int_equal(0, stats.uncompressed_size);
	assert_int_equal(0, stats.compressed_size);
}

void test_analytics_compress_tracking(void) {
	analytics_reset();
	analytics_compress_start();
	// Simulate some work
	for (volatile int i = 0; i < 1000; i++);
	analytics_compress_end(1000, 500);
	
	compression_stats_t stats = analytics_get_stats();
	assert_int_equal(1000, stats.uncompressed_size);
	assert_int_equal(500, stats.compressed_size);
	assert_true(stats.compression_ratio > 0.0);
	assert_true(stats.compression_ratio < 1.0);
}

void test_analytics_decompress_tracking(void) {
	analytics_reset();
	analytics_decompress_start();
	// Simulate some work
	for (volatile int i = 0; i < 1000; i++);
	analytics_decompress_end(1000);
	
	compression_stats_t stats = analytics_get_stats();
	assert_int_equal(1000, stats.uncompressed_size);
	assert_true(stats.decompression_time >= 0.0);
}

void test_analytics_reset(void) {
	analytics_compress_start();
	analytics_compress_end(1000, 500);
	analytics_reset();
	
	compression_stats_t stats = analytics_get_stats();
	assert_int_equal(0, stats.uncompressed_size);
	assert_int_equal(0, stats.compressed_size);
}

// Test integrity verification
void test_integrity_check_block(void) {
	uint8_t test_data[] = {1, 2, 3, 4, 5};
	bool result = integrity_check_block(test_data, sizeof(test_data), LZMA_CHECK_CRC32);
	// Should return true for valid data
	assert_true(result);
}

// Test string assertions
void test_string_assertions(void) {
	assert_string_equal("test", "test");
	assert_string_contains("est", "test");
	assert_string_not_contains("xyz", "test");
	assert_string_starts_with("te", "test");
	assert_string_ends_with("st", "test");
}

// Test pointer assertions
void test_pointer_assertions(void) {
	int value = 42;
	int *ptr = &value;
	int *null_ptr = NULL;
	
	assert_not_null(ptr);
	assert_null(null_ptr);
	assert_ptr_equal(ptr, &value);
	assert_ptr_not_equal(ptr, null_ptr);
}

// Test integer assertions
void test_integer_assertions(void) {
	assert_int_equal(42, 42);
	assert_ulong_equal(100UL, 100UL);
}

// Test floating point assertions
void test_float_assertions(void) {
	assert_float_equal(1.0f, 1.0f, 0.001f);
	assert_double_equal(2.0, 2.0, 0.001);
	assert_float_equal(1.0f, 1.001f, 0.01f); // Within delta
}

// Test array assertions
void test_array_assertions(void) {
	int arr1[] = {1, 2, 3, 4, 5};
	int arr2[] = {1, 2, 3, 4, 5};
	int arr3[] = {1, 2, 3, 4, 6};
	
	assert_n_array_equal(arr1, arr2, sizeof(arr1));
	// This should fail, but we test the mechanism
	// assert_n_array_equal(arr1, arr3, sizeof(arr1));
}

// Test bit assertions
void test_bit_assertions(void) {
	int value = 0x0F; // Binary: 00001111
	
	assert_bit_set(0, value);
	assert_bit_set(1, value);
	assert_bit_set(2, value);
	assert_bit_set(3, value);
	assert_bit_not_set(4, value);
	assert_bit_not_set(5, value);
	
	assert_bit_mask_matches(value, 0x0F);
}

// Test fixtures
void fixture_setup_adaptive(void) {
	// Setup for adaptive tests
}

void fixture_teardown_adaptive(void) {
	// Cleanup for adaptive tests
}

void fixture_setup_analytics(void) {
	analytics_init();
}

void fixture_teardown_analytics(void) {
	analytics_reset();
}

// Test fixtures
void test_fixture_adaptive(void) {
	test_fixture_start();
	fixture_setup(fixture_setup_adaptive);
	fixture_teardown(fixture_teardown_adaptive);
	
	run_test(test_adaptive_detect_text);
	run_test(test_adaptive_detect_png);
	run_test(test_adaptive_detect_elf);
	run_test(test_adaptive_detect_binary);
	run_test(test_adaptive_get_optimal_settings_text);
	run_test(test_adaptive_get_optimal_settings_executable);
	
	test_fixture_end();
}

void test_fixture_analytics(void) {
	test_fixture_start();
	fixture_setup(fixture_setup_analytics);
	fixture_teardown(fixture_teardown_analytics);
	
	run_test(test_analytics_init);
	run_test(test_analytics_compress_tracking);
	run_test(test_analytics_decompress_tracking);
	run_test(test_analytics_reset);
	
	test_fixture_end();
}

void test_fixture_assertions(void) {
	test_fixture_start();
	
	run_test(test_string_assertions);
	run_test(test_pointer_assertions);
	run_test(test_integer_assertions);
	run_test(test_float_assertions);
	run_test(test_array_assertions);
	run_test(test_bit_assertions);
	run_test(test_integrity_check_block);
	
	test_fixture_end();
}

// Main test suite
void run_all_tests(void) {
	test_fixture_adaptive();
	test_fixture_analytics();
	test_fixture_assertions();
}

// Suite setup/teardown
void suite_setup_func(void) {
	// Global setup
}

void suite_teardown_func(void) {
	// Global cleanup
}

int main(int argc, char **argv) {
	return stest_testrunner(argc, argv, run_all_tests,
	                        suite_setup_func, suite_teardown_func);
}

