
/*
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
#include <linux/mtd/partitions.h>
#ifdef CONFIG_SYS_I2C_MESON
#include <i2c.h>
#include <dt-bindings/i2c/meson-i2c.h>
#include <fs.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

#define N_CAL_SETTINGS 6
#define BUS_NUM 4
#define I2C_LED_REG 0x64
#define AW2015_REG_GCR 0x01
#define AW2015_REG_IMAX 0x03
#define AW2015_REG_LEDCTR 0x08
#define AW2015_LED_ON_MODE_MASK 0x00
#define AW2015_LED_CHIP_ENABLE_MASK 0x01
#define AW2015_LED_CHARGE_DISABLE_MASK 0x02
#define AW2015_REG_LCFG1 0x04
#define AW2015_REG_LEDEN 0x07
#define AW2015_REG_ILED1 0x10
#define AW2015_REG_PWM1 0x1C

// Copied from
// https://eureka-partner-review.googlesource.com/c/amlogic/u-boot/+/149113
// Reads "status_led_calibration_LUT.txt" from the factory partition.
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
	ret = fs_read("status_led_calibration_LUT.txt", buf, seek, len - 1,
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
// pwm_b, pwm_g, pwm_r, current_b, current_g, current_r
// On failure, |settings| is not modified.
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
	ret = i2c_get_chip_for_busnum(BUS_NUM, I2C_LED_REG, 1, &led_devp);
	if (ret) {
		pr_err("LED: i2c get bus fail\n");
		return;
	}

	// PWMs and currents.
	// Default to dim purple if there's no cal.
	unsigned int cal_settings[N_CAL_SETTINGS] = {128, 0, 128, 100, 100, 100};

	get_cal_settings(&cal_settings);

	dm_i2c_reg_write(led_devp, AW2015_REG_GCR,
			AW2015_LED_CHIP_ENABLE_MASK |
			AW2015_LED_CHARGE_DISABLE_MASK);
	dm_i2c_reg_write(led_devp, AW2015_REG_IMAX, 2);
	dm_i2c_reg_write(led_devp, AW2015_REG_LEDCTR, 3);
	dm_i2c_reg_write(led_devp, AW2015_REG_LEDEN, 0b111);

	for(i = 0; i < 3; ++i) {
		dm_i2c_reg_write(led_devp, AW2015_REG_LCFG1 + i,
			AW2015_LED_ON_MODE_MASK);
		dm_i2c_reg_write(led_devp, AW2015_REG_PWM1 + i,
			cal_settings[i]);
		dm_i2c_reg_write(led_devp, AW2015_REG_ILED1 + i,
			cal_settings[3+i]);
        }
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

unsigned int get_dram_size(void)
{
	// >>16 -> MB, <<20 -> real size, so >>16<<20 = <<4
#if defined(CONFIG_SYS_MEM_TOP_HIDE)
	return (((readl(SYSCTRL_SEC_STATUS_REG4)) & 0xFFFF0000) << 4) - CONFIG_SYS_MEM_TOP_HIDE;
#else
	return (((readl(SYSCTRL_SEC_STATUS_REG4)) & 0xFFFF0000) << 4);
#endif /* CONFIG_SYS_MEM_TOP_HIDE */
}


int dram_init_banksize(void)
{
	gd->bd->bi_dram[0].start = 0;
	gd->bd->bi_dram[0].size = get_dram_size();
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
	unsigned int value;

	printf("board init\n");

	/* set GPIOB_3 to low before emmc init */
	value = readl(PADCTRL_PIN_MUX_REG3);
	/* set to gpio pin */
	writel(value & ~(0xf << 12), PADCTRL_PIN_MUX_REG3);
	value = readl(PADCTRL_GPIOB_OEN);
	/* enable gpio output */
	writel(value & ~(0x1 << 3), PADCTRL_GPIOB_OEN);
	value = readl(PADCTRL_GPIOB_O);
	/* set output value to low */
	writel(value & ~(0x1 << 3), PADCTRL_GPIOB_O);

	//Please keep try usb boot first in board_init, as other init before usb may cause burning failure
#if defined(CONFIG_AML_V3_FACTORY_BURN) && defined(CONFIG_AML_V3_USB_TOOl)
	if ((0x1b8ec003 != readl(SYSCTRL_SEC_STICKY_REG2)) && (0x1b8ec004 != readl(SYSCTRL_SEC_STICKY_REG2)))
	{ aml_v3_factory_usb_burning(0, gd->bd); }
#endif//#if defined(CONFIG_AML_V3_FACTORY_BURN) && defined(CONFIG_AML_V3_USB_TOOl)

	pinctrl_devices_active(PIN_CONTROLLER_NUM);
	active_clk();
	sys_led_init();

	return 0;
}

int board_late_init(void)
{
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
	printf("\nmach_cpu_init\n");
	return 0;
}

int ft_board_setup(void *blob, bd_t *bd)
{
	/* eg: bl31/32 rsv */
	return 0;
}

int do_get_hw_id(cmd_tbl_t *cmdtp, int flag, int argc,
		char * const argv[])
{
	unsigned int hw_id = 0, ret = 0;
	char hw_id_str[8] = {0};  // eg: 0x0A

	/*
	 * HWID_4 : GPIOD_9
	 * HWID_3 : GPIOD_8
	 * HWID_2 : GPIOD_7
	 * HWID_1 : GPIOD_6
	 * HWID_0 : GPIOD_5
	 *
	 */
	/* disable gpio pull */
	ret = readl(PADCTRL_GPIOD_PULL_EN);
	writel(ret & (~(0x1F << 5)), PADCTRL_GPIOD_PULL_EN);

	/* pin mux to gpio pin */
	/* GPIOD_5, GPIOD_6, GPIOD_7 */
	ret = readl(PADCTRL_PIN_MUX_REG0);
	writel(ret & (~(0xFFF << 20)), PADCTRL_PIN_MUX_REG0);
	/* GPIOD_8, GPIOD_9 */
	ret = readl(PADCTRL_PIN_MUX_REG1);
	writel(ret & (~(0xFF << 0)), PADCTRL_PIN_MUX_REG1);

	/* enable input */
	ret = readl(PADCTRL_GPIOD_OEN);
	writel(ret | (0x1F << 5), PADCTRL_GPIOD_OEN);

	/* read hw id */
	ret = readl(PADCTRL_GPIOD_I);
	hw_id = (ret >> 5) & 0x1F;

	snprintf(hw_id_str, sizeof(hw_id_str), "0x%02x", hw_id);
	env_set("hw_id", hw_id_str);
	return 0;
}

U_BOOT_CMD(
	get_hw_id, 1, 0, do_get_hw_id, "get HW_ID and env_set 'hw_id'\n", "get_hw_id"
);

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
