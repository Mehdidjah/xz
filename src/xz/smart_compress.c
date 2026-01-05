// SPDX-License-Identifier: 0BSD
//
// Smart Compression Engine Implementation
// Professional-grade compression intelligence and optimization

#include "private.h"
#include "smart_compress.h"
#include "adaptive.h"
#include "optimizer.h"
#include "parallel.h"
#include "analytics.h"
#include "mytime.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

// Engine state
static bool engine_initialized = false;
static uint64_t stats_total_files = 0;
static uint64_t stats_total_bytes = 0;
static double stats_total_ratio = 0.0;
static double stats_total_speed = 0.0;
static uint64_t stats_compression_count = 0;

// Internal helper: Analyze data entropy
static double calculate_entropy(const uint8_t *data, size_t size) {
	if (size == 0) return 0.0;
	
	uint64_t freq[256] = {0};
	
	// Count byte frequencies
	for (size_t i = 0; i < size; i++) {
		freq[data[i]]++;
	}
	
	// Calculate entropy
	double entropy = 0.0;
	for (int i = 0; i < 256; i++) {
		if (freq[i] > 0) {
			double probability = (double)freq[i] / (double)size;
			entropy -= probability * log2(probability);
		}
	}
	
	return entropy;
}

// Internal helper: Estimate compression ratio from entropy
static double estimate_ratio_from_entropy(double entropy) {
	// Higher entropy = less compressible = higher ratio
	// Entropy ranges from 0 to 8 bits per byte
	// Map to compression ratio (0.0 = no compression, 1.0 = perfect compression)
	double ratio = 1.0 - (entropy / 8.0);
	
	// Clamp to reasonable range
	if (ratio < 0.1) ratio = 0.1;
	if (ratio > 0.95) ratio = 0.95;
	
	return ratio;
}

void smart_compress_init(void) {
	if (engine_initialized) return;
	
	// Initialize dependent modules
	adaptive_detect_file_type(NULL, 0); // Trigger initialization
	optimizer_init();
	parallel_init();
	analytics_init();
	
	smart_compress_reset_stats();
	engine_initialized = true;
}

compression_prediction_t smart_compress_predict(const uint8_t *data,
                                               size_t data_size,
                                               smart_strategy_t strategy) {
	compression_prediction_t prediction = {0};
	
	if (data == NULL || data_size == 0) {
		return prediction;
	}
	
	smart_compress_init();
	
	// Sample data if too large (use first 1MB for analysis)
	size_t sample_size = data_size;
	if (sample_size > 1024 * 1024) {
		sample_size = 1024 * 1024;
	}
	
	// Calculate entropy
	double entropy = calculate_entropy(data, sample_size);
	
	// Estimate compression ratio
	double base_ratio = estimate_ratio_from_entropy(entropy);
	
	// Detect file type for better prediction
	file_type_t file_type = adaptive_detect_file_type(data, sample_size);
	
	// Adjust prediction based on file type
	switch (file_type) {
	case FILE_TYPE_TEXT:
		base_ratio *= 0.85; // Text compresses well
		break;
	case FILE_TYPE_EXECUTABLE:
		base_ratio *= 0.70; // Executables compress moderately
		break;
	case FILE_TYPE_IMAGE:
		base_ratio *= 0.95; // Images are already compressed
		break;
	case FILE_TYPE_ARCHIVE:
		base_ratio *= 0.90; // Archives may compress further
		break;
	default:
		break;
	}
	
	// Adjust based on strategy
	switch (strategy) {
	case SMART_STRATEGY_SPEED:
		base_ratio *= 0.90; // Faster = slightly worse ratio
		prediction.recommended_preset = 3;
		break;
	case SMART_STRATEGY_RATIO:
		base_ratio *= 1.05; // Better ratio possible
		if (base_ratio > 0.95) base_ratio = 0.95;
		prediction.recommended_preset = 8;
		break;
	case SMART_STRATEGY_BALANCED:
		prediction.recommended_preset = 6;
		break;
	case SMART_STRATEGY_MEMORY_EFFICIENT:
		base_ratio *= 0.92;
		prediction.recommended_preset = 5;
		break;
	default:
		prediction.recommended_preset = 6;
		break;
	}
	
	prediction.predicted_ratio = base_ratio;
	prediction.estimated_output_size = (uint64_t)(data_size * base_ratio);
	
	// Calculate confidence based on sample size
	if (sample_size >= data_size) {
		prediction.confidence_level = 0.95;
	} else {
		prediction.confidence_level = 0.70 + (0.25 * ((double)sample_size / (double)data_size));
	}
	
	// Estimate time (rough calculation)
	double mb_size = data_size / (1024.0 * 1024.0);
	double speed_mbps = 10.0; // Conservative estimate
	if (prediction.recommended_preset <= 3) speed_mbps = 30.0;
	else if (prediction.recommended_preset <= 6) speed_mbps = 15.0;
	else speed_mbps = 5.0;
	
	prediction.estimated_time_seconds = mb_size / speed_mbps;
	
	// Estimate memory (rough calculation)
	prediction.estimated_memory_mb = (uint64_t)(mb_size * 2.0 * prediction.recommended_preset);
	if (prediction.estimated_memory_mb < 64) prediction.estimated_memory_mb = 64;
	if (prediction.estimated_memory_mb > 1024) prediction.estimated_memory_mb = 1024;
	
	return prediction;
}

