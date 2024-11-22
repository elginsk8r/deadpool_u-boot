#include <amlogic/leds-aw210xx.h>
#include <dm.h>

#define CAL_FILENAME "led_calibration_LUT.txt"

// Copied from
// https://eureka-partner-review.googlesource.com/c/amlogic/u-boot/+/149113
// Reads "led_calibration_LUT.txt" from the factory partition.
// Forwards errors, returns 0 on success.
int get_cal_string(char *buf, int len)
{
	int ret;
	loff_t len_read;
	const loff_t seek = 21;

	ret = fs_set_blk_dev("mmc", "1:4", FS_TYPE_EXT);
	if (ret) {
		pr_err("LED: fs_set_blk_dev error=%d\n", ret);
		return ret;
	}
	// Leave space for terminator
	ret = fs_read(CAL_FILENAME, buf, seek, len - 1,
		      &len_read);
	if (ret) {
		pr_err("LED: fs_read error=%d\n", ret);
		return ret;
	}
	// fs_read does not add a null terminator
	buf[len_read] = '\0';

	return 0;
}

// Copied from
// https://eureka-partner-review.googlesource.com/c/amlogic/u-boot/+/149113
// Gets calibration settings for the status LED:
// pwm_r, pwm_g, pwm_b, current_r, current_g, current_b
// On failure, |settings| is not modified.
void get_cal_settings(unsigned int *settings, const char color_header[])
{
	int i;
	unsigned long settings_[N_CAL_SETTINGS];

	// Max length to cover all color lines
	// 9 colors * (color_name + ":4095:" + xxx[,|;|\n] * 6)
	// 9 * (14 + 6 + 4 * 6 )
	const unsigned int max_chars = 396;
	// Add one for null terminator
	char buf[max_chars];

	if (get_cal_string(&buf, max_chars))
		return;

	const char *p, *p_next;

	p = strstr(buf, color_header);
	if (!p) {
		pr_err("LED: \"%s\" not found in cal file segment: %s\n",
		       color_header, buf);
		return;
	}
	p += strlen(color_header);

	for (i = 0; i < N_CAL_SETTINGS; ++i) {
		unsigned long value = simple_strtoul(p, &p_next, 10);

		if (value > 255) {
			pr_err("LED: value %d is too large: %lu\n",
			       settings_[i]);
			return;
		}
		settings_[i] = value;

		// Skip check that |p_next| is valid on last iteration
		if (i == 5)
			break;
		// Is there not a separator, or not another number
		if ((*p_next != ',' && *p_next != ';') ||
		    !isdigit(p_next[1])) {
			// Is this the first current
			if (i == 3) {
				// One current instead of RGB current
				settings_[4] = value;
				settings_[5] = value;
				break;
			}
			// Invalid
			pr_err("LED calibration file is malformed\n");
			return;
		}

		p = p_next + 1;
	}

	for (i = 0; i < N_CAL_SETTINGS; ++i)
		settings[i] = (unsigned int)settings_[i];
}

void reg_write(struct udevice *led_devp, int reg, int mask, int val) {
	int old_val;
	int new_val;

	old_val = dm_i2c_reg_read(led_devp, reg);
	if (old_val < 0) {
		pr_err("LED: read %d reg failed\n", reg);
		return;
	}
	new_val = (old_val & ~mask) | (val & mask);
	dm_i2c_reg_write(led_devp, reg, new_val);
}

void sys_led_init(enum LED_ANIMATION led_animation)
{
#ifdef CONFIG_SYS_I2C_MESON
	int ret;
	struct udevice *led_devp = NULL;
	ret = i2c_get_chip_for_busnum(I2C_BUS_NUM, I2C_LED_REG, 1, &led_devp);
	if (ret) {
		pr_err("LED: i2c get bus fail\n");
		return;
	}

	reg_write(led_devp, AW210XX_REG_RESET, AW210XX_RESET_MASK,
		  AW210XX_RESET_CHIP);
	reg_write(led_devp, AW210XX_REG_GCR, AW210XX_BIT_CHIPEN_MASK,
		  AW210XX_BIT_CHIPEN_ENABLE);
	reg_write(led_devp, AW210XX_REG_GCR, AW210XX_BIT_APSE_MASK,
		  AW210XX_BIT_APSE_ENABLE);
	reg_write(led_devp, AW210XX_REG_GCCR, AW210XX_GLOBAL_CURRENT_MASK,
		  AW210xx_MAX_CURRENT);
	reg_write(led_devp, AW210XX_REG_PHCR, AW210XX_PDE_MASK,
		  AW210XX_PDE_ENABLE);

	switch(led_animation) {
	case WHITE:
		turn_on_white_led(led_devp);
		break;
	case YELLOW:
		turn_on_yellow_led(led_devp);
		break;
	case BLINK_YELLOW:
		blink_yellow_led(led_devp);
		break;
	default:
		pr_err("LED: unknown led animation\n");
	}
#endif  // CONFIG_SYS_I2C_MESON
}

