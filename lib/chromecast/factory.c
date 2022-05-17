/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2021 Google LLC
 */

#include <blk.h>
#include <chromecast/factory.h>
#include <common.h>
#include <linux/ctype.h>
#include <part.h>
#include <stdio.h>
#include <string.h>
#include <fs.h>

// same as PROPERTY_VALUE_MAX in Android
#define CHROMECAST_FACTORY_CONFIG_VALUE_MAX 92

static const struct {
	const char *sku;
	const char *region_code;
	const char *locales;
} SKU_LIST[] = {
	// default to use the first one
	{ "US", "US", "en-US,es-US" },
	{ "CA", "CA", "en-CA,fr-CA" },
	{ "GB", "EU", "en-GB,en-IE" },
	{ "DE", "EU", "de-DE" },
	{ "FR", "EU", "fr-FR" },
	{ "NL", "EU", "nl-NL,de-BE,fr-BE,nl-BE,de-AT" },
	{ "NO", "EU", "nb-NO,fi-FI,sv-SE,da-DK,en-FI,en-GB" },
	{ "IT", "EU", "it-IT,es-ES,fr-CH,de-CH,it-CH" },
	{ "AU", "AU", "en-AU,en-NZ" },
	{ "JP", "JP", "ja-JP" },
	{ "KR", "KR", "ko-KR" },
	{ "IN", "US", "en-IN,hi-IN" },
	{ "LA", "MX", "es-MX,es-PE,es-CO,es-GT,es-HN,es-SV,es-NI,es-CR,es-PA,es-EC" },
	{ "TW", "US", "zh-TW" },
	{ "BR", "BR", "pt-BR" },
};

static loff_t get_factory_config(struct blk_desc *dev_desc, int part,
				 const char *name, char *buf,
				 const char *default_value)
{
	int ret;
	loff_t read_size;
	char config_path[128];

	snprintf(config_path, sizeof(config_path), "/configs/%s", name);

	// NOTE: fs_read closes the device, so we need to reset the device
	//	 for every fs_read.
	ret = fs_set_blk_dev_with_part(dev_desc, part);
	if (ret == 0) {
		ret = fs_read(config_path, (ulong) buf, 0,
			      CHROMECAST_FACTORY_CONFIG_VALUE_MAX - 1,
			      &read_size);
	}
	if (ret) {
		strlcpy(buf, default_value,
			CHROMECAST_FACTORY_CONFIG_VALUE_MAX);
		read_size = strlen(buf);
	}

	// There could be end of lines behind the value, remove them.
	while (read_size > 0 && isspace(buf[read_size - 1])) {
		read_size--;
	}
	buf[read_size] = '\0';

	return read_size;
}

int load_chromecast_factory_configs(struct blk_desc *dev_desc)
{
	disk_partition_t info;
	int part;
	int i;
	char buf[CHROMECAST_FACTORY_CONFIG_VALUE_MAX];
	int sku_id;

	part = part_get_info_by_name(dev_desc, "factory", &info);
	if (part == -1) {
		printf("%s: factory partition not found\n", __func__);
		return 1;
	}

	get_factory_config(dev_desc, part, "serial", buf,
			   "1234567890");
	env_set("serial#", buf);

	// Set the default value to Google owned local mac address.
	get_factory_config(dev_desc, part, "mac_wifi", buf,
			   "02:1A:11:00:00:00");
	env_set("mac_wifi", buf);

	// Set the default value to Google owned local mac address.
	get_factory_config(dev_desc, part, "mac_bt", buf,
			   "02:1A:11:00:00:11");
	env_set("mac_bt", buf);

	get_factory_config(dev_desc, part, "sku", buf,
			   SKU_LIST[0].sku);
	sku_id = -1;
	for (i = 0; i < sizeof(SKU_LIST) / sizeof(SKU_LIST[0]); i++) {
		if (strcmp(buf, SKU_LIST[i].sku) == 0) {
			sku_id = i;
			break;
		}
	}
	if (sku_id == -1) {
		printf("unknown sku: %s, use the default: %s", buf,
		       SKU_LIST[0].sku);
		strlcpy(buf, SKU_LIST[0].sku, sizeof(buf));
		sku_id = 0;
	}
	env_set("sku", buf);

	// Allow overwriting the default region_code and locales by factory
	// config for testing.
	get_factory_config(dev_desc, part, "region_code", buf,
			   SKU_LIST[sku_id].region_code);
	env_set("region_code", buf);

	get_factory_config(dev_desc, part, "locales", buf,
			   SKU_LIST[sku_id].locales);
	env_set("locales", buf);

	return 0;
}
