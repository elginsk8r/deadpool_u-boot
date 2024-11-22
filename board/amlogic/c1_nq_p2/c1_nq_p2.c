
/*
 * board/amlogic/c1_nq_p2/c1_nq_p2.c
 *
 * Copyright (C) 2015 Amlogic, Inc. All rights reserved.
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
#include <asm/io.h>
#include <malloc.h>
#include <errno.h>
#include <environment.h>
#include <fdt_support.h>
#include <linux/crc8.h>
#include <linux/libfdt.h>
#include <amlogic/cpu_id.h>
#include <asm/arch/secure_apb.h>
#include <asm/arch/pinctrl_init.h>
#include <linux/sizes.h>
#include <asm-generic/gpio.h>
#include <dm.h>
#include <asm/armv8/mmu.h>
#include <amlogic/aml_v3_burning.h>
#include <amlogic/aml_v2_burning.h>
#include <amlogic/cv_data.h>
#include <linux/mtd/partitions.h>
#ifdef CONFIG_SYS_I2C_MESON
#include <i2c.h>
#include <dt-bindings/i2c/meson-i2c.h>
#include <fs.h>
#endif
#ifdef CONFIG_SECURE_POWER_CONTROL
#include <asm/arch/pwr_ctrl.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

#define LED_ENABLE_PIN_NAME "gpiom_7"

int led_power_enable(char *name)
{
	int ret;
	struct gpio_desc en_pin_desc;

	ret = dm_gpio_lookup_name(name, &en_pin_desc);
	if (ret) {
		pr_err("gpio: %s not found\n", name);
		return ret;
	}

	ret = dm_gpio_request(&en_pin_desc, name);
	if (ret && ret != -EBUSY) {
		pr_err("gpio: requesting pin %s failed\n", name);
		return;
	}
	ret = dm_gpio_set_dir_flags(&en_pin_desc, GPIOD_IS_OUT);
	if (ret) {
		pr_err("gpio: set direction failed\n");
		return ret;
	}
	ret = dm_gpio_set_value(&en_pin_desc, 1);
	if (ret) {
		pr_err("gpio: set value failed\n");
		return ret;
	}
}

// Checks CV_DATA in SRAM to get the OOBE status.
bool oobe_complete(void)
{
	struct CvRwdata *rw_data = CV_RWDATA_BASE_ADDR;
	u8 version = *((u8 *)CV_DATA_VER_ADDR);
	u8 valid = (version == CV_DATA_CUR_VER) &&
		   (rw_data->size <= CV_DATA_SIZE_LIMIT) &&
		   (crc8(0, rw_data, rw_data->size - 1) == rw_data->crc8);
	return valid && rw_data->oobe_status && rw_data->rtos_enable;
}

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
	ret = fs_read("led_calibration_LUT.txt", buf, seek, len - 1,
		      &len_read);
	if (ret) {
		pr_err("LED: fs_read error=%d\n", ret);
		return ret;
	}
	// fs_read does not add a null terminator
	buf[len_read] = '\0';

	return 0;
}

// Gets calibration settings for the status LED:
// pwm_b, pwm_g, pwm_r, current_b, current_g, current_r
// On failure, |settings| is not modified.
#define N_CAL_SETTINGS 6
void get_cal_settings(unsigned int *settings)
{
	int i;
	unsigned long settings_[N_CAL_SETTINGS];

	// Max length to get whole cal line with a newline, plus one for terminator
	const u8 max_chars = 36;
	// Add one for null terminator
	char buf[max_chars];

	if (get_cal_string(&buf, max_chars))
		return;

	const char color_header[] = "White:4095:";
	const char *p, *p_next;

	p = strstr(buf, color_header);
	if (!p) {
		pr_err("LED: \"%s\" not found in cal file segment: %s\n",
		       color_header, buf);
		return;
	}
	p += strlen(color_header);

	// Cal file and device settings differ in order.
	// Cal file may provide 1 or 3 currents
	// Cal file: pwm_r,pwm_g,pwm_b;cur[,cur_g,cur_b]
	// Settings: pwm_b pwm_g pwm_r cur_b cur_g cur_r
	const u8 map[] = {2, 1, 0, 5, 4, 3};

	for (i = 0; i < N_CAL_SETTINGS; ++i) {
		unsigned long value = simple_strtoul(p, &p_next, 10);

		if (value > 255) {
			pr_err("LED: value %d is too large: %lu\n",
			       settings_[i]);
			return;
		}
		settings_[map[i]] = value;

		// Skip check that |p_next| is valid on last iteration
		if (i == 5)
			break;
		// Is there not a separator, or not another number
		if ((*p_next != ',' && *p_next != ';') ||
		    !isdigit(p_next[1])) {
			// Is this the first current
			if (i == 3) {
				// One current instead of RGB current
				settings_[map[4]] = value;
				settings_[map[5]] = value;
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

void sys_led_init(void)
{
#ifdef CONFIG_SYS_I2C_MESON
	int ret, i;
	struct udevice *led_devp = NULL;

	ret = led_power_enable(LED_ENABLE_PIN_NAME);
	if (ret) {
		pr_err("LED: LED power enable fail\n");
		return;
	}
	ret = i2c_get_chip_for_busnum(MESON_I2C_M3, 0X30, 1, &led_devp);
	if (ret) {
		pr_err("LED: i2c get bus fail\n");
		return;
	}

	// PWMs and currents.
	// Default to dim purple if there's no cal.
	unsigned int cal_settings[N_CAL_SETTINGS] = {73, 0, 125, 100, 100, 100};

	get_cal_settings(&cal_settings);

	dm_i2c_reg_write(led_devp, 0x0, 0x40);
	mdelay(1);
	dm_i2c_reg_write(led_devp, 0x1, 0x3f);
	dm_i2c_reg_write(led_devp, 0x8, 0x61);
	for (i = 0; i < N_CAL_SETTINGS; ++i)
		dm_i2c_reg_write(led_devp, i + 2, cal_settings[i]);
#endif  // CONFIG_SYS_I2C_MESON
}

int serial_set_pin_port(unsigned long port_base)
{
    return 0;
}

int dram_init(void)
{
	gd->ram_size = PHYS_SDRAM_1_SIZE;
	return 0;
}

/* secondary_boot_func
 * this function should be write with asm, here, is is only for compiling pass
 * */
