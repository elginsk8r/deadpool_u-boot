/* SPDX-License-Identifier: (GPL-2.0+ OR MIT) */
/*
 * Copyright (c) 2019 Amlogic, Inc. All rights reserved.
 */

#ifndef __BOARD_CFG_H__
#define __BOARD_CFG_H__

#include <asm/arch/cpu.h>

/*
 * platform power init config
 */

#define AML_VCCK_INIT_VOLTAGE	  799	    //VCCK power up voltage
#define AML_VDDEE_INIT_VOLTAGE    800       // VDDEE power up voltage

/*y_led brightness val*/
#define YELLOW_LED_VAL 0x00090013

/* SMP Definitinos */
#define CPU_RELEASE_ADDR		secondary_boot_func

/* Serial config */
#define CONFIG_CONS_INDEX 2
#define CONFIG_BAUDRATE  115200

/*low console baudrate*/
#define CONFIG_LOW_CONSOLE_BAUD			0

#define CONFIG_VENDOR_NAME    "Google"
#define CONFIG_PRODUCT_NAME   "Chromecast HD"

#ifdef CONFIG_DTB_BIND_KERNEL	//load dtb from kernel, such as boot partition
#define CONFIG_DTB_LOAD  "imgread dtb ${boot_part} ${dtb_mem_addr}"
#else
#define CONFIG_DTB_LOAD  "imgread dtb _aml_dtb ${dtb_mem_addr}"
#endif//#ifdef CONFIG_DTB_BIND_KERNEL	//load dtb from kernel, such as boot partition

#define CONFIG_VIDEO_BMP_GZIP
#define CONFIG_SYS_VIDEO_LOGO_MAX_SIZE (2 << 20)