void turn_on_white_led(struct udevice *led_devp) {
	unsigned int cal_settings[N_CAL_SETTINGS] =
		{128, 0, 128, 100, 100, 100};
	int i;
	get_cal_settings(&cal_settings, "white:255:");
	// HW design: LED7 is blue, LED8 is red, LED9 is green
	// Calibration file is: R,G,B
	for(i = 0; i < 3; ++i) {
		if (i == 2) {
			// blue led
			reg_write(led_devp, AW210XX_REG_SL06, AW210XX_SLXX_MASK,
					cal_settings[3+i]);
			reg_write(led_devp, AW210XX_REG_BR06L,
					AW210XX_BRXX_MASK, cal_settings[i]);
		} {
			// red led and green led
			reg_write(led_devp, AW210XX_REG_SL07 + i,
					AW210XX_SLXX_MASK, cal_settings[3+i]);
			reg_write(led_devp, AW210XX_REG_BR07L + i*2,
					AW210XX_BRXX_MASK, cal_settings[i]);
		}
        }
	reg_write(led_devp, AW210XX_REG_UPDATE, AW210XX_UPDATE_MASK,
			AW210XX_UPDATE_ENABLE);
}

void turn_on_yellow_led(struct udevice *led_devp) {
	unsigned int cal_settings[N_CAL_SETTINGS] =
		{255, 255, 255, 0xff, 0xef, 0};
	int i;

	get_cal_settings(&cal_settings, "amber:255:");
	// HW design: LED7 is blue, LED8 is red, LED9 is green
	// Calibration file is: R,G,B
	for(i = 0; i < 3; ++i) {
		if (i == 2) {
			// blue led
			reg_write(led_devp, AW210XX_REG_SL06, AW210XX_SLXX_MASK,
					cal_settings[3+i]);
			reg_write(led_devp, AW210XX_REG_BR06L,
					AW210XX_BRXX_MASK, cal_settings[i]);
		} else {
			// red led and green led
			reg_write(led_devp, AW210XX_REG_SL07 + i,
					AW210XX_SLXX_MASK, cal_settings[3+i]);
			reg_write(led_devp, AW210XX_REG_BR07L + i*2,
					AW210XX_BRXX_MASK, cal_settings[i]);
		}
        }
	reg_write(led_devp, AW210XX_REG_UPDATE, AW210XX_UPDATE_MASK,
			AW210XX_UPDATE_ENABLE);
}

void blink_yellow_led(struct udevice *led_devp) {
	unsigned int cal_settings[N_CAL_SETTINGS] =
		{255, 255, 255, 0xff, 0xef, 0};
	int i;

	reg_write(led_devp, AW210XX_REG_GCFG, AW210XX_GE_MASK,
			AW210XX_GE2_SET);
	reg_write(led_devp, AW210XX_REG_ABMCFG, AW210XX_PATCFG_MASK,
			AW210XX_PATE_ENABLE | AW210XX_PATMD_AUTO);
	reg_write(led_devp, AW210XX_REG_ABMT0, AW210XX_PAT_RISE_MASK,
			AW210XX_PAT_RISE_TIME);
	reg_write(led_devp, AW210XX_REG_ABMT1, AW210XX_PAT_FALL_MASK,
			AW210XX_PAT_FALL_TIME);
	reg_write(led_devp, AW210XX_REG_ABMT0, AW210XX_PAT_ON_MASK,
			AW210XX_PAT_ON_TIME);
	reg_write(led_devp, AW210XX_REG_ABMT1, AW210XX_PAT_OFF_MASK,
			AW210XX_PAT_OFF_TIME);

	get_cal_settings(&cal_settings, "amber:255:");
	// HW design: LED7 is blue, LED8 is red, LED9 is green
	// datasheet: in group mode
	// GSLR: for ledx(x=1,4,7)
	// GSLG: for ledx(x=2,5,8)
	// GSLB: for ledx(x=3,6,9)
	for(i = 0; i < 3; ++i) {
		if (i == 2) {
			// blue led
			reg_write(led_devp, AW210XX_REG_GSLR, AW210XX_SLXX_MASK,
					cal_settings[3+i]);
		} else {
			// red led and green led
			reg_write(led_devp, AW210XX_REG_GSLG + i,
					AW210XX_SLXX_MASK, cal_settings[3+i]);
		}
        }

	// set brightness range
	reg_write(led_devp, AW210XX_REG_GBRH, AW210XX_BRXX_MASK,
			AW210xx_MAX_BRIGHTNESS);
	reg_write(led_devp, AW210XX_REG_GBRL, AW210XX_BRXX_MASK,
			AW210xx_MIN_BRIGHTNESS);

	// start run
	reg_write(led_devp, AW210XX_REG_ABMGO, AW210XX_PAT_RUN_MASK,
			AW210XX_PAT_RUN);
}
