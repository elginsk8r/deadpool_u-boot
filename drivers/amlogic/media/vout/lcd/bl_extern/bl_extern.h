/* SPDX-License-Identifier: (GPL-2.0+ OR MIT) */
/*
 * Copyright (c) 2019 Amlogic, Inc. All rights reserved.
 */

#ifndef _BL_EXTERN_H_
#define _BL_EXTERN_H_
#include <amlogic/media/vout/lcd/bl_extern.h>

#define BLEXT(fmt, args...)       printf("bl extern: "fmt"", ## args)
#define BLEXTERR(fmt, args...)    printf("bl extern: error: "fmt"", ## args)
#define BL_EXTERN_DRIVER         "bl_extern"

extern void bl_extern_i2c_bus_print(unsigned char i2c_bus);
extern int bl_extern_i2c_write(unsigned char i2c_bus, unsigned i2c_addr,
		unsigned char *buff, unsigned len);
extern int bl_extern_i2c_read(unsigned char i2c_bus, unsigned i2c_addr,
		unsigned char *buff, unsigned int len);

#ifdef CONFIG_AML_BL_EXTERN_I2C_LP8556
extern int i2c_lp8556_probe(void);
#endif

#ifdef CONFIG_AML_BL_EXTERN_MIPI_IT070ME05
extern int mipi_lt070me05_probe(void);
#endif


#endif