/* args/envs */
#define CONFIG_SYS_MAXARGS  64
#define CONFIG_EXTRA_ENV_SETTINGS \
        "firstboot=1\0"\
        "silent=1\0"\
        "upgrade_step=0\0"\
        "jtag=disable\0"\
        "loadaddr=0x00020000\0"\
        "os_ident_addr=0x00500000\0"\
        "loadaddr_rtos=0x00001000\0"\
        "loadaddr_kernel=0x03080000\0"\
        "otg_device=1\0" \
        "panel_type=lcd_1\0" \
        "outputmode=1080p60hz\0" \
        "hdmimode=1080p60hz\0" \
        "cvbsmode=dummy_l\0" \
        "hdmitx_hpd_wait_ms=0\0" \
        "display_width=1920\0" \
        "display_height=1080\0" \
        "display_bpp=16\0" \
        "display_color_index=16\0" \
        "display_layer=osd0\0" \
        "display_color_fg=0xffff\0" \
        "display_color_bg=0\0" \
        "dtb_mem_addr=0x01000000\0" \
        "fb_addr=0x00300000\0" \
        "fb_width=1920\0" \
        "fb_height=1080\0" \
        "frac_rate_policy=1\0" \
        "fstab_suffix=f2fs\0" \
        "hdr_policy=1\0" \
        "usb_burning=adnl 1000\0" \
        "fdt_high=0x20000000\0"\
        "sdcburncfg=aml_sdc_burn.ini\0"\
        "EnableSelinux=enforcing\0" \
        "recovery_part=recovery\0"\
        "lock=10101000\0"\
        "recovery_offset=0\0"\
        "cvbs_drv=0\0"\
        "osd_reverse=0\0"\
        "video_reverse=0\0"\
        "active_slot=normal\0"\
        "boot_part=boot\0"\
        "vendor_boot_part=vendor_boot\0"\
        "board=boreal\0"\
        "Irq_check_en=0\0"\
        "common_dtb_load=" CONFIG_DTB_LOAD "\0"\
        "get_os_type=if store read ${os_ident_addr} ${boot_part} 0 0x1000; then os_ident ${os_ident_addr}; fi\0"\
        "fatload_dev=usb\0"\
        "fs_type=""rootfstype=ramfs""\0"\
        "initargs="\
            "init=/init "\
            "loop.max_part=4 "\
            "kvm-arm.mode=none "\
            "\0"\
        "storeargs="\
            "get_bootloaderversion;" \
            "setenv bootargs ${initargs} ${consoleargs} ${fs_type} otg_device=${otg_device} "\
                "logo=${display_layer},loaded,${fb_addr} vout=${outputmode},enable panel_type=${panel_type} "\
                "hdmitx=${cecconfig},${colorattribute} hdmimode=${hdmimode} "\
                "frac_rate_policy=${frac_rate_policy} hdmi_read_edid=${hdmi_read_edid} cvbsmode=${cvbsmode} "\
                "hdr_policy=${hdr_policy} hdr_priority=${hdr_priority} "\
                "osd_reverse=${osd_reverse} video_reverse=${video_reverse} irq_check_en=${Irq_check_en}  "\
                "androidboot.selinux=${EnableSelinux} androidboot.firstboot=${firstboot} jtag=${jtag}; "\
            "setenv bootargs ${bootargs} androidboot.bootloader=${bootloader_version} androidboot.hardware=amlogic;"\
            "setenv bootargs ${bootargs} " \
                "androidboot.board_variant=${board_variant} " \
                "androidboot.fstab_suffix=${fstab_suffix} " \
                "androidboot.serialno=${serial#} " \
                "mac_wifi=${mac_wifi} " \
                "androidboot.mac_wifi=${mac_wifi} " \
                "mac_bt=${mac_bt} " \
                "androidboot.btmacaddr=${mac_bt} " \
                "androidboot.hardware.sku=${sku} " \
                "androidboot.wificountrycode=${region_code} " \
                "androidboot.oem.locales=${locales} "\
                "androidboot.oem.key1=ATV00100022-${sku};" \
            "\0"\
        "switch_bootmode="\
            "get_rebootmode;"\
            "if test ${reboot_mode} = factory_reset; then "\
                    "run recovery_from_flash;"\
            "else if test ${reboot_mode} = update; then "\
                    "run update;"\
            "else if test ${reboot_mode} = quiescent; then "\
                    "setenv bootargs ${bootargs} androidboot.quiescent=1;"\
            "else if test ${reboot_mode} = recovery_quiescent; then "\
                    "setenv bootargs ${bootargs} androidboot.quiescent=1;"\
                    "run recovery_from_flash;"\
            "else if test ${reboot_mode} = cold_boot; then "\
            "else if test ${reboot_mode} = fastboot; then "\
                "fastboot 0;"\
            "fi;fi;fi;fi;fi;fi;"\
            "\0" \
        "storeboot="\
            "if gpio input GPIOD_10; then "\
                "echo FDR button pressed;"\
                /* Try to boot RMA image first, then the recovery */\
                "run boot_rma;"\
                "run recovery_from_flash;"\
            "fi;"\
            "bcb uboot-command;"\
            "run switch_bootmode;"\
            "run get_os_type;"\
            "if test ${os_type} = rtos; then "\
                "setenv loadaddr ${loadaddr_rtos};"\
                "store read ${loadaddr} ${boot_part} 0 0x400000;"\
                "bootm ${loadaddr};"\
            "else if test ${os_type} = kernel; then "\
                "get_system_as_root_mode;"\
                "echo system_mode in storeboot: ${system_mode};"\
                "get_avb_mode;"\
                "echo active_slot in storeboot: ${active_slot};"\
                "if test ${system_mode} = 1; then "\
                    "setenv bootargs ${bootargs} ro rootwait skip_initramfs;"\
                "else "\
                    "setenv bootargs ${bootargs} androidboot.force_normal_boot=1;"\
                "fi;"\
                "if test ${active_slot} != normal; then "\
                    "setenv bootargs ${bootargs} androidboot.slot_suffix=${active_slot};"\
                "fi;"\
                "if fdt addr ${dtb_mem_addr}; then else echo retry common dtb; run common_dtb_load; fi;"\
                "setenv loadaddr ${loadaddr_kernel};"\
                "if imgread kernel ${boot_part} ${loadaddr}; then bootm ${loadaddr}; fi;"\
            "else echo wrong OS format ${os_type}; fi;fi;"\
            "echo try upgrade as booting failure; run update;"\
            "\0" \
         "update="\
            /*first usb burning, second sdc_burn, third ext-sd autoscr/recovery, last udisk autoscr/recovery*/\
            "run usb_burning; "\
            "run recovery_from_flash;"\
            "\0"\
        "recovery_from_flash="\
            "echo active_slot: ${active_slot};"\
            "setenv loadaddr ${loadaddr_kernel};"\
            "if fdt addr ${dtb_mem_addr}; then else echo retry common dtb; run common_dtb_load; fi;"\
            "setenv bootargs ${bootargs} ${fs_type} aml_dt=${aml_dt} recovery_part=${boot_part} recovery_offset=${recovery_offset} androidboot.slot_suffix=${active_slot};"\
            "if imgread kernel ${boot_part} ${loadaddr}; then bootm ${loadaddr}; fi;"\
            "\0"\
        "bcb_cmd="\
            "get_avb_mode;"\
            "get_valid_slot;"\
            "if test ${vendor_boot_mode} = true; then "\
                "setenv loadaddr_kernel 0x3080000;"\
                "setenv dtb_mem_addr 0x1000000;"\
            "fi;"\
            "if test ${active_slot} != normal; then "\
                "echo ab mode, read dtb from kernel;"\
                "setenv common_dtb_load ""imgread dtb ${boot_part} ${dtb_mem_addr}"";"\
            "fi;"\
            "\0"\
        "init_display="\
            "get_rebootmode;"\
            "echo reboot_mode:::: ${reboot_mode};"\
            "if test ${reboot_mode} = quiescent; then "\
                    "setenv reboot_mode_android ""quiescent"";"\
                    "setenv dolby_status 0;"\
                    "setenv dolby_vision_on 0;"\
                    "setenv outputmode dummy_l;"\
                    "run storeargs;"\
                    "setenv bootargs ${bootargs} androidboot.quiescent=1;"\
                    "osd open;osd clear;"\
            "else if test ${reboot_mode} = recovery_quiescent; then "\
                    "setenv reboot_mode_android ""quiescent"";"\
                    "setenv dolby_status 0;"\
                    "setenv dolby_vision_on 0;"\
                    "setenv outputmode dummy_l;"\
                    "run storeargs;"\
                    "setenv bootargs ${bootargs} androidboot.quiescent=1;"\
                    "osd open;osd clear;"\
            "else "\
                "setenv reboot_mode_android ""normal"";"\
                "run storeargs;"\
                "hdmitx hpd;hdmitx get_preferred_mode;hdmitx get_parse_edid;setenv dolby_status 0;setenv dolby_vision_on 0;osd open;osd clear;bmp display $logo_addr;bmp scale;vout output ${outputmode};vpp hdrpkt;"\
            "fi;fi;"\
            "\0"\
        "storage_param="\
            "store param;"\
            "setenv bootargs ${bootargs} ${mtdbootparts}; "\
            "\0"\
        "boot_rma="\
            "if test ${active_slot} = _a; then "\
                "setenv rma_slot _b; "\
            "else "\
                "setenv rma_slot _a; "\
            "fi; "\
            "tcpc start; "\
            "usb start; "\
            "if size usb 0 boot.img && "\
            "   itest ${filesize} <= 0x4000000 && "\
            "   load usb 0 ${loadaddr} boot.img ${filesize} && "\
            "   store write ${loadaddr} boot${rma_slot} 0 ${filesize} && "\
            "   size usb 0 vendor_boot.img && "\
            "   itest ${filesize} <= 0x4000000 && "\
            "   load usb 0 ${loadaddr} vendor_boot.img ${filesize} && "\
            "   store write ${loadaddr} vendor_boot${rma_slot} 0 ${filesize} && "\
            "   size usb 0 dtbo.img && "\
            "   itest ${filesize} <= 0x200000 && "\
            "   load usb 0 ${loadaddr} dtbo.img ${filesize} && "\
            "   store write ${loadaddr} dtbo${rma_slot} 0 ${filesize} && "\
            "   size usb 0 vbmeta_system.img && "\
            "   itest ${filesize} <= 0x100000 && "\
            "   load usb 0 ${loadaddr} vbmeta_system.img ${filesize} && "\
            "   store write ${loadaddr} vbmeta_system${rma_slot} 0 ${filesize} && "\
            "   size usb 0 vbmeta.img && "\
            "   itest ${filesize} <= 0x100000 && "\
            "   load usb 0 ${loadaddr} vbmeta.img ${filesize} && "\
            "   store write ${loadaddr} vbmeta${rma_slot} 0 ${filesize}; then "\
                "setenv active_slot ${rma_slot}; "\
                "setenv boot_part boot${rma_slot}; "\
                "setenv vendor_boot_part vendor_boot${rma_slot}; "\
                "if test ${rma_slot} = _a; then "\
                    "setenv slot-suffixes 0; "\
                "else "\
                    "setenv slot-suffixes 1; "\
                "fi; "\
                "setenv use_external_avb_key 1; "\
                "setenv bootargs ${bootargs} androidboot.rma=true; "\
                "run recovery_from_flash; "\
            "fi; "\
            "\0"