compression_prediction_t smart_compress_predict_file(const char *filename,
                                                    smart_strategy_t strategy) {
	compression_prediction_t prediction = {0};
	
	if (filename == NULL) {
		return prediction;
	}
	
	FILE *file = fopen(filename, "rb");
	if (file == NULL) {
		return prediction;
	}
	
	// Get file size first
	fseek(file, 0, SEEK_END);
	long file_size = ftell(file);
	fseek(file, 0, SEEK_SET);
	
	// Read sample (first 1MB)
	uint8_t *buffer = malloc(1024 * 1024);
	if (buffer == NULL) {
		fclose(file);
		return prediction;
	}
	
	size_t bytes_read = fread(buffer, 1, 1024 * 1024, file);
	fclose(file);
	
	if (bytes_read > 0 && file_size > 0) {
		
		if (file_size > 0) {
			// Use prediction with actual file size context
			prediction = smart_compress_predict(buffer, bytes_read, strategy);
			// Adjust output size estimate based on full file size
			if (file_size > bytes_read) {
				double ratio = prediction.predicted_ratio;
				prediction.estimated_output_size = (uint64_t)(file_size * ratio);
			}
		}
	}
	
	free(buffer);
	return prediction;
}

lzma_ret smart_compress_multi_stream(const multi_stream_config_t *config) {
	if (config == NULL || config->input_files == NULL || 
	    config->num_files == 0 || config->output_file == NULL) {
		return LZMA_PROG_ERROR;
	}
	
	smart_compress_init();
	
	FILE *outfile = fopen(config->output_file, "wb");
	if (outfile == NULL) {
		return LZMA_PROG_ERROR;
	}
	
	lzma_ret overall_ret = LZMA_OK;
	
	for (size_t i = 0; i < config->num_files; i++) {
		FILE *infile = fopen(config->input_files[i], "rb");
		if (infile == NULL) {
			overall_ret = LZMA_PROG_ERROR;
			continue;
		}
		
		lzma_stream strm = LZMA_STREAM_INIT;
		lzma_ret ret = lzma_easy_encoder(&strm, 6, LZMA_CHECK_CRC64);
		
		if (ret == LZMA_OK) {
			uint8_t inbuf[64 * 1024];
			uint8_t outbuf[64 * 1024];
			
			strm.next_out = outbuf;
			strm.avail_out = sizeof(outbuf);
			
			while (!feof(infile)) {
				strm.next_in = inbuf;
				strm.avail_in = fread(inbuf, 1, sizeof(inbuf), infile);
				
				if (strm.avail_in == 0) break;
				
				ret = lzma_code(&strm, feof(infile) ? LZMA_FINISH : LZMA_RUN);
				
				if (strm.avail_out == 0 || ret == LZMA_STREAM_END) {
					size_t write_size = sizeof(outbuf) - strm.avail_out;
					if (fwrite(outbuf, 1, write_size, outfile) != write_size) {
						ret = LZMA_PROG_ERROR;
						break;
					}
					strm.next_out = outbuf;
					strm.avail_out = sizeof(outbuf);
				}
				
				if (ret != LZMA_OK && ret != LZMA_STREAM_END) {
					break;
				}
			}
			
			if (ret == LZMA_STREAM_END) {
				// Write remaining output
				size_t write_size = sizeof(outbuf) - strm.avail_out;
				if (write_size > 0) {
					fwrite(outbuf, 1, write_size, outfile);
				}
			}
		}
		
		lzma_end(&strm);
		fclose(infile);
		
		if (ret != LZMA_OK && ret != LZMA_STREAM_END) {
			overall_ret = ret;
		}
	}
	
	fclose(outfile);
	return overall_ret;
}

