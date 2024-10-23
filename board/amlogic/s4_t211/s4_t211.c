// SPDX-License-Identifier: (GPL-2.0+ OR MIT)
/*
 * Copyright (c) 2019 Amlogic, Inc. All rights reserved.
 */

#include <blk.h>
#include <board_variant.h>
#include <chromecast/partition.h>
#include <chromecast/factory.h>
#include <common.h>
#include <asm/io.h>
#include <malloc.h>
#include <errno.h>
#include <emmc_partitions.h>
#include <environment.h>
#include <stdio.h>
#include <asm/arch/secure_apb.h>
#include <asm/arch/pinctrl_init.h>
#include <asm-generic/gpio.h>
#include <dm.h>
#include <asm/armv8/mmu.h>
#include <amlogic/aml_v3_burning.h>
#include <amlogic/aml_v2_burning.h>
#include <amlogic/aml_efuse.h>
#include <amlogic/storage.h>
#include <asm/arch/bl31_apis.h>
#include <asm/gpio.h>

#include "partition_table.h"

#ifdef CONFIG_AML_VPU
#include <amlogic/media/vpu/vpu.h>
#endif
#ifdef CONFIG_AML_VPP
#include <amlogic/media/vpp/vpp.h>
#endif
#ifdef CONFIG_AML_HDMITX20
#include <amlogic/media/vout/hdmitx/hdmitx_module.h>
#endif
#ifdef CONFIG_AML_CVBS
#include <amlogic/media/vout/aml_cvbs.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

extern unsigned char res_images_logo_bmp_gz[];
extern unsigned int res_images_logo_bmp_gz_len;

#define BOOT_INDEX_EMMC_USER	0x10
#define BOOT_INDEX_EMMC_BOOT0	0x11
#define BOOT_INDEX_EMMC_BOOT1	0x12
#define BOOT_INDEX_USB		0x50
#define GPIOZ_4 "GPIOZ_4"
#define GPIOZ_6 "GPIOZ_6"
#define GPIOZ_7 "GPIOZ_7"

static int get_boot_index(void)
{
	return readl(SYSCTRL_SEC_STATUS_REG2) & 0xff;
}

int get_board_variant(void)
{
#ifdef CONFIG_DEBUG_BOARD_VARIANT_PROD
	return BOARD_VARIANT_PROD;
#else
	static int board_variant = BOARD_VARIANT_UNKNOWN;

	if (board_variant == BOARD_VARIANT_UNKNOWN) {
		if (IS_FEAT_DIS_NORMAL_DEVICE_ROOTCERT_0() == 0 &&
		    IS_FEAT_DIS_DFU_DEVICE_ROOTCERT_0() == 0)
			board_variant = BOARD_VARIANT_DEV;
		else
			board_variant = BOARD_VARIANT_PROD;
	}

	return board_variant;
#endif
}

int dram_init(void)
{
	gd->ram_size = PHYS_SDRAM_1_SIZE;
	return 0;
}

#ifdef CONFIG_AML_HDMITX20
static void hdmitx_set_hdmi_5v(void)
{
	/*Power on VCC_5V for HDMI_5V*/
	run_command("gpio set GPIOH_7", 0);
}
#endif

void board_init_mem(void) {
	#if 1
	/* config bootm low size, make sure whole dram/psram space can be used */
	phys_size_t ram_size;
	char *env_tmp;
	env_tmp = env_get("bootm_size");
	if (!env_tmp) {
		ram_size = (((readl(SYSCTRL_SEC_STATUS_REG4)) & 0xFFF80000) << 4);
		env_set_hex("bootm_low", 0);
		env_set_hex("bootm_size", ram_size);
	}
	#endif
}

int board_init(void)
{
	printf("board init\n");

	/* The non-secure watchdog is enabled in BL2 TEE, disable it */
	run_command("watchdog off", 0);
	printf("watchdog disable\n");

	aml_set_bootsequence(0);
	pinctrl_devices_active(PIN_CONTROLLER_NUM);
#ifdef CONFIG_AML_HDMITX20
	hdmitx_set_hdmi_5v();
	hdmitx_init();
#endif
	return 0;
}