void secondary_boot_func(void)
{
}

int board_eth_init(bd_t *bis)
{
	return 0;
}

int active_clk(void)
{
	struct udevice *clk = NULL;
	int err;

	err = uclass_get_device_by_name(UCLASS_CLK,
			"xtal-clk", &clk);
	if (err) {
		pr_err("Can't find xtal-clk clock (%d)\n", err);
		return err;
	}
	err = uclass_get_device_by_name(UCLASS_CLK,
			"clock-controller@0", &clk);
	if (err) {
		pr_err("Can't find clock-controller@0 clock (%d)\n", err);
		return err;
	}

	return 0;
}

void board_init_mem(void) {
	/* config bootm low size, make sure whole dram/psram space can be used */
	phys_size_t ram_size;
	char *env_tmp;
	env_tmp = env_get("bootm_size");
	if (!env_tmp) {
		ram_size = (((readl(SYSCTRL_SEC_STATUS_REG4)) & 0xFFFF0000) << 4);
		env_set_hex("bootm_low", 0);
		env_set_hex("bootm_size", ram_size);
	}
}

int board_init(void)
{
	pr_info("board init\n");
	//Please keep try usb boot first in board_init, as other init before usb may cause burning failure
#if defined(CONFIG_AML_V3_FACTORY_BURN) && defined(CONFIG_AML_V3_USB_TOOl)
	if ((0x1b8ec003 != readl(SYSCTRL_SEC_STICKY_REG2)) && (0x1b8ec004 != readl(SYSCTRL_SEC_STICKY_REG2)))
	{ aml_v3_factory_usb_burning(0, gd->bd); }
#endif//#if defined(CONFIG_AML_V3_FACTORY_BURN) && defined(CONFIG_AML_V3_USB_TOOl)

	pinctrl_devices_active(PIN_CONTROLLER_NUM);
	active_clk();

	if (!oobe_complete())
		sys_led_init();

	/* Disable PM_ETH */
#ifdef CONFIG_SECURE_POWER_CONTROL
	pwr_ctrl_psci_smc(PM_ETH, 1);
#endif

	return 0;
}

