
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
#include <dm.h>

struct vol_table {
	unsigned int val;
	char voltage[5];
};

struct vol_table vddee_table[] = {
	{0xb, "0.71V"},
	{0xc, "0.72V"},
	{0xd, "0.73V"},
	{0xe, "0.74V"},
	{0xf, "0.75V"},
	{0x10, "0.76V"},
	{0x11, "0.77V"},
	{0x12, "0.78V"},
	{0x13, "0.79V"},
	{0x14, "0.80V"},
	{0x15, "0.81V"},
	{0x16, "0.82V"},
	{0x17, "0.83V"},
	{0x18, "0.84V"},
	{0x19, "0.85V"},
	{0x1a, "0.86V"},
	{0x1b, "0.87V"},
	{0x1c, "0.88V"},
	{0x1d, "0.89V"},
};

int do_charger_detect_boot(cmd_tbl_t *cmdtp, int flag, int argc,
		char * const argv[])
{
#if defined(CONFIG_ENABLE_CHARGER_DETECTION)
	char *s;
	const char *key_name = "boot.enforce_charger_detection";
	char key_value[256] = { 0 };
	int i = 10;

	s = env_get("reboot_mode");
	if (strncmp(s, "cold_boot", 9)) {
		pr_crit("Skip charger enforcement since it's not cold boot\n");
		return 0;
	}

	if(is_flash_inited()) {
		flash_ts_init();

		flash_ts_get(key_name, key_value, sizeof(key_value));
		pr_info("FTS read: boot.enforce_charger_detection -> %s\n", key_value);
	}

	s = env_get("charger_type");
	if (!strncmp(s, "DCP", 3)) {
		pr_info("charger-DCP: Booting...\n");
		sys_led_init(WHITE);
		return 0;
	}

	if (strncmp(key_value, "true", 4)) {
		sys_led_init(YELLOW);
		return 0;
	}

	sys_led_init(BLINK_YELLOW);
	do {
		s = env_get("charger_type");
		if (!strncmp(s, "DCP", 3)) {
			pr_info("charger-DCP: Booting...\n");
			sys_led_init(WHITE);
			break;
		} else {
			pr_crit("Stop booting kernel due to charger type is incompatible\n");
			pr_crit("Expected: DCP, Actual: %s\n", s);
			pr_crit("Register dump:\n");
			pr_crit("[0xfe0044a4] = 0x%x\n", readl(0xfe0044a4));
			mdelay(1000);
			i--;
			if (i > 0) {
				pr_crit("Retrying...\n");
				run_command("musb bc", 0);
			}
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

int do_get_vddee_voltage(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int ret = 0;
	unsigned int bus_num = 4;
	unsigned int dev_addr = 0x6a;
	unsigned int reg_addr = 0x0;
	unsigned int reg_val;
	char voltage_str[5];
	struct udevice *ic_devp = NULL;
	int i;

	ret = i2c_get_chip_for_busnum(bus_num, dev_addr, 1, &ic_devp);
	if (ret) {
		pr_err("LED: i2c get bus fail\n");
		return;
	}

	reg_val = dm_i2c_reg_read(ic_devp, reg_addr);
	for (i = 0; i < (sizeof(vddee_table)/sizeof(struct vol_table)); i++) {
		if((reg_val & 0x3f) == vddee_table[i].val)
			break;
	}

	if (i == sizeof(vddee_table)/sizeof(struct vol_table)) {
		env_set("vddee_voltage", "unknown");
	} else {
		env_set("vddee_voltage", vddee_table[i].voltage);
	}

	return ret;
}

U_BOOT_CMD(
  get_ee_vol, 1,  0,  do_get_vddee_voltage,
  "get the vddee voltage and export to cmdline",
  "get_ee_vol\n"
);
