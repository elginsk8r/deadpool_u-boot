#include <common.h>
#include <dm/device.h>
#include <i2c.h>
#include <linux/ssr/ps13216.h>

// AC power threshold in bootloader is set to 90 percent of the peak voltage
// (22.6V) of the lowest allowed VAC (16VAC), which is ~20V. The driver returns
// the squared value of HVSENSE voltage, hence the criteria for boot check is
//
//	sqrt(V_squared) / 1024 * 60 > 20 => V_squared * 9 > 1024 * 1024
//
#define AC_VOLTAGE_THRESHOLD_NUM (1 << 20)
#define AC_VOLTAGE_THRESHOLD_DEN 9

// Setting a 16V relaxed hard limit to AC voltage detection in u-boot.
// TODO(tzupengwang): Remove this once the threshold is finalized.
//
//	sqrt(V_squared) / 1024 * 60 > 16 => V_squared * 625 > 2^24
//
#define AC_VOLTAGE_HARD_THRESHOLD_NUM (1 << 24)
#define AC_VOLTAGE_HARD_THRESHOLD_DEN 225

// For development purpose, allow device boot with 5V USB power. The USB power
// introduces ~4.3V power to ACDCBUCK_IN.
//
//	sqrt(V_squared) / 1024 * 60 > 4 => V_squared * 625 > 1024 * 1024
//	sqrt(V_squared) / 1024 * 60 < 5 => V_squared * 9 < 256 * 256
//
#define VAC_USB_UPPER_THRESHOLD_NUM (1 << 16)
#define VAC_USB_UPPER_THRESHOLD_DEN 9
#define VAC_USB_LOWER_THRESHOLD_NUM (1 << 20)
#define VAC_USB_LOWER_THRESHOLD_DEN 225

// The V_rms is equal to the V_peak for DC power, while the V_rms is equal to
// V_peak/sqrt(2) for AC power. Considering the precision of the measurement,
// choose ratio 0.9 as the criteria for DC power check.
//
//  V_rms > 0.9 * V_peak
//  => V_rms * V_rms > 0.81 * V_peak * V_peak
//  => 100 * V_rms * V_rms > 81 * V_peak * V_peak
//
#define DC_VOLTAGE_RMS_PEAK_RATIO_NUM 81
#define DC_VOLTAGE_RMS_PEAK_RATIO_DEN 100

#define PS13216_I2C_REG 0x10

enum ssr_power_type {
	SSR_AC_POWER,
	SSR_DC_POWER,
};

static int last_squared_ac_voltage;

int ssr_probe(struct udevice **dev)
{
	int err;

	err = uclass_get_device_by_name(UCLASS_I2C_GENERIC,
					"ps13216@10", dev);
	if (err) {
		pr_err("Can't find ps13216 ssr (%d)\n", err);
		return err;
	}

	err = dm_i2c_probe((*dev)->parent, PS13216_I2C_REG, 0, dev);
	if (err) {
		pr_err("Can't probe ps13216 ssr (%d)\n", err);
		return err;
	}

	return 0;
}

static bool check_ac_voltage(int squared_voltage)
{
	return squared_voltage * AC_VOLTAGE_HARD_THRESHOLD_DEN >
	       AC_VOLTAGE_HARD_THRESHOLD_NUM;
}

static bool check_usb_voltage(int squared_voltage)
{
	return squared_voltage * VAC_USB_LOWER_THRESHOLD_DEN >
		       VAC_USB_LOWER_THRESHOLD_NUM &&
	       squared_voltage * VAC_USB_UPPER_THRESHOLD_DEN <
		       VAC_USB_UPPER_THRESHOLD_NUM;
}

int check_power(struct udevice *dev, enum gpio_state pgood_state)
{
	int err;
	int squared_voltage;
	int peak_voltage;
	enum ssr_power_type type;

	err = get_ssr_hvsense_voltage(dev, &squared_voltage, &peak_voltage);

	if (err) {
		pr_err("Failed to get SSR HV_Sense voltage: %d\n", err);
		return err;
	}

	pr_info("mean_squared: %d, peak: %d, squared_peak: %d\n",
		squared_voltage, peak_voltage, peak_voltage * peak_voltage);

	if (squared_voltage * DC_VOLTAGE_RMS_PEAK_RATIO_DEN >
	    peak_voltage * peak_voltage * DC_VOLTAGE_RMS_PEAK_RATIO_NUM) {
		type = SSR_DC_POWER;
	} else {
		type = SSR_AC_POWER;
	}

	last_squared_ac_voltage = squared_voltage;

	pr_info("detected %s power (%d)\n",
		(type == SSR_AC_POWER) ? "AC" : "DC", squared_voltage);

	// PX board
	if (pgood_state == NOT_AVAILABLE) {
		// Allow boot if voltage is sufficient AC power or USB power
		if (check_ac_voltage(squared_voltage) ||
		    check_usb_voltage(squared_voltage)) {
			return 0;
		}
	}

	// BX board with sufficient AC power
	if (pgood_state == HIGH && check_ac_voltage(squared_voltage)) {
		return 0;
	}

	// BX board with USB power
	if (pgood_state == LOW && type == SSR_DC_POWER &&
	    check_usb_voltage(squared_voltage)) {
		pr_info("Allow boot with usb power\n");
		return 0;
	}

	// return error if detected AC voltage does not satisfy the criteria
	if (squared_voltage * AC_VOLTAGE_THRESHOLD_DEN <
	    AC_VOLTAGE_THRESHOLD_NUM) {
		pr_warn("AC transformer voltage (%d) is lower than 20V\n",
			squared_voltage);
	}

	return -EAGAIN;
}

static int do_get_boot_ac_voltage(cmd_tbl_t *cmdtp, int flag, int argc,
		       char *const argv[])
{
	char voltage_str[10] = {0};

	if (argc != 1)
		return CMD_RET_USAGE;

	snprintf(voltage_str, sizeof(voltage_str), "%d", last_squared_ac_voltage);
	env_set("boot_ac_voltage", voltage_str);
	return 0;
}

U_BOOT_CMD(
	get_boot_ac_voltage, 1,	0, do_get_boot_ac_voltage,
	"env_set 'boot_ac_voltage'", "get_boot_ac_voltage"
);