int board_late_init(void)
{
	pr_info("board late init\n");
	board_init_mem();

#ifdef CONFIG_AML_FACTORY_BURN_LOCAL_UPGRADE //try auto upgrade from ext-sdcard
	aml_try_factory_sdcard_burning(0, gd->bd);
#endif//#ifdef CONFIG_AML_FACTORY_BURN_LOCAL_UPGRADE
	//auto enter usb mode after board_late_init if 'adnl.exe setvar burnsteps 0x1b8ec003'
#if defined(CONFIG_AML_V3_FACTORY_BURN) && defined(CONFIG_AML_V3_USB_TOOl)
	if (0x1b8ec003 == readl(SYSCTRL_SEC_STICKY_REG2))
	{ aml_v3_factory_usb_burning(0, gd->bd); }
#endif//#if defined(CONFIG_AML_V3_FACTORY_BURN) && defined(CONFIG_AML_V3_USB_TOOl)
	return 0;
}


phys_size_t get_effective_memsize(void)
{
#ifdef UBOOT_RUN_IN_SRAM
	return 0x180000; /* SRAM 1.5MB */
#else
	// >>16 -> MB, <<20 -> real size, so >>16<<20 = <<4
#if defined(CONFIG_SYS_MEM_TOP_HIDE)
	return (((readl(SYSCTRL_SEC_STATUS_REG4)) & 0xFFFF0000) << 4) - CONFIG_SYS_MEM_TOP_HIDE;
#else
	return (((readl(SYSCTRL_SEC_STATUS_REG4)) & 0xFFFF0000) << 4);
#endif /* CONFIG_SYS_MEM_TOP_HIDE */
#endif /* UBOOT_RUN_IN_SRAM */
}

static struct mm_region bd_mem_map[] = {
	{
		.virt = 0x00000000UL,
		.phys = 0x00000000UL,
		.size = 0x80000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) |
			 PTE_BLOCK_INNER_SHARE
	}, {
		.virt = 0x80000000UL,
		.phys = 0x80000000UL,
		.size = 0x80000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
			 PTE_BLOCK_NON_SHARE |
			 PTE_BLOCK_PXN | PTE_BLOCK_UXN
	}, {
		/* List terminator */
		0,
	}
};

struct mm_region *mem_map = bd_mem_map;

int mach_cpu_init(void) {
	pr_info("\nmach_cpu_init\n");
	return 0;
}

int ft_board_setup(void *blob, bd_t *bd)
{
	/* eg: bl31/32 rsv */
	return 0;
}

int do_get_nq_hw_id(cmd_tbl_t *cmdtp, int flag, int argc,
		char * const argv[])
{
	unsigned int hw_id = 0, ret = 0;
	char hw_id_str[8] = {0};  // eg: 0x0A

	/*
	 * HWID_4 : GPIOX_10
	 * HWID_3 : GPIOX_9
	 * HWID_2 : GPIOX_8
	 * HWID_1 : GPIOX_7
	 * HWID_0 : GPIOX_6
	 *
	 */
	/* pin mux to gpio pin */
	ret = readl(PADCTRL_PIN_MUX_REG6);
	writel(ret & (~(0xFF << 24)), PADCTRL_PIN_MUX_REG6);
	ret = readl(PADCTRL_PIN_MUX_REG7);
	writel(ret & (~(0xFFF << 0)), PADCTRL_PIN_MUX_REG7);

	/* enable input */
	ret = readl(PADCTRL_GPIOX_OEN);
	writel(ret | (0x1F << 6), PADCTRL_GPIOX_OEN);

	/* disable gpio pull */
	ret = readl(PADCTRL_GPIOX_PULL_EN);
	writel(ret & (~(0x1F << 6)), PADCTRL_GPIOX_PULL_EN);

	/* read hw id */
	ret = readl(PADCTRL_GPIOX_I);
	hw_id = (ret >> 6) & 0x1F;

	snprintf(hw_id_str, sizeof(hw_id_str), "0x%02x", hw_id);
	env_set("hw_id", hw_id_str);
	return 0;
}

U_BOOT_CMD(
	get_nq_hw_id, 1, 0, do_get_nq_hw_id,
	"get NQ HW_ID and env_set 'hw_id'\n",
	"get_nq_hw_id"
);

