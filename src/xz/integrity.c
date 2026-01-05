// SPDX-License-Identifier: 0BSD
//
// File Integrity Verification Implementation

#include "private.h"
#include "integrity.h"
#include <stdio.h>
#include <string.h>

integrity_result_t integrity_verify_file(const char *filename) {
	FILE *file = fopen(filename, "rb");
	if (file == NULL) {
		return INTEGRITY_ERROR;
	}
	
	// Try to open as .xz stream
	lzma_stream strm = LZMA_STREAM_INIT;
	lzma_ret ret = lzma_stream_decoder(&strm, UINT64_MAX, 0);
	
	if (ret != LZMA_OK) {
		fclose(file);
		lzma_end(&strm);
		return INTEGRITY_ERROR;
	}
	
	uint8_t inbuf[BUFSIZ];
	uint8_t outbuf[BUFSIZ];
	bool has_error = false;
	
	strm.next_in = NULL;
	strm.avail_in = 0;
	strm.next_out = outbuf;
	strm.avail_out = sizeof(outbuf);
	
	while (true) {
		if (strm.avail_in == 0 && !feof(file)) {
			strm.next_in = inbuf;
			strm.avail_in = fread(inbuf, 1, sizeof(inbuf), file);
		}
		
		ret = lzma_code(&strm, feof(file) ? LZMA_FINISH : LZMA_RUN);
		
		if (ret == LZMA_STREAM_END) {
			break;
		} else if (ret != LZMA_OK && ret != LZMA_BUF_ERROR) {
			has_error = true;
			break;
		}
		
		if (strm.avail_out == 0) {
			strm.next_out = outbuf;
			strm.avail_out = sizeof(outbuf);
		}
	}
	
	fclose(file);
	lzma_end(&strm);
	
	if (has_error) {
		return INTEGRITY_CORRUPTED;
	}
	
	return INTEGRITY_OK;
}

bool integrity_repair_file(const char *filename, const char *output_filename) {
	// Basic repair: try to extract what we can
	FILE *infile = fopen(filename, "rb");
	if (infile == NULL) return false;
	
	FILE *outfile = fopen(output_filename, "wb");
	if (outfile == NULL) {
		fclose(infile);
		return false;
	}
	
	lzma_stream strm = LZMA_STREAM_INIT;
	lzma_ret ret = lzma_stream_decoder(&strm, UINT64_MAX, LZMA_CONCATENATED | LZMA_IGNORE_CHECK);
	
	if (ret != LZMA_OK) {
		fclose(infile);
		fclose(outfile);
		lzma_end(&strm);
		return false;
	}
	
	uint8_t inbuf[BUFSIZ];
	uint8_t outbuf[BUFSIZ];
	bool success = false;
	
	strm.next_in = NULL;
	strm.avail_in = 0;
	strm.next_out = outbuf;
	strm.avail_out = sizeof(outbuf);
	
	while (true) {
		if (strm.avail_in == 0 && !feof(infile)) {
			strm.next_in = inbuf;
			strm.avail_in = fread(inbuf, 1, sizeof(inbuf), infile);
		}
		
		ret = lzma_code(&strm, feof(infile) ? LZMA_FINISH : LZMA_RUN);
		
		if (strm.avail_out < sizeof(outbuf)) {
			size_t write_size = sizeof(outbuf) - strm.avail_out;
			if (fwrite(outbuf, 1, write_size, outfile) != write_size) {
				break;
			}
			strm.next_out = outbuf;
			strm.avail_out = sizeof(outbuf);
		}
		
		if (ret == LZMA_STREAM_END) {
			success = true;
			break;
		} else if (ret != LZMA_OK && ret != LZMA_BUF_ERROR) {
			// Try to continue with next stream
			if (ret == LZMA_DATA_ERROR || ret == LZMA_FORMAT_ERROR) {
				lzma_end(&strm);
				lzma_stream_decoder(&strm, UINT64_MAX, LZMA_CONCATENATED | LZMA_IGNORE_CHECK);
				continue;
			}
			break;
		}
	}
	
	fclose(infile);
	fclose(outfile);
	lzma_end(&strm);
	
	return success;
}

bool integrity_check_block(const uint8_t *data, size_t size, lzma_check check_type) {
	// This would need access to liblzma's internal check functions
	// For now, return true as placeholder
	(void)data;
	(void)size;
	(void)check_type;
	return true;
}

void integrity_generate_report(const char *filename, bool json_output) {
	integrity_result_t result = integrity_verify_file(filename);
	
	if (json_output) {
		printf("{\n");
		printf("  \"file\": \"%s\",\n", filename);
		printf("  \"integrity\": \"%s\"\n",
		       result == INTEGRITY_OK ? "ok" :
		       result == INTEGRITY_CORRUPTED ? "corrupted" :
		       result == INTEGRITY_ERROR ? "error" : "unknown");
		printf("}\n");
	} else {
		printf("\n=== Integrity Report ===\n");
		printf("File: %s\n", filename);
		printf("Status: ");
		switch (result) {
		case INTEGRITY_OK:
			printf("OK - File integrity verified\n");
			break;
		case INTEGRITY_CORRUPTED:
			printf("CORRUPTED - File has integrity errors\n");
			break;
		case INTEGRITY_ERROR:
			printf("ERROR - Could not verify file\n");
			break;
		default:
			printf("UNKNOWN\n");
			break;
		}
		printf("=======================\n");
	}
}

