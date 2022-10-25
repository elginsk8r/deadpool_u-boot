/*
 * board/amlogic/a1_korlan_p2/a1_korlan_p2
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
#include <linux/libfdt.h>
#include <amlogic/cpu_id.h>
#include <asm/arch/secure_apb.h>
#include <asm/arch/pinctrl_init.h>
#include <linux/sizes.h>
#include <asm-generic/gpio.h>
#include <dm.h>
#include <asm/armv8/mmu.h>
#include <amlogic/aml_v3_burning.h>
#include <linux/mtd/partitions.h>

DECLARE_GLOBAL_DATA_PTR;

void sys_led_init(void)
{
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

int active_a1_clk(void)
{
	struct udevice *a1_clk = NULL;
	int err;

	err = uclass_get_device_by_name(UCLASS_CLK,
			"xtal-clk", &a1_clk);
	if (err) {
		pr_err("Can't find xtal-clk clock (%d)\n", err);
		return err;
	}
	err = uclass_get_device_by_name(UCLASS_CLK,
			"clock-controller@0", &a1_clk);
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
	/* reset uart A for BT*/
	writel(0x4000000, RESETCTRL_RESET1);
	//Please keep try usb boot first in board_init, as other init before usb may cause burning failure
#if defined(CONFIG_AML_V3_FACTORY_BURN) && defined(CONFIG_AML_V3_USB_TOOl)
	if ((0x1b8ec003 != readl(SYSCTRL_SEC_STICKY_REG2)) && (0x1b8ec004 != readl(SYSCTRL_SEC_STICKY_REG2)))
	{ aml_v3_factory_usb_burning(0, gd->bd); }
#endif//#if defined(CONFIG_AML_V3_FACTORY_BURN) && defined(CONFIG_AML_V3_USB_TOOl)

	pinctrl_devices_active(PIN_CONTROLLER_NUM);
	active_a1_clk();

	return 0;
}

int board_late_init(void)
{
	pr_info("board late init\n");
	board_init_mem();

	//auto enter usb mode after board_late_init if 'adnl.exe setvar burnsteps 0x1b8ec003'
#if defined(CONFIG_AML_V3_FACTORY_BURN) && defined(CONFIG_AML_V3_USB_TOOl)
	if (0x1b8ec003 == readl(SYSCTRL_SEC_STICKY_REG2))
	{ aml_v3_factory_usb_burning(0, gd->bd); }
#endif//#if defined(CONFIG_AML_V3_FACTORY_BURN) && defined(CONFIG_AML_V3_USB_TOOl)

	/*GPIOP_10: USB_SEL set output-high by default*/
	gpio_option("GPIOP_10", GPIOD_IS_OUT, true);
	/*GPIOP_1: output-low by default*/
	gpio_option("GPIOP_1", GPIOD_IS_OUT, false);
	/*GPIOP_2: input enable*/
	gpio_option("GPIOP_2", GPIOD_IS_IN, false);
	/*GPIOA_6: output-high*/
	gpio_option("GPIOA_6", GPIOD_IS_OUT, true);
	/*led control: output low, turn on led*/
	gpio_option("GPIOP_9", GPIOD_IS_OUT, false);
	gpio_option("GPIOF_11", GPIOD_IS_OUT, false);
	gpio_option("GPIOP_12", GPIOD_IS_OUT, false);
	/*GPIOP_5: SOC_POGO_USB_EN, default is high*/
	gpio_option("GPIOP_5", GPIOD_IS_OUT, true);

	return 0;
}

phys_size_t get_dram_size(void)
{
	// >>16 -> MB, <<20 -> real size, so >>16<<20 = <<4
#if defined(CONFIG_SYS_MEM_TOP_HIDE)
	return (((readl(SYSCTRL_SEC_STATUS_REG4)) & 0xFFFF0000) << 4) - CONFIG_SYS_MEM_TOP_HIDE;
#else
	return (((readl(SYSCTRL_SEC_STATUS_REG4)) & 0xFFFF0000) << 4);
#endif /* CONFIG_SYS_MEM_TOP_HIDE */
}

phys_size_t get_effective_memsize(void)
{
#ifdef CONFIG_UBOOT_RUN_IN_SRAM
	return 0x180000; /* SRAM 1.5MB */
#else
	return get_dram_size();
#endif /* CONFIG_UBOOT_RUN_IN_SRAM */
}

#ifdef CONFIG_UBOOT_RUN_IN_SRAM
ulong board_get_usable_ram_top(ulong total_size)
{
	return (PHYS_SDRAM_1_BASE + PHYS_SDRAM_1_SIZE);
}
#endif

int dram_init_banksize(void)
{
	gd->bd->bi_dram[0].start = 0;
	gd->bd->bi_dram[0].size = get_dram_size();
	return 0;
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
		.size = 0x7FE00000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
			 PTE_BLOCK_NON_SHARE |
			 PTE_BLOCK_PXN | PTE_BLOCK_UXN
	}, {
		.virt = 0xFFE00000UL,
		.phys = 0xFFE00000UL,
		.size = 0x00200000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) |
			 PTE_BLOCK_INNER_SHARE
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

int do_get_board_hw_id(cmd_tbl_t *cmdtp, int flag, int argc,
		char * const argv[])
{
	unsigned int hw_id = 0, ret = 0;
	char hw_id_str[8] = {0};  // eg: 0x0A

	/*
	 * HWID_4 : GPIOX_4
	 * HWID_3 : GPIOX_3
	 * HWID_2 : GPIOX_2
	 * HWID_1 : GPIOX_1
	 * HWID_0 : GPIOX_0
	 *
	 */
	/* pin mux to gpio pin */
	ret = readl(PADCTRL_PIN_MUX_REG3);
	writel(ret & (~(0xFFFFF << 0)), PADCTRL_PIN_MUX_REG3);

	/* enable input */
	ret = readl(PADCTRL_GPIOX_OEN);
	writel(ret | (0x1F << 0), PADCTRL_GPIOX_OEN);

	/* disable gpio pull */
	ret = readl(PADCTRL_GPIOX_PULL_EN);
	writel(ret & (~(0x1F << 0)), PADCTRL_GPIOX_PULL_EN);

	/* read hw id */
	ret = readl(PADCTRL_GPIOX_I);
	hw_id = (ret >> 0) & 0x1F;

	snprintf(hw_id_str, sizeof(hw_id_str), "0x%02x", hw_id);
	env_set("hw_id", hw_id_str);
	return 0;
}

U_BOOT_CMD(
	get_board_hw_id, 1, 0, do_get_board_hw_id,
	"get GQ/NQ HW_ID and env_set 'hw_id'\n",
	"get_board_hw_id"
);

/* partition table */
/* partition table for spinand flash */
#if (defined(CONFIG_SPI_NAND) || defined(CONFIG_MTD_SPI_NAND))
static const struct mtd_partition spinand_partitions[] = {
	{
		.name = "fts",
		.offset = 0,
		.size = 1 * SZ_1M,
	},
	{
		.name = "factory",
		.offset = 0,
		.size = 4 * SZ_1M,
	},
	{
		.name = "recovery",
		.offset = 0,
		.size = 12 * SZ_1M,
	},
	{
		.name = "boot",
		.offset = 0,
		.size = 12 * SZ_1M,
	},
	{
		.name = "system",
		.offset = 0,
		.size = 30 * SZ_1M,
	},
	/* last partition get the rest capacity */
	{
		.name = "cache",
		.offset = MTDPART_OFS_APPEND,
		.size = MTDPART_SIZ_FULL,
	}
};
const struct mtd_partition *get_partition_table(int *partitions)
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
