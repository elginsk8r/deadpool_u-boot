/* SPDX-License-Identifier: GPL-2.0 */
/**
 * Copyright 2021 Google LLC.
 */

#ifndef __SSR_PS13216_H_
#define __SSR_PS13216_H_

#include <dm/device.h>

enum gpio_state {
	LOW,
	HIGH,
	NOT_AVAILABLE,
};

int get_ssr_hvsense_voltage(struct udevice *dev, int *mean_squared_voltage,
			    int *peak_voltage);
int ssr_probe(struct udevice **dev);
int check_power(struct udevice *dev, enum gpio_state pgood_state);

#endif
