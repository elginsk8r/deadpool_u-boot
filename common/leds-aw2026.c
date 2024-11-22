#include <amlogic/leds-aw2026.h>
#include <dm.h>

#define CAL_FILENAME "led_calibration_LUT.txt"
#define AW2026_MAX_CURRENT AW2026_LED_3_1875mA

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

	reg_write(led_devp, AW2026_REG_RSTIDR, AW2026_LED_RSTIDR_MASK,
		  AW2026_LED_RSTIDR_RESET);
	reg_write(led_devp, AW2026_REG_GCR, AW2026_LED_CHIPEN_MASK,
		  AW2026_LED_CHIP_ENABLE);
	reg_write(led_devp, AW2026_REG_IMAX, AW2026_LED_IMX_MASK,
		  AW2026_MAX_CURRENT);
	reg_write(led_devp, AW2026_REG_LEDCTR, AW2026_LED_PWMLOG_MASK,
		  AW2026_LED_PWMLOG_LEANER);
	reg_write(led_devp, AW2026_REG_LEDEN, AW2026_LED_LEDEN_MASK,
		  AW2026_LED_LEDEN_TURN_ON_ALL);

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
	get_cal_settings(&cal_settings, "white:0:");
	for(i = 0; i < 3; ++i) {
		reg_write(led_devp, AW2026_REG_LCFG1 + i, AW2026_LED_LEDMD_MASK,
			AW2026_LED_ON_MODE);
		reg_write(led_devp, AW2026_REG_PWM1 + i, AW2026_LED_PWM_MASK,
			cal_settings[i]);
		reg_write(led_devp, AW2026_REG_ILED1 + i, AW2026_LED_ILED_MASK,
			cal_settings[3+i]);
        }
}

void turn_on_yellow_led(struct udevice *led_devp) {
	unsigned int cal_settings[N_CAL_SETTINGS] =
		{255, 255, 255, 0xff, 0xef, 0};
	int i;
	get_cal_settings(&cal_settings, "amber:0:");
	for(i = 0; i < 3; ++i) {
		reg_write(led_devp, AW2026_REG_LCFG1 + i, AW2026_LED_LEDMD_MASK,
			AW2026_LED_ON_MODE);
		reg_write(led_devp, AW2026_REG_PWM1 + i, AW2026_LED_PWM_MASK,
			cal_settings[i]);
		reg_write(led_devp, AW2026_REG_ILED1 + i, AW2026_LED_ILED_MASK,
			cal_settings[3+i]);
        }
}

void blink_yellow_led(struct udevice *led_devp) {
	int i;
	int val;

	turn_on_yellow_led(led_devp);

	for(i = 0; i < 3; ++i) {
		reg_write(led_devp, AW2026_REG_PAT1_T1 + i * PAT_FIELD_NUMBER,
			  AW2026_LED_PAT_TRISE_MASK, 0x0);
		reg_write(led_devp, AW2026_REG_PAT1_T2 + i * PAT_FIELD_NUMBER,
			  AW2026_LED_PAT_TFALL_MASK, 0x0);
		reg_write(led_devp, AW2026_REG_PAT1_T1 + i * PAT_FIELD_NUMBER,
			  AW2026_LED_PAT_TON_MASK,
			  AW2026_LED_PAT_ON_1SEC);
		reg_write(led_devp, AW2026_REG_PAT1_T2 + i * PAT_FIELD_NUMBER,
			  AW2026_LED_PAT_TOFF_MASK,
			  AW2026_LED_PAT_OFF_1SEC);
		reg_write(led_devp, AW2026_REG_PAT1_T4 + i * PAT_FIELD_NUMBER,
			  AW2026_LED_PAT_CTR_MASK,
			  AW2026_LED_PAT_CTR_INF_LOOP);
		reg_write(led_devp, AW2026_REG_LCFG1 + i,
			  AW2026_LED_LEDMD_MASK,
			  AW2026_LED_BLINK_MODE);
	}
	reg_write(led_devp, AW2026_REG_PATRUN, AW2026_LED_PATRUN_MASK,
		  AW2026_LED_PATRUN_START_ALL);
}
