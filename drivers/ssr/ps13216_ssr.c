// SPDX-License-Identifier: GPL-2.0
/**
 * Author:
 * Paul Wang <tzupengwang@google.com>
 *
 * Description:
 * Driver for Schumacher (Photeon ps13216 Solid-State Relay)
 *
 * This simplified version of the PS13216 driver only supports HVSENSE reading.
 *
 * Copyright 2021 Google LLC.
 */

#include <asm-generic/gpio.h>
#include <dm/device.h>
#include <dm/uclass-id.h>
#include <i2c.h>
#include <linux/compat.h>
#include <linux/delay.h>
#include <linux/ssr/ps13216_regs.h>
#include <linux/ssr/ps13216.h>

/**
 * Misc constants
 */

// It takes 34 ms to go from OFF state to RUN state
#define PS13216_IF_EN_STARTUP_DELAY_MS		34
#define PS13216_HVSENSE_ADC_CAM_DELAY_MS	128
#define PS13216_ADC_FIFO_BANK_BYTES		256

// Delay value for charge pumps. ADC readings
// taken before the charge pumps have fully
// charged will be inaccurate.
#define PS13216_CHARGE_PUMP_DELAY_MS		30

struct ps13216_device {
	struct gpio_desc *enable_gpio;
	u8 adc_fifo_data[PS13216_ADC_FIFO_BANK_BYTES];
};

int get_ssr_hvsense_voltage(struct udevice *dev, int *mean_squared_voltage,
			    int *peak_voltage) {
	struct ps13216_device *ps = dev->platdata;
	u16 fifo_len;
	u8 *fifo_data_ptr;
	size_t sample_count;
	size_t idx;
	int raw_voltage;

	// Clear ADC EOC (End of Conversion) event flag.
	dm_i2c_reg_write(dev, PS13216_REG_EVENT1, PS13216_M_EVENT1_EOC);

	dm_i2c_reg_write(dev, PS13216_REG_ADC_CTRL,
		     (PS13216_V_ADC_CTRL_MODE(PS13216_ADC_MODE_CAM) |
		      PS13216_M_ADC_CTRL_HV_SENSE));
	mdelay(PS13216_HVSENSE_ADC_CAM_DELAY_MS);

	dm_i2c_reg_write(dev, PS13216_REG_ADC_CTRL, 0x00);

	// Read FIFO length
	dm_i2c_read(dev, PS13216_REG_ADC_FIFO_FILL_CNT_LSB, (u8 *)&fifo_len, 2);

	for (idx = 0; idx < fifo_len; idx ++) {
		dm_i2c_read(dev, PS13216_REG_ADC_FIFO, &ps->adc_fifo_data[idx], 1);
	}

	// Each voltage read takes 2 bytes
	sample_count = fifo_len / 2;
	if (sample_count == 0) {
		return -EAGAIN;
	}

	fifo_data_ptr = ps->adc_fifo_data;
	*mean_squared_voltage = 0;
	*peak_voltage = INT_MIN;

	for (idx = 0; idx < sample_count; fifo_data_ptr += 2, ++idx) {
		raw_voltage = ((int)fifo_data_ptr[1] << 8) | fifo_data_ptr[0];
		*mean_squared_voltage += raw_voltage * raw_voltage;
		if (*peak_voltage < raw_voltage)
			*peak_voltage = raw_voltage;
	}
	*mean_squared_voltage /= sample_count;

	return 0;
}

static int ps13216_probe(struct udevice *dev) {
	struct ps13216_device *ps = dev->platdata;
	int ret;

	// Initialize SSR_EN gpio and enable Schumacher
	ret = gpio_request_by_name(dev, "enable-gpios", 0,
                             ps->enable_gpio, GPIOD_IS_OUT | GPIOD_IS_OUT_ACTIVE);
	if (ret < 0) {
		dev_err(dev, "%s: failed to initialize enable_gpio.\n", __func__);
		goto error;
	}
	mdelay(PS13216_IF_EN_STARTUP_DELAY_MS);

	dev_info(dev, "%s: probe successfully!\n", __func__);

	return 0;

error:
	dev_err(dev, "%s: error %d\n", __func__, ret);
	return ret;
}

static struct udevice_id ps13216_of_match[] = {
	{
		.compatible = "photeon,ps13216",
	},
	{ },
};

U_BOOT_DRIVER(ps13216_ssr) = {
	.name = "ps13216-ssr",
	.id = UCLASS_I2C_GENERIC,
	.of_match = ps13216_of_match,
	.platdata_auto_alloc_size = sizeof(struct ps13216_device),
	.probe = ps13216_probe,
};
