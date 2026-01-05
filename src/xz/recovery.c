// SPDX-License-Identifier: 0BSD
//
// Advanced Error Recovery Implementation

#include "private.h"
#include "recovery.h"
#include "integrity.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static recovery_mode_t current_mode = RECOVERY_PARTIAL;
static recovery_stats_t stats = {0};
static bool recovery_initialized = false;

void recovery_init(void) {
	if (recovery_initialized) return;
	recovery_reset_stats();
	recovery_initialized = true;
}

void recovery_set_mode(recovery_mode_t mode) {
	current_mode = mode;
}

bool recovery_attempt(const uint8_t *corrupted_data, size_t corrupted_size,
                     uint8_t *recovered_data, size_t *recovered_size,
                     recovery_mode_t mode) {
	if (corrupted_data == NULL || recovered_data == NULL || recovered_size == NULL) {
		return false;
	}
	
	if (*recovered_size < corrupted_size) {
		return false;
	}
	
	recovery_mode_t use_mode = mode;
	if (use_mode == RECOVERY_NONE) {
		use_mode = current_mode;
	}
	
	stats.corrupted_blocks++;
	
	// Try to decompress with error tolerance
	lzma_stream strm = LZMA_STREAM_INIT;
	lzma_ret ret = lzma_stream_decoder(&strm, UINT64_MAX,
	                                  LZMA_CONCATENATED | LZMA_IGNORE_CHECK);
	
	if (ret != LZMA_OK) {
		lzma_end(&strm);
		return false;
	}
	
	strm.next_in = corrupted_data;
	strm.avail_in = corrupted_size;
	strm.next_out = recovered_data;
	strm.avail_out = *recovered_size;
	
	ret = lzma_code(&strm, LZMA_FINISH);
	
	size_t recovered = *recovered_size - strm.avail_out;
	*recovered_size = recovered;
	
	lzma_end(&strm);
	
	if (ret == LZMA_STREAM_END || ret == LZMA_OK) {
		stats.recovered_blocks++;
		stats.recovered_bytes += recovered;
		return true;
	} else if (use_mode >= RECOVERY_PARTIAL && recovered > 0) {
		// Partial recovery
		stats.recovered_blocks++;
		stats.recovered_bytes += recovered;
		return true;
	}
	
	stats.skipped_blocks++;
	return false;
}

bool recovery_recover_file(const char *corrupted_file,
                          const char *output_file,
                          recovery_mode_t mode) {
	if (corrupted_file == NULL || output_file == NULL) {
		return false;
	}
	
	FILE *infile = fopen(corrupted_file, "rb");
	if (infile == NULL) {
		return false;
	}
	
	FILE *outfile = fopen(output_file, "wb");
	if (outfile == NULL) {
		fclose(infile);
		return false;
	}
	
	uint8_t buffer[64 * 1024];
	uint8_t recovered[64 * 1024];
	size_t total_recovered = 0;
	
	while (!feof(infile)) {
		size_t read = fread(buffer, 1, sizeof(buffer), infile);
		if (read == 0) break;
		
		size_t recovered_size = sizeof(recovered);
		if (recovery_attempt(buffer, read, recovered, &recovered_size, mode)) {
			if (fwrite(recovered, 1, recovered_size, outfile) != recovered_size) {
				fclose(infile);
				fclose(outfile);
				return false;
			}
			total_recovered += recovered_size;
		} else if (mode >= RECOVERY_AGGRESSIVE) {
			// Try to skip corrupted block and continue
			stats.skipped_blocks++;
		}
	}
	
	fclose(infile);
	fclose(outfile);
	
	if (total_recovered > 0) {
		stats.recovery_rate = (double)stats.recovered_blocks / 
		                     (double)(stats.corrupted_blocks + stats.skipped_blocks);
		return true;
	}
	
	return false;
}

recovery_stats_t recovery_get_stats(void) {
	if (stats.corrupted_blocks > 0) {
		stats.recovery_rate = (double)stats.recovered_blocks / 
		                     (double)stats.corrupted_blocks;
	}
	return stats;
}

void recovery_reset_stats(void) {
	memset(&stats, 0, sizeof(stats));
}

bool recovery_validate(const uint8_t *data, size_t size, lzma_check check_type) {
	return integrity_check_block(data, size, check_type);
}