int read_gpio(const char *str_gpio)
{
	unsigned int gpio;
	int ret;
	ret = gpio_lookup_name(str_gpio, NULL, NULL, &gpio);
	if (ret) {
		printf("GPIO: '%s' not found\n", str_gpio);
		return -1;
	}
	/* grab the pin before we tweak it */
	ret = gpio_request(gpio, "cmd_gpio");
	if (ret && ret != -EBUSY) {
		printf("gpio: requesting pin %u failed\n", gpio);
		return -1;
	}
	gpio_direction_input(gpio);
	int value = gpio_get_value(gpio);
	gpio_free(gpio);
	return value;
}

void env_set_board_id(void)
{
	// See go/boreal-pinmux.
	int hw_id[] = {read_gpio(GPIOZ_6), read_gpio(GPIOZ_7), read_gpio(GPIOZ_4)};
	int hw_id_length = (int) (sizeof(hw_id) / sizeof(hw_id[0]));
	int board_id = 0;

	for (int i = 0; i < hw_id_length; i++)
	{
		if (hw_id[i] < 0) {
			printf("Failed to read board id from hw_id[%d]\n", i);
			return;
		}
		board_id += (hw_id[i] << i);
	}

	printf("Store board id: %d\n", board_id);
	env_set_ulong("board_id", board_id);
}

int board_late_init(void)
{
	printf("board late init\n");

	char *env_fstab_suffix;
	env_fstab_suffix = env_get("fstab_suffix");
	if (!env_fstab_suffix) {
		env_set("fstab_suffix", "ext4");
	}

	run_command("defenv_reserv", 0);

	void *bootloader_buf = NULL;
	int boot_idx = get_boot_index();

	printf("%s: boot_idx: %#x\n", __func__, boot_idx);

	if (boot_idx == BOOT_INDEX_EMMC_USER) {
		// Migrate from AML partition table to GPT
		// The boot address in the user partition will be occupied by GPT
		bootloader_buf = malloc(CHROMECAST_BOOTLOADER_SIZE);
		if (!bootloader_buf) {
			printf("%s: unable to allocate memory for bootloader_buf\n",
			       __func__);
		} else {
			printf("%s: copy bootloader to boot0 and boot1\n",
			       __func__);
			store_boot_read("bootloader", 0,
					CHROMECAST_BOOTLOADER_SIZE,
					bootloader_buf);
			store_boot_write("bootloader", 1,
					 CHROMECAST_BOOTLOADER_SIZE,
					 bootloader_buf);
			store_boot_write("bootloader", 2,
					 CHROMECAST_BOOTLOADER_SIZE,
					 bootloader_buf);
		}
	}

	struct blk_desc *dev_desc = NULL;
	dev_desc = blk_get_dev("mmc", 1);
	if (dev_desc) {
		init_chromecast_partitions(dev_desc, &boreal_partition_table);
		load_chromecast_factory_configs(dev_desc);
	} else {
		printf("%s: failed to get mmc 1\n", __func__);
	}

	// reinit aml mmc partitions
	mmc_device_init(find_mmc_device(1));

	if (bootloader_buf) {
		printf("%s: copy bootloader to bootloader_a/b\n", __func__);
		store_write("bootloader_a", 0,
			    CHROMECAST_BOOTLOADER_SIZE, bootloader_buf);
		store_write("bootloader_b", 0,
			    CHROMECAST_BOOTLOADER_SIZE, bootloader_buf);
		free(bootloader_buf);
	}

	env_set_board_id();

	// The env partition could be changed, save the env.
	env_save();

	env_set_hex("logo_addr", (ulong) res_images_logo_bmp_gz);

	if (get_board_variant() != BOARD_VARIANT_DEV) {
		// set bootdelay to -2 to not check for abort
		env_set("bootdelay", "-2");
		env_set("consoleargs", "console=ttynull");
		env_set("board_variant", "prod");
	} else {
		// autoboot with no delay but can be stopped by key input
		env_set("bootdelay", "0");
		env_set("consoleargs", "console=ttyS0,921600 no_console_suspend earlycon=aml-uart,0xfe07a000");
		env_set("board_variant", "dev");
	}

	if (boot_idx == BOOT_INDEX_USB) {
		run_command("fastboot 0", 0);
	}

	board_init_mem();
	run_command("run bcb_cmd", 0);

#ifdef CONFIG_AML_VPU
	vpu_probe();
#endif
#ifdef CONFIG_AML_VPP
	vpp_init();
#endif
#ifdef CONFIG_AML_CVBS
	cvbs_init();
#endif
	run_command("amlsecurecheck", 0);
	run_command("update_tries", 0);
	return 0;
}


