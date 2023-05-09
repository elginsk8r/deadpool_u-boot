/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2022 Google LLC
 */
#ifndef _BOARD_VARIANT_H_
#define _BOARD_VARIANT_H_

#define BOARD_VARIANT_PROD    0xAA55
#define BOARD_VARIANT_DEV     0x55AA
#define BOARD_VARIANT_UNKNOWN 0xA5A5

/* implement in $(BOARD)/$(BOARD).c */
int get_board_variant(void);

#endif  // _BOARD_VARIANT_H_
