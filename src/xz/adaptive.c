// SPDX-License-Identifier: 0BSD
//
// Adaptive Compression Implementation

#include "private.h"
#include "adaptive.h"
#include <string.h>
#include <ctype.h>

// Magic number detection
static bool is_png(const uint8_t *data, size_t size) {
	return size >= 8 && data[0] == 0x89 && data[1] == 0x50 &&
	       data[2] == 0x4E && data[3] == 0x47;
}

static bool is_jpeg(const uint8_t *data, size_t size) {
	return size >= 3 && data[0] == 0xFF && data[1] == 0xD8 && data[2] == 0xFF;
}

static bool is_gif(const uint8_t *data, size_t size) {
	return size >= 6 && memcmp(data, "GIF87a", 6) == 0 ||
	       (size >= 6 && memcmp(data, "GIF89a", 6) == 0);
}

static bool is_elf(const uint8_t *data, size_t size) {
	return size >= 4 && memcmp(data, "\x7F""ELF", 4) == 0;
}

static bool is_pe(const uint8_t *data, size_t size) {
	return size >= 2 && data[0] == 0x4D && data[1] == 0x5A; // MZ
}

static bool is_zip(const uint8_t *data, size_t size) {
	return size >= 4 && memcmp(data, "PK\x03\x04", 4) == 0;
}

static bool is_tar(const uint8_t *data, size_t size) {
	return size >= 5 && memcmp(data, "ustar", 5) == 0;
}

static bool is_text(const uint8_t *data, size_t size) {
	if (size == 0) return false;
	
	// Check first 512 bytes for text characteristics
	size_t check_size = size < 512 ? size : 512;
	size_t printable = 0;
	
	for (size_t i = 0; i < check_size; i++) {
		if (isprint(data[i]) || isspace(data[i])) {
			printable++;
		} else if (data[i] == 0) {
			// Null bytes suggest binary
			return false;
		}
	}
	
	// If >90% printable, consider it text
	return (printable * 100 / check_size) > 90;
}

file_type_t adaptive_detect_file_type(const uint8_t *data, size_t size) {
	if (size == 0) return FILE_TYPE_UNKNOWN;
	
	// Check for known binary formats first
	if (is_png(data, size) || is_jpeg(data, size) || is_gif(data, size)) {
		return FILE_TYPE_IMAGE;
	}
	
	if (is_elf(data, size) || is_pe(data, size)) {
		return FILE_TYPE_EXECUTABLE;
	}
	
	if (is_zip(data, size) || is_tar(data, size)) {
		return FILE_TYPE_ARCHIVE;
	}
	
	// Check for text
	if (is_text(data, size)) {
		return FILE_TYPE_TEXT;
	}
	
	// Default to binary
	return FILE_TYPE_BINARY;
}

void adaptive_get_optimal_settings(file_type_t type,
                                   lzma_filter *filters,
                                   uint32_t *preset,
                                   uint64_t *dict_size) {
	// Default settings
	*preset = LZMA_PRESET_DEFAULT;
	*dict_size = 0; // Let liblzma choose
	
	switch (type) {
	case FILE_TYPE_TEXT:
		// Text compresses well, use higher preset
		*preset = 7;
		*dict_size = 8 * 1024 * 1024; // 8 MiB
		filters[0].id = LZMA_FILTER_LZMA2;
		filters[0].options = NULL;
		filters[1].id = LZMA_VLI_UNKNOWN;
		break;
		
	case FILE_TYPE_EXECUTABLE:
		// Use BCJ filter for executables
		*preset = 6;
		*dict_size = 16 * 1024 * 1024; // 16 MiB
		filters[0].id = LZMA_FILTER_X86; // Default to x86, could detect arch
		filters[0].options = NULL;
		filters[1].id = LZMA_FILTER_LZMA2;
		filters[1].options = NULL;
		filters[2].id = LZMA_VLI_UNKNOWN;
		break;
		
	case FILE_TYPE_IMAGE:
		// Images are already compressed, use fast preset
		*preset = 3;
		*dict_size = 4 * 1024 * 1024; // 4 MiB
		filters[0].id = LZMA_FILTER_LZMA2;
		filters[0].options = NULL;
		filters[1].id = LZMA_VLI_UNKNOWN;
		break;
		
	case FILE_TYPE_ARCHIVE:
		// Archives may contain mixed content, use balanced settings
		*preset = 6;
		*dict_size = 8 * 1024 * 1024; // 8 MiB
		filters[0].id = LZMA_FILTER_LZMA2;
		filters[0].options = NULL;
		filters[1].id = LZMA_VLI_UNKNOWN;
		break;
		
	default:
		// Binary or unknown - use default settings
		*preset = LZMA_PRESET_DEFAULT;
		*dict_size = 0;
		filters[0].id = LZMA_FILTER_LZMA2;
		filters[0].options = NULL;
		filters[1].id = LZMA_VLI_UNKNOWN;
		break;
	}
}

void adaptive_analyze_file(const char *filename,
                           file_type_t *detected_type,
                           lzma_filter *suggested_filters,
                           uint32_t *suggested_preset) {
	FILE *file = fopen(filename, "rb");
	if (file == NULL) {
		*detected_type = FILE_TYPE_UNKNOWN;
		return;
	}
	
	uint8_t buffer[512];
	size_t bytes_read = fread(buffer, 1, sizeof(buffer), file);
	fclose(file);
	
	*detected_type = adaptive_detect_file_type(buffer, bytes_read);
	
	uint64_t dict_size;
	adaptive_get_optimal_settings(*detected_type, suggested_filters,
	                              suggested_preset, &dict_size);
}

