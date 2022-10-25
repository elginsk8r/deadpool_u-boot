
/*
 * cmd/amlogic/cmd_misc.c
 *
 * Copyright (C) 2018 Amlogic, Inc. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <amlogic/flash_ts.h>
#include <amlogic/led_aw2015.h>

int do_charger_detect_boot(cmd_tbl_t *cmdtp, int flag, int argc,
		char * const argv[])
{
#if defined(CONFIG_ENABLE_CHARGER_DETECTION)
	char *s;
	const char *key_name = "boot.enforce_charger_detection";
	char key_value[256] = { 0 };

	s = env_get("reboot_mode");
	if (!strncmp(s, "factory_boot", 12)) {
		pr_info("factory boot, skip charger detection\n");
		return 0;
	}

	if(is_flash_inited()) {
		flash_ts_init();

		flash_ts_get(key_name, key_value, sizeof(key_value));
		pr_info("FTS read: boot.enforce_charger_detection -> %s\n", key_value);
	}

	if (strncmp(key_value, "true", 4)) {
		s = env_get("charger_type");
		if (!strncmp(s, "DCP", 3)) {
			sys_led_init(true);
		} else {
			sys_led_init(false);
		}
		return 0;
	}

	do {
		s = env_get("charger_type");
		if (!strncmp(s, "DCP", 3)) {
			pr_info("charger-DCP: Booting...\n");
			sys_led_init(true);
			break;
		} else {
			sys_led_init(false);
			pr_info("Stop booting kernel due to charger type is incompatible\n");
			pr_info("Expected: DCP, Actual: %s\n", s);
			pr_info("Register dump:\n");
			pr_info("[0xfe0044a4] = 0x%x\n", readl(0xfe0044a4));
			pr_info("Retrying...\n");
			mdelay(1000);
			run_command("musb bc", 0);
		}
	} while(1);
#endif
	return 0;
}

U_BOOT_CMD(
	charger_detect_boot, 1, 0, do_charger_detect_boot,
	"only allow booting when charger is detected as DCP\n",
	"charger_detect_boot"
);

int charger_detection_config (char * const value)
{
	int ret = 1;

	if(is_flash_inited()) {
		flash_ts_init();

		char key[] = "boot.enforce_charger_detection";
		ret = flash_ts_set(key, value);

		printf("FTS set:\n%s -> %s\nReturn: %d\n", key, value, ret);
	}
	return ret;
}

int do_enable_charger_detection (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	char value[] = "true";
	return charger_detection_config(value);
}

int do_disable_charger_detection (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	char value[] = "false";
	return charger_detection_config(value);
}

int do_clear_charger_detection_flag (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	char value[] = "";
	return charger_detection_config(value);
}

U_BOOT_CMD(
  enforce_charger_detection, 1,  0,  do_enable_charger_detection,
  "Set FTS flag to force chareger detection.",
  "enforce_charger_detection\n"
);

U_BOOT_CMD(
  disable_charger_detection, 1,  0,  do_disable_charger_detection,
  "set FTS flag to disable chareger detection.",
  "disable_charger_detection\n"
);

U_BOOT_CMD(
  clear_charger_detection_flag, 1,  0,  do_clear_charger_detection_flag,
  "clear chareger detection key.",
  "clear_charger_detection_flag\n"
);