phys_size_t get_effective_memsize(void)
{
	// >>16 -> MB, <<20 -> real size, so >>16<<20 = <<4
#if defined(CONFIG_SYS_MEM_TOP_HIDE)
	return ((readl(SYSCTRL_SEC_STATUS_REG4) & 0xFFF80000) << 4) > 0xf1000000 ? 0xf1000000 : \
			(((readl(SYSCTRL_SEC_STATUS_REG4) & 0xFFF80000) << 4) - CONFIG_SYS_MEM_TOP_HIDE);
#else
	return ((readl(SYSCTRL_SEC_STATUS_REG4) & 0xFFF80000) << 4) > 0xf1000000 ? 0xf1000000 : \
			((readl(SYSCTRL_SEC_STATUS_REG4) & 0xFFF80000) << 4);
#endif /* CONFIG_SYS_MEM_TOP_HIDE */

}

static struct mm_region bd_mem_map[] = {
	{
		.virt = 0x00000000UL,
		.phys = 0x00000000UL,
		.size = 0x80000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) |
			 PTE_BLOCK_INNER_SHARE
	}, {
		.virt = 0xf1000000UL,
		.phys = 0xf1000000UL,
		.size = 0x0f000000UL,
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
	/* update mmu table from bl2 ddr auto detect size */
#ifdef CONFIG_UPDATE_MMU_TABLE
	unsigned long nddrSize = ((readl(SYSCTRL_SEC_STATUS_REG4) & 0xFFF80000) << 4) > 0xf1000000 ? 0xf1000000 :  \
			((readl(SYSCTRL_SEC_STATUS_REG4) & 0xFFF80000) << 4);
	bd_mem_map[0].size = nddrSize;
#endif

	//printf("\nmach_cpu_init\n");
	return 0;
}

int ft_board_setup(void *blob, bd_t *bd)
{
	/* eg: bl31/32 rsv */
	return 0;
}

const char * const _env_args_reserve_[] =
{
	"lock",
	"fstab_suffix",
	"bootloader_version",
	"hdr_policy",
	"hdr_priority",
	"hdmichecksum",
	"hdmimode",
	"outputmode",
	"colorattribute",
	"hdmi_colorspace",
	"hdmi_colordepth",
	"1080p60hz_deepcolor",
	"1080p50hz_deepcolor",
	"1080p30hz_deepcolor",
	"1080p25hz_deepcolor",
	"1080p24hz_deepcolor",
	"720p60hz_deepcolor",
	"720p50hz_deepcolor",
	"1080i60hz_deepcolor",
	"1080i50hz_deepcolor",
	"576p50hz_deepcolor",
	"480p60hz_deepcolor",
	"digitaudiooutput",
	"is.bestmode",
	"user_colorattribute",
	"user_hdr_type",
	"dolby_status",
	"dv_enable",
	"user_prefer_dv_type",
	"hdr_force_mode",
	"hdr_preferred_policy",
	"frac_rate_policy",
	"bestcolorspace",
	"framerate_priority",
	"hdr_resolution_priority",
	NULL//Keep NULL be last to tell END
};