int do_get_wake_args(cmd_tbl_t *cmdtp, int flag, int argc,
		     char * const argv[])
{
	const volatile struct RtosStatus * const rtos_status =
		RTOS_STATUS_BASE_ADDR;
	char wake_args_str[150] = {0};

	u16 wake_reasons;
	if ((rtos_status->crc8 ==
	    crc8(0, rtos_status, sizeof(struct RtosStatus) - 1)) &&
	    oobe_complete()) {
		wake_reasons = rtos_status->wakeup_reasons;
	} else {
		wake_reasons = 0;
	}

	/* MCU_RESET (5) */
	const u8 mcu_reset = (wake_reasons & 0x20) != 0;
	/* PIR (0), WiFi (1), doorbell (2), tamper (7), low battery (9),
	 *  charger fault (12)
	 */
	const u8 fast_reason = (wake_reasons & 0x1287) != 0;

	const u8 fastpath = !mcu_reset && fast_reason;

	char *mode;
	if (wake_reasons == 0 || mcu_reset)
		mode = "cold";
	else if (wake_reasons == 0x1)
		mode = "partial_warm";
	else
		mode = "warm";

	char* event_path = NULL;
	if (wake_reasons & 0x4) {
		event_path = "doorbell";
	} else if (wake_reasons & 0x2) {
		event_path = "wifi";
	} else if (wake_reasons & 0x1) {
		event_path = "pir";
	} else {
		event_path = "other";
	}

	snprintf(wake_args_str, sizeof(wake_args_str),
		 "androidboot.wake_reasons=0x%04x androidboot.bootpath=%s "
		 "androidboot.eventpath=%s dhd.load_mode=%s",
		 wake_reasons, fastpath ? "fast" : "slow", event_path, mode);
	env_set("wake_args", wake_args_str);

	return 0;
}

U_BOOT_CMD(get_wake_args, 1, 0, do_get_wake_args,
	   "Get wake_reasons and bootpath, envset wake_args\n",
	   "Parses CV status from RTOS extract wake_reasons and bootpath.\n"\
	   "envsets |wake_args| to add to kernel command line, contains:\n"\
	   "  androidboot.wake_reason=0x04x: wake_reasons from RTOS\n"\
	   "  androidboot.bootpath=(fast|slow): boot quickly or check RW FS\n"
	   "  androidboot.eventpath=:(event string) specify type of fast boot\n"
	   "  dhd.load_mode=(cold|partial_warm|warm) specify load mode for wifi driver\n"
	   "    cold: wifi hardware is not expected to be in the init and sleeping state\n"
	   "    partial_warm: wifi hardware is asleeping, will not be woken immediately on boot\n"
	   "    warm: wifi hardware is sleeping, will wake ASAP on boot\n");

/* partition table */
/* partition table for spinand flash */
#ifdef CONFIG_SPI_NAND
static const struct mtd_partition spinand_partitions[] = {
	{
		.name = "logo",
		.offset = 0,
		.size = 2 * SZ_1M,
	},
	{
		.name = "recovery",
		.offset = 0,
		.size = 16 * SZ_1M,
	},
	{
		.name = "boot",
		.offset = 0,
		.size = 16 * SZ_1M,
	},
	{
		.name = "system",
		.offset = 0,
		.size = 64 * SZ_1M,
	},
	/* last partition get the rest capacity */
	{
		.name = "data",
		.offset = MTDPART_OFS_APPEND,
		.size = MTDPART_SIZ_FULL,
	}
};
struct mtd_partition *get_partition_table(int *partitions)
{
	*partitions = ARRAY_SIZE(spinand_partitions);
	return spinand_partitions;
}
#endif /* CONFIG_SPI_NAND */

int __attribute__((weak)) mmc_initialize(bd_t *bis){ return 0;}

int __attribute__((weak)) do_bootm(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]){ return 0;}

void __attribute__((weak)) set_working_fdt_addr(ulong addr) {}

int __attribute__((weak)) ofnode_read_u32_default(ofnode node, const char *propname, u32 def) {return 0;}

void __attribute__((weak)) md5_wd (unsigned char *input, int len, unsigned char output[16],	unsigned int chunk_sz){}
