/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2021 Google LLC
 */

#ifndef _BOARD_AMLOGIC_S4_T211_PARTITION_TABLE_H_
#define _BOARD_AMLOGIC_S4_T211_PARTITION_TABLE_H_

#include <chromecast/partition.h>
#include <linux/sizes.h>

#define CHROMECAST_BOOTLOADER_SIZE (3 * SZ_1M + 512 * SZ_1K)

// go/boreal-partition
const chromecast_partition_table_t boreal_partition_table = {
	.align = SZ_1M,
	.reserved = SZ_1M,  // reserved space for second GPT
	.partitions = {
		{
			.name = "reserved",
			.start = 36 * SZ_1M,
			.size = 64 * SZ_1M,
		},
		{
			.name = "env",
			.size = 8 * SZ_1M,
			.migrate_op = CHROMECAST_PARTITION_MIGRATE_OP_ERASE,
		},
		{
			.name = "frp",
			.size = 1 * SZ_1M,
			.migrate_op = CHROMECAST_PARTITION_MIGRATE_OP_ERASE,
		},
		{
			.name = "misc",
			.size = 1 * SZ_1M,
			.migrate_op = CHROMECAST_PARTITION_MIGRATE_OP_ERASE,
		},
		{
			.name = "tee",
			.size = 16 * SZ_1M,
			.migrate_op = CHROMECAST_PARTITION_MIGRATE_OP_ERASE,
		},
		{
			.name = "cri_data",
			.size = 8 * SZ_1M,
			.migrate_op = CHROMECAST_PARTITION_MIGRATE_OP_ERASE,
		},
		{
			.name = "param",
			.size = 8 * SZ_1M,
			.migrate_op = CHROMECAST_PARTITION_MIGRATE_OP_ERASE,
		},
		{
			.name = "factory",
			.size = 8 * SZ_1M,
		},
		{
			.name = "bootloader_a",
			.size = CHROMECAST_BOOTLOADER_SIZE,
		},
		{
			.name = "bootloader_b",
			.size = CHROMECAST_BOOTLOADER_SIZE,
		},
		{
			.name = "boot_a",
			.size = 64 * SZ_1M,
		},
		{
			.name = "boot_b",
			.size = 64 * SZ_1M,
		},
		{
			.name = "vendor_boot_a",
			.size = 64 * SZ_1M,
		},
		{
			.name = "vendor_boot_b",
			.size = 64 * SZ_1M,
		},
		{
			.name = "dtbo_a",
			.size = 2 * SZ_1M,
		},
		{
			.name = "dtbo_b",
			.size = 2 * SZ_1M,
		},
		{
			.name = "metadata",
			.size = 16 * SZ_1M,
			.migrate_op = CHROMECAST_PARTITION_MIGRATE_OP_ERASE,
		},
		{
			.name = "vbmeta_a",
			.size = 1 * SZ_1M,
		},
		{
			.name = "vbmeta_b",
			.size = 1 * SZ_1M,
		},
		{
			.name = "vbmeta_system_a",
			.size = 1 * SZ_1M,
		},
		{
			.name = "vbmeta_system_b",
			.size = 1 * SZ_1M,
		},
		{
			.name = "super",
			.size = 2560ULL * SZ_1M,
		},
		{
			.name = "userdata",
			.size = 0,  // use the rest of the spaces
			.migrate_op = CHROMECAST_PARTITION_MIGRATE_OP_ERASE,
		},
	},
};

#endif  // _BOARD_AMLOGIC_S4_T211_PARTITION_TABLE_H_