size_t smart_compress_benchmark(const uint8_t *data,
                               size_t data_size,
                               uint32_t *presets,
                               size_t num_presets,
                               benchmark_result_t *results,
                               smart_strategy_t strategy) {
	if (data == NULL || data_size == 0 || presets == NULL || 
	    num_presets == 0 || results == NULL) {
		return 0;
	}
	
	smart_compress_init();
	
	size_t results_count = 0;
	
	for (size_t i = 0; i < num_presets && i < 10; i++) {
		uint32_t preset = presets[i];
		if (preset > 9) continue;
		
		lzma_stream strm = LZMA_STREAM_INIT;
		lzma_ret ret = lzma_easy_encoder(&strm, preset, LZMA_CHECK_CRC64);
		
		if (ret != LZMA_OK) {
			lzma_end(&strm);
			continue;
		}
		
		// Allocate output buffer (worst case: no compression)
		uint8_t *outbuf = malloc(data_size * 2);
		if (outbuf == NULL) {
			lzma_end(&strm);
			continue;
		}
		
		strm.next_in = data;
		strm.avail_in = data_size;
		strm.next_out = outbuf;
		strm.avail_out = data_size * 2;
		
		uint64_t start_time = mytime_get_elapsed();
		ret = lzma_code(&strm, LZMA_FINISH);
		uint64_t end_time = mytime_get_elapsed();
		
		if (ret == LZMA_STREAM_END) {
			benchmark_result_t *result = &results[results_count];
			result->preset = preset;
			result->output_size_bytes = (data_size * 2) - strm.avail_out;
			result->compression_ratio = (double)result->output_size_bytes / (double)data_size;
			
			double elapsed_sec = (end_time > start_time) ? 
			                    (double)(end_time - start_time) / 1000.0 : 0.001;
			double mb_size = data_size / (1024.0 * 1024.0);
			
			result->compression_time_sec = elapsed_sec;
			result->compression_speed_mbps = (elapsed_sec > 0) ? mb_size / elapsed_sec : 0.0;
			
			// Estimate decompression speed (typically faster)
			result->decompression_speed_mbps = result->compression_speed_mbps * 3.0;
			result->decompression_time_sec = elapsed_sec / 3.0;
			
			// Estimate memory usage
			result->memory_used_mb = (uint64_t)(mb_size * 2.0 * preset);
			if (result->memory_used_mb < 64) result->memory_used_mb = 64;
			
			results_count++;
			
			// Update global stats
			stats_total_bytes += data_size;
			stats_total_ratio += result->compression_ratio;
			stats_total_speed += result->compression_speed_mbps;
			stats_compression_count++;
		}
		
		free(outbuf);
		lzma_end(&strm);
	}
	
	return results_count;
}

