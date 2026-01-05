// SPDX-License-Identifier: 0BSD
//
// Adaptive Compression - Auto-detect file type and optimize compression settings
// New feature for XZ Utils

#ifndef XZ_ADAPTIVE_H
#define XZ_ADAPTIVE_H

#include "sysdefs.h"
#include "lzma.h"

// File type detection
typedef enum {
	FILE_TYPE_UNKNOWN,
	FILE_TYPE_TEXT,
	FILE_TYPE_BINARY,
	FILE_TYPE_EXECUTABLE,
	FILE_TYPE_IMAGE,
	FILE_TYPE_AUDIO,
	FILE_TYPE_VIDEO,
	FILE_TYPE_ARCHIVE,
	FILE_TYPE_DATABASE
} file_type_t;

// Detect file type from content
file_type_t adaptive_detect_file_type(const uint8_t *data, size_t size);

// Get optimal compression settings for file type
void adaptive_get_optimal_settings(file_type_t type, 
                                    lzma_filter *filters,
                                    uint32_t *preset,
                                    uint64_t *dict_size);

// Analyze file and suggest compression strategy
void adaptive_analyze_file(const char *filename,
                           file_type_t *detected_type,
                           lzma_filter *suggested_filters,
                           uint32_t *suggested_preset);

#endif // XZ_ADAPTIVE_H

