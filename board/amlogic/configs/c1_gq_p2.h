/* SPDX-License-Identifier: (GPL-2.0+ OR MIT) */
/*
 * Copyright (c) 2019 Amlogic, Inc. All rights reserved.
 */

#ifndef __BOARD_CFG_H__
#define __BOARD_CFG_H__

#include <asm/arch/cpu.h>

/* UBOOT Facotry usb burning config */
#ifndef AML_DISABLE_UPDATE_MODE
#define CONFIG_AML_V3_FACTORY_BURN 1
#define CONFIG_AML_V3_USB_TOOl 1
#define CONFIG_USB_GADGET_DOWNLOAD 1
#define CONFIG_FASTBOOT_DWC_PCD 1
#define CONFIG_USB_DEVICE_V2 1
#endif

/*
 * platform power init config
 */

#define AML_VCCK_INIT_VOLTAGE	  980 	    //VCCK power up voltage
#define AML_VDDEE_INIT_VOLTAGE    811       // VDDEE power up voltage

/* SMP Definitinos */
#define CPU_RELEASE_ADDR		secondary_boot_func

/* Bootloader Control Block function
   That is used for recovery and the bootloader to talk to each other
  */

/* Serial config */
#define CONFIG_CONS_INDEX 2
#define CONFIG_BAUDRATE  115200

/*config the default parameters for adc power key*/
#define AML_ADC_POWER_KEY_CHAN   2  /*channel range: 0-7*/
#define AML_ADC_POWER_KEY_VAL    0  /*sample value range: 0-1023*/

#ifdef AML_ENABLE_PRODUCTION_MODE
#define JUDGE_MODE "echo this is production mode;"
#define CONFIG_AML_PRODUCT_MODE 1
#ifdef CONFIG_MESON_SERIAL
#define CONFIG_DISABLE_AML_SERIAL 1
#endif //CONFIG_MESON_SERIAL
#define USB_BURNING_OPTION "echo productmode"
#else  //AML_ENABLE_PRODUCTION_MODE
#define JUDGE_MODE "echo this is not production mode;"
#define CONFIG_CMD_BOOTD 1
#if (defined(CONFIG_ARM) && !defined(CONFIG_ARM64))
#define CONFIG_CMD_BOOTZ 1
#endif //defined(CONFIG_ARM) && !defined(CONFIG_ARM64)
#ifdef CONFIG_ARM64
#define CONFIG_CMD_BOOTI 1
#endif //CONFIG_ARM64
#define CONFIG_CMD_MEMORY 1
#define CONFIG_CMD_SOURCE 1
#ifdef CONFIG_MISC
#define CONFIG_CMD_JTAG 1
#endif //CONFIG_MISC
#ifdef CONFIG_CMD_BOOTI
#define CONFIG_CMD_LZMADEC 1
#define CONFIG_CMD_UNZIP 1
#define CONFIG_LZMA 1
#endif //CONFIG_CMD_BOOTI
#define USB_BURNING_OPTION "adnl"

#endif  //AML_ENABLE_PRODUCTION_MODE

