// SPDX-License-Identifier: 0BSD
//
// File Integrity Verification and Repair
// New feature for XZ Utils

#ifndef XZ_INTEGRITY_H
#define XZ_INTEGRITY_H

#include "sysdefs.h"
#include "lzma.h"
#include <stdbool.h>

// Verify file integrity
typedef enum {
	INTEGRITY_OK,
	INTEGRITY_ERROR,
	INTEGRITY_CORRUPTED,
	INTEGRITY_UNKNOWN
} integrity_result_t;

// Verify .xz file integrity
integrity_result_t integrity_verify_file(const char *filename);

// Attempt to repair corrupted file (if possible)
bool integrity_repair_file(const char *filename, const char *output_filename);

// Check block integrity
bool integrity_check_block(const uint8_t *data, size_t size, lzma_check check_type);

// Generate integrity report
void integrity_generate_report(const char *filename, bool json_output);

#endif // XZ_INTEGRITY_H