quality_score_t smart_compress_calculate_quality(const benchmark_result_t *result,
                                                 smart_strategy_t strategy) {
	quality_score_t score = {0};
	
	if (result == NULL) {
		return score;
	}
	
	// Compression ratio score (0-100)
	// Better ratio = higher score
	double ratio_score = (1.0 - result->compression_ratio) * 100.0;
	if (ratio_score < 0) ratio_score = 0;
	if (ratio_score > 100) ratio_score = 100;
	score.compression_ratio_score = ratio_score;
	
	// Speed score (0-100)
	// Normalize speed (assume 50 MB/s is excellent)
	double speed_score = (result->compression_speed_mbps / 50.0) * 100.0;
	if (speed_score > 100) speed_score = 100;
	score.speed_score = speed_score;
	
	// Memory efficiency score (0-100)
	// Lower memory = higher score (normalize: 500 MB = 50 points)
	double memory_score = 100.0 - ((double)result->memory_used_mb / 500.0) * 50.0;
	if (memory_score < 0) memory_score = 0;
	if (memory_score > 100) memory_score = 100;
	score.memory_efficiency_score = memory_score;
	
	// Overall score based on strategy
	switch (strategy) {
	case SMART_STRATEGY_SPEED:
		score.overall_score = (speed_score * 0.6) + (ratio_score * 0.2) + (memory_score * 0.2);
		break;
	case SMART_STRATEGY_RATIO:
		score.overall_score = (ratio_score * 0.6) + (speed_score * 0.2) + (memory_score * 0.2);
		break;
	case SMART_STRATEGY_MEMORY_EFFICIENT:
		score.overall_score = (memory_score * 0.5) + (ratio_score * 0.3) + (speed_score * 0.2);
		break;
	default: // BALANCED
		score.overall_score = (ratio_score * 0.4) + (speed_score * 0.4) + (memory_score * 0.2);
		break;
	}
	
	// Assign letter grade
	if (score.overall_score >= 95) strcpy(score.quality_grade, "A+");
	else if (score.overall_score >= 90) strcpy(score.quality_grade, "A");
	else if (score.overall_score >= 85) strcpy(score.quality_grade, "B+");
	else if (score.overall_score >= 80) strcpy(score.quality_grade, "B");
	else if (score.overall_score >= 75) strcpy(score.quality_grade, "C+");
	else if (score.overall_score >= 70) strcpy(score.quality_grade, "C");
	else if (score.overall_score >= 60) strcpy(score.quality_grade, "D");
	else strcpy(score.quality_grade, "F");
	
	return score;
}

void smart_compress_get_recommendations(const char *filename,
                                       smart_strategy_t strategy,
                                       uint32_t *preset,
                                       lzma_filter *filters,
                                       uint64_t *dict_size,
                                       uint32_t *threads) {
	if (filename == NULL) return;
	
	smart_compress_init();
	
	// Get file type
	FILE *file = fopen(filename, "rb");
	if (file == NULL) return;
	
	uint8_t buffer[512];
	size_t bytes_read = fread(buffer, 1, sizeof(buffer), file);
	fclose(file);
	
	file_type_t file_type = adaptive_detect_file_type(buffer, bytes_read);
	
	// Get optimal settings
	adaptive_get_optimal_settings(file_type, filters, preset, dict_size);
	
	// Adjust based on strategy
	switch (strategy) {
	case SMART_STRATEGY_SPEED:
		if (*preset > 4) *preset = 4;
		break;
	case SMART_STRATEGY_RATIO:
		if (*preset < 7) *preset = 7;
		break;
	case SMART_STRATEGY_MEMORY_EFFICIENT:
		if (*preset > 5) *preset = 5;
		if (*dict_size > 8 * 1024 * 1024) *dict_size = 8 * 1024 * 1024;
		break;
	default:
		break;
	}
	
	// Get optimal threads
	if (threads) {
		// Get file size
		file = fopen(filename, "rb");
		if (file) {
			fseek(file, 0, SEEK_END);
			long file_size = ftell(file);
			fclose(file);
			
			if (file_size > 0) {
				*threads = parallel_get_optimal_threads(file_size, 1024 * 1024 * 1024);
			} else {
				*threads = 1;
			}
		} else {
			*threads = 1;
		}
	}
}

