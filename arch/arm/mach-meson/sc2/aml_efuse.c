// SPDX-License-Identifier: (GPL-2.0+ OR MIT)
/*
 * Copyright (c) 2019 Amlogic, Inc. All rights reserved.
 */

/*
 *
 * Copyright (C) 2018 Amlogic, Inc. All rights reserved.
*/

#include <common.h>
#include <asm/arch/io.h>
#include <amlogic/aml_efuse.h>
#include <asm/arch/secure_apb.h>

//function for EFUSE license query
int  IS_FEAT_BOOT_VERIFY(void)
{
	uint32_t lic_val;

	lic_val = readl(OTP_LIC00);

	if (((lic_val>>8) & 0x3ff0c) == 0x3ff0c)
		return 1;

	return 0;
}
