// SPDX-License-Identifier: 0BSD
//
// Advanced Error Recovery - Robust error handling and data recovery
// Complex feature for XZ Utils Enhanced Edition

#ifndef XZ_RECOVERY_H
#define XZ_RECOVERY_H

#include "sysdefs.h"
#include "lzma.h"
#include <stdbool.h>
#include <stdint.h>

// Recovery mode
typedef enum {
	RECOVERY_NONE,           // No recovery
	RECOVERY_PARTIAL,        // Partial recovery (skip corrupted blocks)
	RECOVERY_AGGRESSIVE,     // Aggressive recovery (try to repair)
	RECOVERY_MAXIMUM         // Maximum recovery (all possible methods)
} recovery_mode_t;

// Recovery statistics
typedef struct {
	uint32_t corrupted_blocks;
	uint32_t recovered_blocks;
	uint32_t skipped_blocks;
	uint64_t recovered_bytes;
	double recovery_rate;
} recovery_stats_t;

// Initialize recovery system
void recovery_init(void);

// Set recovery mode
void recovery_set_mode(recovery_mode_t mode);

// Attempt to recover corrupted data
bool recovery_attempt(const uint8_t *corrupted_data, size_t corrupted_size,
                    uint8_t *recovered_data, size_t *recovered_size,
                    recovery_mode_t mode);

// Recover file with multiple strategies
bool recovery_recover_file(const char *corrupted_file,
                          const char *output_file,
                          recovery_mode_t mode);

// Get recovery statistics
recovery_stats_t recovery_get_stats(void);

// Reset recovery statistics
void recovery_reset_stats(void);

// Validate recovered data integrity
bool recovery_validate(const uint8_t *data, size_t size, lzma_check check_type);

#endif // XZ_RECOVERY_H