/* args/envs */
#define CONFIG_SYS_MAXARGS  64
#define CONFIG_EXTRA_ENV_SETTINGS \
        "firstboot=1\0"\
        "jtag=disable\0"\
        "loadaddr=0x00020000\0"\
        "os_ident_addr=0x00500000\0"\
        "loadaddr_rtos=0x00001000\0"\
        "loadaddr_kernel=0x07000000\0"\
        "loadaddr_dspa=0x06000000\0"\
        "ringtone_addr=0x06800000\0"\
        "dspfw_size=0x200000\0"\
        "ringtone_size=0x100000\0"\
        "otg_device=1\0" \
        "usb_burning=" USB_BURNING_OPTION "\0" \
        "fdt_high=0x10000000\0"\
        "active_slot=_a\0"\
        "boot_part=boot_a\0"\
        "Irq_check_en=0\0"\
        /* partition number in hex */\
        "ringtone_part=1:D\0"\
        "fatload_dev=usb\0"\
        "fs_type=""rootfstype=ramfs""\0"\
        "initargs="\
            "\0"\
        "run_dspa="\
            "unzip 0x6400000 ${loadaddr_dspa};"\
            "if ext4load mmc ${ringtone_part} ${ringtone_addr} quartz_dsp_bootpackage_v4.bin; then ;"\
            "else echo ringtone file not found!;"\
            "fi;"\
            "flushmemory ${ringtone_addr} ${ringtone_size};"\
            "dspset 0 1 1;dsprun 0 ${loadaddr_dspa} ${dspfw_size};"\
            "\0"\
        "storeargs="\
            "get_rebootmode;"\
            "get_wake_args;"\
            "get_chiptype;"\
            "get_cpu_rev;"\
            "if test ${reboot_mode} = fastboot; then "\
                "setenv reboot_mode warm_reboot;"\
            "fi;"\
            "setenv bootargs ${initargs} otg_device=${otg_device} "\
                "gpt "\
                "irq_check_en=${Irq_check_en} "\
                "hw_id=${hw_id} "\
                "androidboot.reboot_mode=${reboot_mode} "\
                "chip_type=${chip_type} "\
                "cpu_rev=${cpu_rev} "\
                "androidboot.hardware="__stringify(BOARD_NAME)" "\
                "androidboot.slot_suffix=${active_slot} "\
                "androidboot.bl2_boot_part=${bl2_part} "\
                "androidboot.firstboot=${firstboot} "\
                "${wake_args} "\
                "jtag=${jtag};"\
            "setenv bootargs ${bootargs};"\
            "\0"\
        "switch_bootmode="\
            "get_rebootmode;"\
            "if test ${reboot_mode} = factory_boot; then " \
                    "setenv loadaddr ${loadaddr_kernel};"\
                    "if imgread kernel system_b ${loadaddr}; then " \
                        "bootm ${loadaddr};"\
                    "fi;" \
                    "run update;"\
            "else if test ${reboot_mode} = update; then "\
                    "run update;"\
            "else if test ${reboot_mode} = quiescent; then "\
                    "setenv bootargs ${bootargs} androidboot.quiescent=1;"\
            "else if test ${reboot_mode} = cold_boot; then "\
            "fi;fi;fi;fi;"\
            "\0" \
        "storeboot="\
            JUDGE_MODE \
            "setenv loadaddr ${loadaddr_kernel};"\
            "if imgread kernel ${boot_part} ${loadaddr}; then bootm ${loadaddr}; fi;"\
            "echo try upgrade as booting failure; run update;"\
            "\0" \
         "update="\
            /*first usb burning, then udisk recovery(for factory)*/\
            "run usb_burning; "\
            "run recovery_from_udisk;"\
            "\0"\
        "recovery_from_fat_dev="\
            "setenv loadaddr ${loadaddr_kernel};"\
            "if fatload ${fatload_dev} 0 ${loadaddr} recovery.img 0x10000000 0; then "\
                "bootm ${loadaddr};fi;"\
            "\0"\
        "recovery_from_udisk="\
            "setenv fatload_dev usb;"\
            "if usb start 0; then run recovery_from_fat_dev; fi;"\
            "\0"\
        "bcb_cmd="\
            "get_valid_slot;"\
            "\0"\
	"get_hw_id=" \
	    "get_board_hw_id;" \
	    "\0" \
        "oqc_check="\
            "setenv test_mode false;"\
            "setenv fatload_dev usb;"\
            "if usb start 0; then "\
                "if fatload ${fatload_dev} 0 ${loadaddr} OQC.txt 0x100000 0; then "\
                    "setenv test_mode true;"\
                "fi;"\
            "fi;"\
            "setenv bootargs ${bootargs} androidboot.test_mode=${test_mode};"\
            "\0"\
        "upgrade_and_oqc_key_check="\
            "if gpio input GPIOX_11; then "\
		"setenv boot_external_image 1;"\
                "run recovery_from_udisk;"\
		"setenv boot_external_image 0;"\
                "run oqc_check;"\
            "fi;"\
            "\0"\

#ifdef CONFIG_G_AB_SYSTEM
#define CONFIG_PREBOOT  \
            "run bcb_cmd; "\
            "run run_dspa; "\
            "run get_hw_id;" \
            "run storeargs;"\
            "run upgrade_and_oqc_key_check;"\
            "run switch_bootmode;"

#else
#define CONFIG_PREBOOT  \
            "run get_hw_id;" \
            "run storeargs;"\
            "run upgrade_key;"\
            "run switch_bootmode;"
#endif

/* #define CONFIG_ENV_IS_NOWHERE  1 */
#define CONFIG_ENV_SIZE   (8*1024)
#define CONFIG_FIT 1
#define CONFIG_OF_LIBFDT 1
#define CONFIG_ANDROID_BOOT_IMAGE 1
#define CONFIG_SYS_BOOTM_LEN (64<<20) /* Increase max gunzip size*/

/* ATTENTION */
/* DDR configs move to board/amlogic/[board]/firmware/timing.c */

/* running in sram */
//#define UBOOT_RUN_IN_SRAM
#ifdef UBOOT_RUN_IN_SRAM
#define CONFIG_SYS_INIT_SP_ADDR				(0x00200000)
/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN				(256*1024)
#else
#define CONFIG_SYS_INIT_SP_ADDR				(0x00200000)
#define CONFIG_SYS_MALLOC_LEN				(64*1024*1024)
#endif

//#define CONFIG_NR_DRAM_BANKS			1
/* ddr functions */
#define DDR_FULL_TEST            0 //0:disable, 1:enable. ddr full test
#define DDR_LOW_POWER            0 //0:disable, 1:enable. ddr clk gate for lp
#define DDR_ZQ_PD                0 //0:disable, 1:enable. ddr zq power down
#define DDR_USE_EXT_VREF         0 //0:disable, 1:enable. ddr use external vref
#define DDR4_TIMING_TEST         0 //0:disable, 1:enable. ddr4 timing test function
#define DDR_PLL_BYPASS           0 //0:disable, 1:enable. ddr pll bypass function

