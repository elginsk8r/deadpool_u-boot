
/*
 * cmd/amlogic/cmd_get_chiptype.c
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
#include <asm/arch/bl31_apis.h>
#include <amlogic/cpu_id.h>
#include <asm/arch/register.h>

int do_get_chiptype(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	char *type[] = {"SS", "SF", "TT", "FF"};
	unsigned int dvfs_id = aml_get_dvfs_id();
	pr_info("get dvfs_id:%d\n", dvfs_id);
	if (dvfs_id > 3) {
		pr_err("fail to get dvfs id\n");
		dvfs_id = 0;
	}
	env_set("chip_type", type[dvfs_id]);
	return 0;
}

U_BOOT_CMD(
	get_chiptype, 1, 0, do_get_chiptype,
	"get c328x chip type and env_set 'chip_type'\n",
	"get_chiptype"
);

int do_get_cpu_rev(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	cpu_id_t cpu_id = get_cpu_id();
	char rev[4] = { 0 };
	unsigned int reg_val = 0;

	if (cpu_id.family_id == MESON_CPU_MAJOR_ID_C1) {
		snprintf(rev, sizeof(rev), "%x", cpu_id.chip_rev);
	} else {
		snprintf(rev, sizeof(rev), "%d", -1);
	}

	reg_val = readl(SYSCTRL_STICKY_REG5);
	strcat(rev, "-");
	if (reg_val == 1) {
		/* ACRK key */
		strcat(rev, "r");
	} else {
		/* DVUK key */
		strcat(rev, "u");
	}

	env_set("cpu_rev", rev);

	return 0;
}

U_BOOT_CMD(
	get_cpu_rev, 1, 0, do_get_cpu_rev,
	"get cpu revision",
	"  This command will get and setenv 'cpu_rev'\n"
);