lzma_ret smart_compress_file(const char *input_file,
                             const char *output_file,
                             smart_strategy_t strategy,
                             quality_score_t *quality_out) {
	if (input_file == NULL || output_file == NULL) {
		return LZMA_PROG_ERROR;
	}
	
	smart_compress_init();
	
	// Get recommendations
	uint32_t preset;
	lzma_filter filters[LZMA_FILTERS_MAX + 1];
	uint64_t dict_size;
	uint32_t threads;
	
	smart_compress_get_recommendations(input_file, strategy, &preset, filters,
	                                  &dict_size, &threads);
	
	// Read input file
	FILE *infile = fopen(input_file, "rb");
	if (infile == NULL) {
		return LZMA_PROG_ERROR;
	}
	
	fseek(infile, 0, SEEK_END);
	long file_size = ftell(infile);
	fseek(infile, 0, SEEK_SET);
	
	if (file_size <= 0) {
		fclose(infile);
		return LZMA_PROG_ERROR;
	}
	
	uint8_t *inbuf = malloc(file_size);
	if (inbuf == NULL) {
		fclose(infile);
		return LZMA_MEM_ERROR;
	}
	
	fread(inbuf, 1, file_size, infile);
	fclose(infile);
	
	// Compress
	lzma_stream strm = LZMA_STREAM_INIT;
	lzma_ret ret = lzma_easy_encoder(&strm, preset, LZMA_CHECK_CRC64);
	
	if (ret == LZMA_OK) {
		uint8_t *outbuf = malloc(file_size * 2);
		if (outbuf != NULL) {
			strm.next_in = inbuf;
			strm.avail_in = file_size;
			strm.next_out = outbuf;
			strm.avail_out = file_size * 2;
			
			ret = lzma_code(&strm, LZMA_FINISH);
			
			if (ret == LZMA_STREAM_END) {
				size_t out_size = (file_size * 2) - strm.avail_out;
				
				// Write output
				FILE *outfile = fopen(output_file, "wb");
				if (outfile != NULL) {
					fwrite(outbuf, 1, out_size, outfile);
					fclose(outfile);
					
					// Calculate quality if requested
					if (quality_out != NULL) {
						benchmark_result_t result = {
							.preset = preset,
							.compression_ratio = (double)out_size / (double)file_size,
							.output_size_bytes = out_size
						};
						*quality_out = smart_compress_calculate_quality(&result, strategy);
					}
					
					stats_total_files++;
				} else {
					ret = LZMA_PROG_ERROR;
				}
			}
			
			free(outbuf);
		} else {
			ret = LZMA_MEM_ERROR;
		}
	}
	
	free(inbuf);
	lzma_end(&strm);
	
	return ret;
}

void smart_compress_get_stats(uint64_t *total_files_processed,
                              uint64_t *total_bytes_compressed,
                              double *average_ratio,
                              double *average_speed_mbps) {
	if (total_files_processed) *total_files_processed = stats_total_files;
	if (total_bytes_compressed) *total_bytes_compressed = stats_total_bytes;
	
	if (average_ratio && stats_compression_count > 0) {
		*average_ratio = stats_total_ratio / stats_compression_count;
	}
	
	if (average_speed_mbps && stats_compression_count > 0) {
		*average_speed_mbps = stats_total_speed / stats_compression_count;
	}
}

void smart_compress_reset_stats(void) {
	stats_total_files = 0;
	stats_total_bytes = 0;
	stats_total_ratio = 0.0;
	stats_total_speed = 0.0;
	stats_compression_count = 0;
}

void smart_compress_cleanup(void) {
	smart_compress_reset_stats();
	engine_initialized = false;
}

