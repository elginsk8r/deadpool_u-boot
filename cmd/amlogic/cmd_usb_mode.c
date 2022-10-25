
/*
 * cmd/amlogic/cmd_usb_mode.c
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
#include <amlogic/flash_ts.h>

void clean_strings(char *dst, const char *src)
{
	char *p1 = strstr(dst, src);
	if (p1) {
		const int src_len = strlen(src);
		memmove(p1, p1 + src_len, strlen(p1) - src_len + 1);
	}
}

int update_bootargs(char* arg)
{
	char *bootargs = NULL;
	char *newbootargs = NULL;
	const char *search_key1 = "usb_controller_type=host";
	const char *search_key2 = "usb_controller_type=device";

	bootargs = env_get("bootargs");
	if (!bootargs) {
		bootargs = "\0";
	}

	/* arg is present in args, no need to set again */
	if (strstr(bootargs, arg))
		return 0;

	/*clean search_key1 & search_key2*/
	if (!strncmp(arg, search_key1, strlen(search_key1))) {
		clean_strings(bootargs, search_key2);
	} else if (!strncmp(arg, search_key2, strlen(search_key2))) {
		clean_strings(bootargs, search_key1);
	}

	newbootargs = malloc(strlen(bootargs) + strlen(arg) + 1 + 1);
	if (!newbootargs) {
		printf("failed to allocate buffer for bootarg\n");
		return -1;
	}

	sprintf(newbootargs, "%s %s", bootargs, arg);
	env_set("bootargs", newbootargs);

	return 0;
}

int do_set_usb_mode_to_bootargs (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	/*get bootargs*/
	if(is_flash_inited()) {
		flash_ts_init();

		const char *fts_key = "usb_controller_type";
		char fts_value[256] = { 0 };

		flash_ts_get(fts_key, fts_value, sizeof(fts_value));
		printf("FTS read: usb_controller_type -> %s\n", fts_value);

		if (strncmp(fts_value, "host", sizeof(fts_value)) == 0) {
			update_bootargs("usb_controller_type=host");
		} else if (strncmp(fts_value, "device", sizeof(fts_value)) == 0) {
			update_bootargs("usb_controller_type=device");
		}
	}

	return 0;
}

int do_set_host_mode_to_bootargs (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	update_bootargs("usb_controller_type=host");
	return 0;
}

int do_set_device_mode_to_bootargs (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	update_bootargs("usb_controller_type=device");
	return 0;
}

U_BOOT_CMD(
  set_usb_mode_to_bootargs, 1,  0,  do_set_usb_mode_to_bootargs,
  "Set usb mode to bootargs, usb mode flag from fts partition",
  "set_usb_mode_to_bootargs\n"
);

U_BOOT_CMD(
  set_host_mode_to_bootargs, 1,  0,  do_set_host_mode_to_bootargs,
  "Set usb host mode to bootargs",
  "set_host_mode_to_bootargs\n"
);

U_BOOT_CMD(
  set_device_mode_to_bootargs, 1,  0,  do_set_device_mode_to_bootargs,
  "Set usb device mode to bootargs",
  "set_device_mode_to_bootargs\n"
);
