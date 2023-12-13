// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2021 Google LLC
 */

#ifndef _CHROMECAST_PARTITION_H_
#define _CHROMECAST_PARTITION_H_

#include <blk.h>

#define CHROMECAST_PARTITION_ENTRY_NUMBERS CONFIG_EFI_PARTITION_ENTRIES_NUMBERS

typedef enum {
	CHROMECAST_PARTITION_MIGRATE_OP_NONE = 0,
	CHROMECAST_PARTITION_MIGRATE_OP_ERASE,
} chromecast_partition_migrate_op_t;

typedef struct {
	const char *name;
	uint64_t start;
	uint64_t size;
	chromecast_partition_migrate_op_t migrate_op;
} chromecast_partition_t;

typedef struct {
	// Make sure that 'align' is a multiple of the device block size.
	// For eMMC, make sure that it's also a multiple of the erase group
	// size.
	uint64_t align;
	uint64_t reserved;
	chromecast_partition_t partitions[CHROMECAST_PARTITION_ENTRY_NUMBERS];
} chromecast_partition_table_t;

int init_chromecast_partitions(
		struct blk_desc *dev_desc,
		const chromecast_partition_table_t *partition_table);

#endif  // _CHROMECAST_PARTITION_H_
