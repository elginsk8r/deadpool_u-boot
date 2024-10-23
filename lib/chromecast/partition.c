/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2021 Google LLC
 */

#include <blk.h>
#include <chromecast/partition.h>
#include <linux/sizes.h>
#include <linux/types.h>
#include <malloc.h>
#include <memalign.h>
#include <part.h>
#include <part_efi.h>
#include <stdio.h>
#include <uuid.h>

static int create_partitions_from_table(
		struct blk_desc *dev_desc,
		const chromecast_partition_table_t *partition_table,
		disk_partition_t **partitions,
		int *part_count)
{
	const uint64_t align = partition_table->align;
	const uint64_t reserved = partition_table->reserved;
	const chromecast_partition_t *cc_parts = partition_table->partitions;
	const uint64_t blksz = dev_desc->blksz;
	const lbaint_t total_blk = dev_desc->lba;
	lbaint_t offset_blk;
	lbaint_t align_blk;
	lbaint_t reserved_blk;
	disk_partition_t *parts;
	int count = 0;
	int i;

	if (align % blksz) {
		printf("%s: align (%lld) is not a multiple of blksz (%lld)\n",
		       __func__, align, blksz);
		return -EINVAL;
	}

	if (reserved % blksz) {
		printf("%s: reserved (%lld) is not a multiple of blksz (%lld)\n",
		       __func__, reserved, blksz);
		return -EINVAL;
	}

	for (i = 0; i < CHROMECAST_PARTITION_ENTRY_NUMBERS &&
			cc_parts[i].name; i++) {
		if (cc_parts[i].start % align) {
			printf("%s: partition %s not aligned\n",
			       __func__, cc_parts[i].name);
			return -EINVAL;
		}
		count++;
	}

	offset_blk = 0;
	align_blk = align ? align / blksz : 1;
	reserved_blk = reserved / blksz;

	parts = calloc(count, sizeof(disk_partition_t));
	if (!parts)
		return -ENOMEM;

	for (i = 0; i < count; i++) {
		lbaint_t start_blk = cc_parts[i].start / blksz;
		lbaint_t size_blk = cc_parts[i].size / blksz;

		if (start_blk == 0) {
			start_blk = offset_blk;
		} else if (start_blk < offset_blk) {
			printf("%s: %s partition overlaps with another one\n",
			       __func__, cc_parts[i].name);
			free(parts);
			return -EINVAL;
		}

		if (start_blk + size_blk + reserved_blk > total_blk) {
			printf("%s: %s partition exceeds disk size\n",
			       __func__, cc_parts[i].name);
			free(parts);
			return -EINVAL;
		}

		// extend the last partition to the end if the size is 0
		if (size_blk == 0 && i == count - 1) {
			size_blk = total_blk - reserved_blk - start_blk;
			// align down the size
			size_blk -= size_blk % align_blk;
		}

		offset_blk = start_blk + size_blk;
		// align up the offset
		offset_blk += align_blk - 1;
		offset_blk -= offset_blk % align_blk;

		parts[i].start = start_blk;
		parts[i].size = size_blk;
		strncpy((char *)parts[i].name, cc_parts[i].name, PART_NAME_LEN);
		parts[i].name[PART_NAME_LEN - 1] = '\0';
		printf("%s: %3d: %16s  start: %8ld  size: %8ld\n", __func__,
		       i + 1, parts[i].name, parts[i].start, parts[i].size);
#if CONFIG_IS_ENABLED(PARTITION_UUIDS)
		gen_rand_uuid_str(parts[i].uuid, UUID_STR_FORMAT_GUID);
#endif
	}

	*partitions = parts;
	*part_count = count;

	return 0;
}

static int verify_partitions(struct blk_desc *dev_desc,
			     disk_partition_t *partitions,
			     int part_count)
{
	ALLOC_CACHE_ALIGN_BUFFER_PAD(gpt_header, gpt_head, 1, dev_desc->blksz);
	gpt_entry *gpt_pte = NULL;
	int ret;

	ret = gpt_verify_partitions(dev_desc, partitions, part_count,
				    gpt_head, &gpt_pte);

	free(gpt_pte);
	return ret;
}

static int restore_partitions(struct blk_desc *dev_desc,
			      disk_partition_t *partitions,
			      int part_count)
{
	char disk_guid[UUID_STR_LEN + 1];
	int ret;

	gen_rand_uuid_str(disk_guid, UUID_STR_FORMAT_GUID);
	ret = gpt_restore(dev_desc, disk_guid, partitions, part_count);

	return ret;
}

static int migrate_partitions(
		struct blk_desc *dev_desc,
		const disk_partition_t *partitions,
		int part_count,
		const chromecast_partition_table_t *partition_table)
{
	int i;

	for (i = 0; i < part_count; i++) {
		switch (partition_table->partitions[i].migrate_op) {
		case CHROMECAST_PARTITION_MIGRATE_OP_ERASE:
			printf("%s: erasing %s\n",
			       __func__, partitions[i].name);
			if (blk_derase(dev_desc, partitions[i].start,
				       partitions[i].size) != 0)
				return 1;
			break;
		case CHROMECAST_PARTITION_MIGRATE_OP_NONE:
		default:
			break;
		}
	}

	return 0;
}

int init_chromecast_partitions(
		struct blk_desc *dev_desc,
		const chromecast_partition_table_t *partition_table)
{
	disk_partition_t *partitions = NULL;
	int part_count = 0;
	int ret;

	ret = create_partitions_from_table(dev_desc, partition_table,
					   &partitions, &part_count);
	if (ret) {
		printf("%s: failed to get partitions\n", __func__);
		goto out;
	}

	ret = verify_partitions(dev_desc, partitions, part_count);
	if (ret == 0) {
		printf("%s: successfully verified partitions\n", __func__);
		goto out;
	}

	printf("%s: updating partitions\n", __func__);

	ret = migrate_partitions(dev_desc, partitions, part_count,
				 partition_table);
	if (ret) {
		printf("%s: failed to migrate partitions\n", __func__);
		goto out;
	}

	ret = restore_partitions(dev_desc, partitions, part_count);
	if (ret) {
		printf("%s: failed to update partitions\n", __func__);
		goto out;
	}

	part_init(dev_desc);
	printf("%s: partitions updated\n", __func__);
out:
	free(partitions);
	return ret;
}