/* storage: emmc/nand/sd */
#define CONFIG_ENV_OVERWRITE
//#define CONFIG_MESON_NFC
/* #define 	CONFIG_CMD_SAVEENV */
/* fixme, need fix*/

#if (defined(CONFIG_ENV_IS_IN_AMLNAND) || defined(CONFIG_ENV_IS_IN_MMC)) && defined(CONFIG_STORE_COMPATIBLE)
#error env in amlnand/mmc already be compatible;
#endif

/*
*				storage
*		|---------|---------|
*		|					|
*		emmc<--Compatible-->nand
*					|-------|-------|
*					|				|
*					MTD<-Exclusive->NFTL
*/
/* axg only support slc nand */
/* swither for mtd nand which is for slc only. */


#if defined(CONFIG_AML_NAND) && defined(CONFIG_MESON_NFC)
#error CONFIG_AML_NAND/CONFIG_MESON_NFC can not support at the sametime;
#endif

#if defined(CONFIG_SPI_NAND) && defined(CONFIG_MESON_NFC)
#error CONFIG_SPI_NAND/CONFIG_MESON_NFC can not support at the sametime;
#endif

/* #define		CONFIG_AML_SD_EMMC 1 */
#ifdef		CONFIG_AML_SD_EMMC
	#define 	CONFIG_GENERIC_MMC 1
	#define 	CONFIG_CMD_MMC 1
	#define CONFIG_CMD_GPT 1
	#define	CONFIG_SYS_MMC_ENV_DEV 1
	#define CONFIG_EMMC_DDR52_EN 0
	#define CONFIG_EMMC_DDR52_CLK 35000000
#endif
#define		CONFIG_PARTITIONS 1
#if 0
#define 	CONFIG_SYS_NO_FLASH  1
#endif

#if defined CONFIG_MESON_NFC || defined CONFIG_SPI_NAND
	#define CONFIG_CMD_NAND 1
	#define CONFIG_MTD_DEVICE 1
	/* #define CONFIG_RBTREE */
	#define CONFIG_CMD_NAND_TORTURE 1
	#define CONFIG_CMD_MTDPARTS   1
	#define CONFIG_MTD_PARTITIONS 1
	#define CONFIG_SYS_MAX_NAND_DEVICE  2
	#define CONFIG_SYS_NAND_BASE_LIST   {0}
#endif

/* vpu */
#define AML_VPU_CLK_LEVEL_DFT 7

/* osd */
#define OSD_SCALE_ENABLE
#define AML_OSD_HIGH_VERSION

/* USB
 * Enable CONFIG_MUSB_HCD for Host functionalities MSC, keyboard
 * Enable CONFIG_MUSB_UDD for Device functionalities.
 */
/* #define CONFIG_MUSB_UDC		1 */
/* #define CONFIG_CMD_USB 1 */

#define USB_PHY2_PLL_PARAMETER_1	0x09400414
#define USB_PHY2_PLL_PARAMETER_2	0x927e0000
#define USB_PHY2_PLL_PARAMETER_3	0xAC5F49E5

#define USB_G12x_PHY_PLL_SETTING_1	(0xfe18)
#define USB_G12x_PHY_PLL_SETTING_2	(0xfff)
#define USB_G12x_PHY_PLL_SETTING_3	(0x78000)
#define USB_G12x_PHY_PLL_SETTING_4	(0xe0004)
#define USB_G12x_PHY_PLL_SETTING_5	(0xe000c)

#define AML_TXLX_USB        1
#define AML_USB_V2             1
#define USB_GENERAL_BIT         3
#define USB_PHY21_BIT           4

/* other devices */

/* commands */

/*file system*/
#define CONFIG_DOS_PARTITION 1
#define CONFIG_EFI_PARTITION 1
/* #define CONFIG_MMC 1 */
#define CONFIG_FS_FAT 1
#define CONFIG_FS_EXT4 1

/* Cache Definitions */
/* #define CONFIG_SYS_DCACHE_OFF */
/* #define CONFIG_SYS_ICACHE_OFF */

/* other functions */
#define CONFIG_LIBAVB		1
/* top hide for NBG file */
#define CONFIG_SYS_MEM_TOP_HIDE		0x00400000

#define CONFIG_CPU_ARMV8

/* #define CONFIG_MULTI_DTB    1 */

/* support secure boot */
#define CONFIG_AML_SECURE_UBOOT   1

#if defined(CONFIG_AML_SECURE_UBOOT)

/* unify build for generate encrypted bootloader "u-boot.bin.encrypt" */
#define CONFIG_AML_CRYPTO_UBOOT   1

/* unify build for generate encrypted kernel image
   SRC : "board/amlogic/(board)/boot.img"
   DST : "fip/boot.img.encrypt" */
/* #define CONFIG_AML_CRYPTO_IMG       1 */

#endif /* CONFIG_AML_SECURE_UBOOT */

#define CONFIG_FIP_IMG_SUPPORT  1
#define CONFIG_AML_KASLR_SEED

#endif