#define CONFIG_PREBOOT  \
            "run bcb_cmd; "\
            "run init_display;"\
            "run storeargs;"

/* #define CONFIG_ENV_IS_NOWHERE  1 */
#define CONFIG_ENV_SIZE   (64*1024)
#define CONFIG_FIT 1
#define CONFIG_OF_LIBFDT 1
#define CONFIG_ANDROID_BOOT_IMAGE 1
#define CONFIG_SYS_BOOTM_LEN (64<<20) /* Increase max gunzip size*/

/* cpu */
#define CONFIG_CPU_CLK					1704 //MHz. Range: 100-1800 should be multiple of 24

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
#define CONFIG_SYS_MALLOC_LEN				(192*1024*1024)
#endif

//#define CONFIG_NR_DRAM_BANKS			1
/* ddr functions */
#define DDR_FULL_TEST            0 //0:disable, 1:enable. ddr full test
#define DDR_LOW_POWER            0 //0:disable, 1:enable. ddr clk gate for lp
#define DDR_ZQ_PD                0 //0:disable, 1:enable. ddr zq power down
#define DDR_USE_EXT_VREF         0 //0:disable, 1:enable. ddr use external vref
#define DDR4_TIMING_TEST         0 //0:disable, 1:enable. ddr4 timing test function
#define DDR_PLL_BYPASS           0 //0:disable, 1:enable. ddr pll bypass function

/* vpu */
#define AML_VPU_CLK_LEVEL_DFT 7

/* osd */
#define OSD_SCALE_ENABLE
#define AML_OSD_HIGH_VERSION

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

/* define CONFIG_SYS_MEM_TOP_HIDE 8M space for free buffer */
#define CONFIG_SYS_MEM_TOP_HIDE		0x00800000

#define CONFIG_CPU_ARMV8

/* define CONFIG_UPDATE_MMU_TABLE for need update mmu */
#define CONFIG_UPDATE_MMU_TABLE

/* #define CONFIG_MULTI_DTB    1 */

/* support secure boot */
#define CONFIG_AML_SECURE_UBOOT   1

#if defined(CONFIG_AML_SECURE_UBOOT)

/* unify build for generate encrypted bootloader "u-boot.bin.encrypt" */
#define CONFIG_AML_CRYPTO_UBOOT   1
//#define CONFIG_AML_SIGNED_UBOOT   1
/* unify build for generate encrypted kernel image
   SRC : "board/amlogic/(board)/boot.img"
   DST : "fip/boot.img.encrypt" */
/* #define CONFIG_AML_CRYPTO_IMG       1 */

#endif /* CONFIG_AML_SECURE_UBOOT */

#define CONFIG_FIP_IMG_SUPPORT  1

#define BL32_SHARE_MEM_SIZE  0x800000

#endif

/* AVB KEYS USED */
// TODO(b/206911824): remove default flag in production
#define CONFIG_AVB2_KPUB_DEFAULT
#define CONFIG_AVB2_KPUB_VENDOR_MULTIPLE
