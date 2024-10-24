/*
 * (C) Copyright 2012
 * Amlogic. Inc. zongdong.jiao@amlogic.com
 *
 * This file is used to prefetch/verify/compare HDCP keys
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>
#include <environment.h>
#include <malloc.h>
#include <asm/byteorder.h>
#include <amlogic/hdmi.h>
#include <amlogic/dolby_vision.h>
#ifdef CONFIG_AML_LCD
#include <amlogic/aml_lcd.h>
#endif

#define BIT(nr)			(1UL << (nr))

#ifdef CONFIG_AML_LCD
static unsigned int hdmitx_parse_vout_name(char *name)
{
	char *p, *frac_str;
	unsigned int frac = 0;

	p = strchr(name, ',');
	if (!p) {
		frac = 0;
	} else {
		frac_str = p + 1;
		*p = '\0';
		if (strcmp(frac_str, "frac") == 0)
			frac = 1;
	}

	return frac;
}
#endif

static int do_hpd_detect(cmd_tbl_t *cmdtp, int flag, int argc,
	char *const argv[])
{
#ifdef CONFIG_AML_LCD
	struct aml_lcd_drv_s *lcd_drv = NULL;
	char *mode;
	unsigned int frac;
#endif
	int st;
	char* hdmimode;
	char* cvbsmode;
	char* colorattribute;
	int hpd_st = 0;
	/* some TV sets pull hpd high 1.3S after detect pwr5v high */
	int loop = 15;

	st = getenv_ulong("hdmitx_hpd_bypass", 10, 0);
	if (st) {
		printf("hdmitx_hpd_bypass detect\n");
		return 0;
	}

#ifdef CONFIG_AML_LCD
	lcd_drv = aml_lcd_get_driver();
	if (lcd_drv) {
		if (lcd_drv->lcd_outputmode_check) {
			mode = (char *)malloc(64 * sizeof(char));
			if (!mode) {
				printf("cmd_hpd: mode malloc falied\n");
				if (lcd_drv->lcd_outputmode_check(mode, 0) == 0)
					return 0;
			} else {
				memset(mode, 0, sizeof(mode));
				sprintf(mode, "%s", getenv("outputmode"));
				frac = hdmitx_parse_vout_name(mode);
				if (lcd_drv->lcd_outputmode_check(mode, frac) == 0) {
					free(mode);
					return 0;
				}
				free(mode);
			}
		}
	}
#endif

	hpd_st = hdmitx_device.HWOp.get_hpd_state();
	if (!hpd_st) {
		/* For some TV, they cost extra time to pullup HPD after 5V */
		while (loop--) {
			mdelay(100);
			hpd_st = hdmitx_device.HWOp.get_hpd_state();
			if (hpd_st) {
				printf("hpd delay %d ms\n", (15 - loop) * 100);
				break;
			}
		}
	}
	printf("hpd_state=%d\n", hpd_st);

	/*get hdmi mode and colorattribute from env */
	hdmimode = getenv("hdmimode");
	if (hdmimode)
		printf("do_hpd_detect: hdmimode=%s\n", hdmimode);

	colorattribute = getenv("colorattribute");
	if (colorattribute)
		printf("do_hpd_detect: colorattribute=%s\n", colorattribute);

	/* if hpd_st high, output mode will be saved on hdmi side */
	if (!hpd_st) {
		cvbsmode = getenv("cvbsmode");
		if (cvbsmode)
			setenv("outputmode", cvbsmode);
		setenv("hdmichecksum", "0x00000000");
		run_command("saveenv", 0);
	} else {
		if (!strstr(getenv("outputmode"), "hz"))
			setenv("outputmode", "1080p60hz");
	}

	hdmitx_device.hpd_state = hpd_st;
	return hpd_st;
}

bool hdmitx_get_hpd_state_ext(void)
{
	return hdmitx_device.hpd_state;
}

static unsigned char edid_raw_buf[256] = {0};
static void dump_edid_raw_8bytes(unsigned char *buf)
{
	int i = 0;
	for (i = 0; i < 8; i++)
		printf("%02x ", buf[i]);
	printf("\n");
}

static void dump_full_edid(const unsigned char *buf)
{
	int i;
	int blk_no;

	if (!buf)
		return;
	blk_no = buf[126] + 1;
	if (blk_no > 4)
		blk_no = 4;

	if (blk_no == 2)
		if (buf[128 + 4] == 0xe2 && buf[128 + 5] == 0x78)
			blk_no = buf[128 + 6] + 1;
	if (blk_no > EDID_BLK_NO)
		blk_no = EDID_BLK_NO;

	printf("Dump EDID Rawdata\n");
	for (i = 0; i < blk_no * EDID_BLK_SIZE; i++) {
		printk("%02x", buf[i]);
		if (((i+1) & 0x1f) == 0)    /* print 32bytes a line */
			printk("\n");
	}
}

static int do_edid(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	unsigned int tmp_addr = 0;
	unsigned char edid_addr = 0;
	unsigned char st = 0;

	memset(edid_raw_buf, 0, ARRAY_SIZE(edid_raw_buf));
	if (argc < 2)
		return cmd_usage(cmdtp);
	if (strcmp(argv[1], "read") == 0) {
		tmp_addr = simple_strtoul(argv[2], NULL, 16);
		if (tmp_addr > 0xff)
			return cmd_usage(cmdtp);
		edid_addr = tmp_addr;
		/* read edid raw data */
		/* current only support read 1 byte edid data */
		st = hdmitx_device.HWOp.read_edid(
			&edid_raw_buf[edid_addr & 0xf8], edid_addr & 0xf8, 8);
		printf("edid[0x%02x]: 0x%02x\n", edid_addr,
			edid_raw_buf[edid_addr]);
		if (0) /* Debug only */
			dump_edid_raw_8bytes(&edid_raw_buf[edid_addr & 0xf8]);
		if (!st)
			printf("edid read failed\n");
	}
	return st;
}

static int do_rx_det(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	unsigned char edid_addr = 0xf8;     // Fixed Address
	unsigned char st = 0;

	memset(edid_raw_buf, 0, ARRAY_SIZE(edid_raw_buf));

	// read edid raw data
	// current only support read 1 byte edid data
	st = hdmitx_device.HWOp.read_edid(&edid_raw_buf[edid_addr & 0xf8], edid_addr & 0xf8, 8);
	if (1)      // Debug only
		dump_edid_raw_8bytes(&edid_raw_buf[edid_addr & 0xf8]);
	if (st) {
#if 0
		// set fake value for debug
		edid_raw_buf[250] = 0xfb;
		edid_raw_buf[251] = 0x0c;
		edid_raw_buf[252] = 0x01;
#endif
		if ((edid_raw_buf[250] == 0xfb) & (edid_raw_buf[251] == 0x0c)) {
			printf("RX is FBC\n");

			// set outputmode ENV
			switch (edid_raw_buf[252] & 0x0f) {
			case 0x0:
				run_command("setenv outputmode 1080p50hz", 0);
				break;
			case 0x1:
				run_command("setenv outputmode 2160p50hz420", 0);
				break;
			case 0x2:
				run_command("setenv outputmode 1080p50hz44410bit", 0);
				break;
			case 0x3:
				run_command("setenv outputmode 2160p50hz42010bit", 0);
				break;
			case 0x4:
				run_command("setenv outputmode 2160p50hz42210bit", 0);
				break;
			case 0x5:
				run_command("setenv outputmode 2160p50hz", 0);
				break;
			default:
				run_command("setenv outputmode 1080p50hz", 0);
				break;
			}

			// set RX 3D Info
			switch ((edid_raw_buf[252] >> 4) & 0x0f) {
			case 0x00:
				run_command("setenv rx_3d_info 0", 0);
				break;
			case 0x01:
				run_command("setenv rx_3d_info 1", 0);
				break;
			case 0x02:
				run_command("setenv rx_3d_info 2", 0);
				break;
			case 0x03:
				run_command("setenv rx_3d_info 3", 0);
				break;
			case 0x04:
				run_command("setenv rx_3d_info 4", 0);
				break;
			default:
				break;
			}

			switch (edid_raw_buf[253]) {
			case 0x1:
				// TODO
				break;
			case 0x2:
				// TODO
				break;
			default:
				break;
			}
		}
	} else
		printf("edid read failed\n");

	return st;
}

static void save_default_720p(void)
{
	setenv("outputmode", DEFAULT_HDMI_MODE);
	setenv("colorattribute", DEFAULT_COLOR_FORMAT);
}

static void hdmitx_mask_rx_info(struct hdmitx_dev *hdev)
{
	if (!hdev || !hdev->para)
		return;

	if (getenv("colorattribute"))
		hdmi_parse_attr(hdev->para, getenv("colorattribute"));

	/* when current output color depth is 8bit, mask hdr capability */
	/* refer to SWPL-44445 for more detail */
	if (hdev->para->cd == HDMI_COLOR_DEPTH_24B)
		memset(&hdev->RXCap.hdr_info, 0, sizeof(struct hdr_info));
}

static void hdmitx_config_csc_en(struct hdmitx_dev *hdev)
{
	if (!hdev)
		return;

	if (getenv("config_csc_en")) {
		if (strncmp("0", getenv("config_csc_en"), 1) == 0)
			hdev->config_csc_en = 0;
		else
			hdev->config_csc_en = 1;
		printf("config_csc_en:%d\n", hdev->config_csc_en);
	} else {
		hdev->config_csc_en = 0;
	}
}

static int do_output(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	if (argc < 1)
		return cmd_usage(cmdtp);

	if (strcmp(argv[1], "list") == 0)
		hdmitx_device.HWOp.list_support_modes();
	else if (strcmp(argv[1], "bist") == 0) {
		unsigned int mode = 0;
		if (strcmp(argv[2], "off") == 0)
			mode = 0;
		else if (strcmp(argv[2], "line") == 0)
			mode = 2;
		else if (strcmp(argv[2], "dot") == 0)
			mode = 3;
		else
			mode = simple_strtoul(argv[2], NULL, 10);
		hdmitx_device.HWOp.test_bist(mode);
	} else if (strcmp(argv[1], "prbs") == 0) {
		hdmitx_device.para->cs = HDMI_COLOR_FORMAT_RGB;
		hdmitx_device.para->cd = HDMI_COLOR_DEPTH_24B;
		hdmitx_device.vic = HDMI_1920x1080p60_16x9;
		hdmi_tx_set(&hdmitx_device);
		hdmitx_device.HWOp.test_bist(10);
	} else { /* "output" */
		if (!edid_parsing_ok(&hdmitx_device)) {
			/* in SWPL-34712: if EDID parsing error in kernel,
			 * only forcely output default mode(480p,RGB,8bit)
			 * in sysctl, not save the default mode to env.
			 * if uboot follow this rule, will cause issue OTT-19333:
			 * uboot read edid error and then output default mode,
			 * without save it mode env. if then kernel edid normal,
			 * sysctrl/kernel get mode from env, the actual output
			 * mode differs with outputmode env,it will
			 * cause display abnormal(such as stretch). so don't
			 * follow this rule in uboot, that's to say the actual
			 * output mode needs to stays with the outputmode env.
			 */
			printf("edid parsing ng, forcely output 720p, rgb,8bit\n");
			save_default_720p();
			hdmitx_device.vic = HDMI_1280x720p60_16x9;
			hdmitx_device.para =
				hdmi_get_fmt_paras(hdmitx_device.vic);
			hdmitx_device.para->cs = HDMI_COLOR_FORMAT_RGB;
			hdmitx_device.para->cd = HDMI_COLOR_DEPTH_24B;
			hdmi_tx_set(&hdmitx_device);
			return CMD_RET_SUCCESS;
		}
		hdmitx_device.vic = hdmi_get_fmt_vic(argv[1]);
		hdmitx_device.para = hdmi_get_fmt_paras(hdmitx_device.vic);
		if (hdmitx_device.vic == HDMI_unknown) {
			/* Not find VIC */
			printf("Not find '%s' mapped VIC\n", argv[1]);
			return CMD_RET_FAILURE;
		} else
			printf("set hdmitx VIC = %d\n", hdmitx_device.vic);
		if (strstr(argv[1], "hz420") != NULL)
			hdmitx_device.para->cs = HDMI_COLOR_FORMAT_420;
		if (getenv("colorattribute"))
			hdmi_parse_attr(hdmitx_device.para, getenv("colorattribute"));
		/* For RGB444 or YCbCr444 under 6Gbps mode, no deepcolor */
		/* Only 4k50/60 has 420 modes */
		switch (hdmitx_device.vic) {
		case HDMI_3840x2160p50_16x9:
		case HDMI_3840x2160p60_16x9:
		case HDMI_4096x2160p50_256x135:
		case HDMI_4096x2160p60_256x135:
		case HDMI_3840x2160p50_64x27:
		case HDMI_3840x2160p60_64x27:
		case HDMI_3840x2160p50_16x9_Y420:
		case HDMI_3840x2160p60_16x9_Y420:
		case HDMI_4096x2160p50_256x135_Y420:
		case HDMI_4096x2160p60_256x135_Y420:
		case HDMI_3840x2160p50_64x27_Y420:
		case HDMI_3840x2160p60_64x27_Y420:
			if ((hdmitx_device.para->cs == HDMI_COLOR_FORMAT_RGB) ||
			    (hdmitx_device.para->cs == HDMI_COLOR_FORMAT_444)) {
				if (hdmitx_device.para->cd != HDMI_COLOR_DEPTH_24B) {
					printf("vic %d cs %d has no cd %d\n",
						hdmitx_device.vic,
						hdmitx_device.para->cs,
						hdmitx_device.para->cd);
					hdmitx_device.para->cd = HDMI_COLOR_DEPTH_24B;
					printf("set cd as %d\n", HDMI_COLOR_DEPTH_24B);
				}
			}
			if (hdmitx_device.para->cs == HDMI_COLOR_FORMAT_420)
				hdmitx_device.vic |= HDMITX_VIC420_OFFSET;
			break;
		default:
			if (hdmitx_device.para->cs == HDMI_COLOR_FORMAT_420) {
				printf("vic %d has no cs %d\n", hdmitx_device.vic,
					hdmitx_device.para->cs);
				hdmitx_device.para->cs = HDMI_COLOR_FORMAT_444;
				printf("set cs as %d\n", HDMI_COLOR_FORMAT_444);
			}
			/* For VESA modes, should be RGB format */
			if (hdmitx_device.vic >= HDMITX_VESA_OFFSET) {
				hdmitx_device.para->cs = HDMI_COLOR_FORMAT_RGB;
				hdmitx_device.para->cd = HDMI_COLOR_DEPTH_24B;
			}
			break;
		}
		/* currently, hdmi mode is always set, if
		 * mode set abort/exit, need to add return
		 * result of mode setting, so that vout
		 * driver will pass it to kernel, and do
		 * mode setting again when vout init in kernel
		 */
		hdmitx_config_csc_en(&hdmitx_device);
		hdmi_tx_set(&hdmitx_device);
	}
	return CMD_RET_SUCCESS;
}

static int do_blank(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	if (argc < 1)
		return cmd_usage(cmdtp);

	if (strcmp(argv[1], "1") == 0)
		hdmitx_device.HWOp.output_blank(1);
	if (strcmp(argv[1], "0") == 0)
		hdmitx_device.HWOp.output_blank(0);

	return CMD_RET_SUCCESS;
}

static int do_off(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	hdmitx_device.vic = HDMI_unknown;
	hdmitx_device.HWOp.turn_off();
	printf("turn off hdmitx\n");
	return 1;
}

static int do_dump(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	hdmitx_device.HWOp.dump_regs();
	return 1;
}

static int do_info(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	struct hdmitx_dev *hdev = &hdmitx_device;
	struct hdmi_format_para *para = hdev->para;

	printf("%s %d\n", para->ext_name, hdev->vic);
	printf("cd%d cs%d cr%d\n", para->cd, para->cs, para->cr);
	printf("frac_rate: %d\n", hdev->frac_rate_policy);
	return 1;
}

static int xtochar(int num, char *checksum)
{
	struct hdmitx_dev *hdev = &hdmitx_device;

	if (((hdev->rawedid[num]  >> 4 ) & 0xf) <= 9)
		checksum[0] = ((hdev->rawedid[num]  >> 4 ) & 0xf) + '0';
	else
		checksum[0] = ((hdev->rawedid[num]  >> 4 ) & 0xf) -10 + 'a';

	if ((hdev->rawedid[num] & 0xf) <= 9)
		checksum[1] = (hdev->rawedid[num] & 0xf) + '0';
	else
		checksum[1] = (hdev->rawedid[num] & 0xf) -10 + 'a';

	return 0;
}

/* hdr_priority definition:
 *   strategy1: bit[3:0]
 *       0: original cap
 *       1: disable dolby vision cap
 *       2: disable dolby vision and hdr/hlg cap
 *   strategy2:
 *       bit4: 1: disable dv  0:enable dv
 *       bit5: 1: disable hdr10/hdr10+  0: enable hdr10/hdr10+
 *       bit6: 1: disable hlg  0: enable hlg
 *   bit28-bit31 choose strategy: bit[31:28]
 *       0: strategy1
 *       1: strategy2
 */

/* for uboot, there is no need to dynamically change the hdr_priority as
 * kernel. So below functions only implement the disable_xxx_info() function,
 * and leave the enable_xxx_info as blank
 */

/* dv_info */
static void enable_dv_info(struct dv_info *des, const struct dv_info *src)
{
	if (!des || !src)
		return;
}

static void disable_dv_info(struct dv_info *des)
{
	if (!des)
		return;

	memset(des, 0, sizeof(*des));
}

/* hdr10 */
static void enable_hdr10_info(struct hdr_info *des, const struct hdr_info *src)
{
	if (!des || !src)
		return;
}

static void disable_hdr10_info(struct hdr_info *des)
{
	if (!des)
		return;

	des->hdr_sup_eotf_smpte_st_2084 = 0;
	des->hdr_sup_SMD_type1 = 0;
	des->hdr_lum_max = 0;
	des->hdr_lum_avg = 0;
	des->hdr_lum_min = 0;
}

/* hdr10plus */
static void enable_hdr10p_info(struct hdr10_plus_info *des, const struct hdr10_plus_info *src)
{
	if (!des || !src)
		return;
}

static void disable_hdr10p_info(struct hdr10_plus_info *des)
{
	if (!des)
		return;

	memset(des, 0, sizeof(*des));
}

/* hlg */
static void enable_hlg_info(struct hdr_info *des, const struct hdr_info *src)
{
	if (!des || !src)
		return;
}

static void disable_hlg_info(struct hdr_info *des)
{
	if (!des)
		return;

	des->hdr_sup_eotf_hlg = 0;
}

static void enable_all_hdr_info(struct rx_cap *prxcap)
{
	if (!prxcap)
		return;
}

static void update_hdr_strategy1(struct rx_cap *prxcap, u32 strategy)
{
	if (!prxcap)
		return;

	switch (strategy) {
	case 0:
		enable_all_hdr_info(prxcap);
		break;
	case 1:
		disable_dv_info(&prxcap->dv_info);
		break;
	case 2:
		disable_dv_info(&prxcap->dv_info);
		disable_hdr10_info(&prxcap->hdr_info);
		disable_hdr10p_info(&prxcap->hdr10plus_info);
		disable_hlg_info(&prxcap->hdr_info);
		break;
	default:
		break;
	}
}

static void update_hdr_strategy2(struct rx_cap *prxcap, u32 strategy)
{
	if (!prxcap)
		return;

	/* bit4: 1 disable dv  0 enable dv */
	if (strategy & BIT(4))
		disable_dv_info(&prxcap->dv_info);
	else
		enable_dv_info(&prxcap->dv_info, NULL);
	/* bit5: 1 disable hdr10/hdr10+   0 enable hdr10/hdr10+ */
	if (strategy & BIT(5)) {
		disable_hdr10_info(&prxcap->hdr_info);
		disable_hdr10p_info(&prxcap->hdr10plus_info);
	} else {
		enable_hdr10_info(&prxcap->hdr_info, NULL);
		enable_hdr10p_info(&prxcap->hdr10plus_info, NULL);
	}
	/* bit6: 1 disable hlg   0 enable hlg */
	if (strategy & BIT(6))
		disable_hlg_info(&prxcap->hdr_info);
	else
		enable_hlg_info(&prxcap->hdr_info, NULL);
}

static int hdmitx_set_hdr_priority(struct rx_cap *prxcap, u32 hdr_priority)
{
	u32 choose = 0;
	u32 strategy = 0;

	if (!prxcap)
		return -1;

	printf("%s, set hdr_prio: %u\n", __func__, hdr_priority);
	/* choose strategy: bit[31:28] */
	choose = (hdr_priority >> 28) & 0xf;
	switch (choose) {
	case 0:
		strategy = hdr_priority & 0xf;
		update_hdr_strategy1(prxcap, strategy);
		break;
	case 1:
		strategy = hdr_priority & 0xf0;
		update_hdr_strategy2(prxcap, strategy);
		break;
	default:
		break;
	}
	return 0;
}

static void get_parse_edid_data(struct hdmitx_dev *hdev)
{
	unsigned char *edid = hdev->rawedid;
	unsigned int byte_num = 0;
	unsigned char blk_no = 1;
	int hdr_priority = get_hdr_strategy_priority();

	/* get edid data */
	while (byte_num < 128 * blk_no) {
		hdev->HWOp.read_edid(&edid[byte_num], byte_num & 0x7f, byte_num / 128);
		if (byte_num == 120) {
			blk_no = edid[126] + 1;
			if (blk_no > 4)
				blk_no = 4; /* MAX Read Blocks 4 */
		}
		if (byte_num == 128) {
			if (edid[128 + 4] == 0xe2 && edid[128 + 5] == 0x78)
				blk_no = edid[128 + 6] + 1;
			if (blk_no > EDID_BLK_NO)
				blk_no = EDID_BLK_NO; /* MAX Read Blocks 8 */
		}
		byte_num += 8;
	}

	/* dump edid raw data */
	dump_full_edid(hdev->rawedid);

	/* parse edid data */
	hdmi_edid_parsing(hdev->rawedid, &hdev->RXCap);

	if (hdr_priority == -1)
		return;
	hdmitx_set_hdr_priority(&hdev->RXCap, hdr_priority);
}

/* policy process: to find the output mode/attr/dv_type */
void scene_process(struct hdmitx_dev *hdev,
	scene_output_info_t *scene_output_info)
{
	hdmi_data_t hdmidata;

	if (!hdev || !scene_output_info)
		return;
	/* 1.read dolby vision mode from prop(maybe need to env) */
	memset(&hdmidata, 0, sizeof(hdmi_data_t));
	get_hdmi_data(hdev, &hdmidata);

	/* 2. dolby vision scene process */
	/* only for tv support dv and box enable dv */
	if (is_dv_preference(hdev)) {
		dolbyvision_scene_process(&hdmidata, scene_output_info);
	} else if (is_dolby_enabled()) {
		/* for enable dolby vision core when
		 * first boot connecting non dv tv
		 * NOTE: let systemcontrol to enable DV core
		 */
		/* scene_output_info->final_dv_type = DOLBY_VISION_ENABLE; */
	} else {
		/* for UI disable dolby vision core and boot keep the status
		 * NOTE: TBD if need to disable DV here
		 */
		/* scene_output_info->final_dv_type = DOLBY_VISION_DISABLE; */
	}
	/* 3.sdr scene process */
	/* decide final display mode and deepcolor */
	if (is_dv_preference(hdev)) {
		/* do nothing
		 * already done above, just sync with sysctrl
		 */
	} else if (is_hdr_preference(hdev)) {
		hdr_scene_process(&hdmidata, scene_output_info);
	} else {
		sdr_scene_process(&hdmidata, scene_output_info);
	}
	/* not find outputmode and use default mode */
	if (strlen(scene_output_info->final_displaymode) == 0)
		strcpy(scene_output_info->final_displaymode, DEFAULT_HDMI_MODE);
	/* not find color space and use default mode */
	if (!strstr(scene_output_info->final_deepcolor, "bit"))
		strcpy(scene_output_info->final_deepcolor, DEFAULT_COLOR_FORMAT);
}

static int do_get_parse_edid(cmd_tbl_t * cmdtp, int flag, int argc,
	char * const argv[])
{
	struct hdmitx_dev *hdev = &hdmitx_device;

	unsigned char *store_checkvalue;
	unsigned int i;
	unsigned int checkvalue[4];
	unsigned int checkvalue1;
	unsigned int checkvalue2;
	char checksum[11];
	unsigned char def_cksum[] = {'0', 'x', '0', '0', '0', '0', '0', '0', '0', '0', '\0'};
	char *hdmimode;
	char *colorattribute;
	int user_dv_mode;
	char *last_output_mode;
	char *last_colorattribute;
	int last_dv_status;
	bool over_write = false;
	char dv_type[2] = {0};
	scene_output_info_t scene_output_info;
	struct hdmi_format_para *para = NULL;
	bool mode_support = false;
	/* hdmi_mode / colorattribute may be null or "none".
	 * if either is null or "none", it means user not
	 * selected manually, and need to select the best
	 * mode or colorattribute by policy
	 */
	bool no_manual_output = false;

	if (!hdev) {
		printf("null hdmitx dev\n");
		return CMD_RET_FAILURE;
	}
	if (!hdev->hpd_state) {
		printf("HDMI HPD low, no need parse EDID\n");
		return 1;
	}
	memset(&scene_output_info, 0, sizeof(scene_output_info_t));
	memset(hdev->rawedid, 0, EDID_BLK_SIZE * EDID_BLK_NO);

	get_parse_edid_data(hdev);

	/* check if the tv has changed or anything wrong */
	store_checkvalue = (unsigned char*)getenv("hdmichecksum");
	/* get user selected output mode/color */
	colorattribute = getenv("user_colorattribute");
	hdmimode = getenv("hdmimode");
	user_dv_mode = get_ubootenv_dv_type();

	last_output_mode = getenv("outputmode");
	last_colorattribute = getenv("colorattribute");
	last_dv_status = get_ubootenv_dv_status();
	if (!store_checkvalue)
		store_checkvalue = def_cksum;

	printf("read hdmichecksum: %s, user hdmimode: %s, colorattribute: %s, dv_type: %d\n",
	       store_checkvalue, hdmimode ? hdmimode : "null",
	       colorattribute ? colorattribute : "null", user_dv_mode);

	for (i = 0; i < 4; i++) {
		if (('0' <= store_checkvalue[i * 2 + 2]) && (store_checkvalue[i * 2 + 2] <= '9'))
			checkvalue1 = store_checkvalue[i * 2 + 2] -'0';
		else
			checkvalue1 = store_checkvalue[i * 2 + 2] -'W';
		if (('0' <= store_checkvalue[i * 2 + 3]) && (store_checkvalue[i * 2 + 3] <= '9'))
			checkvalue2 = store_checkvalue[i * 2 + 3] -'0';
		else
			checkvalue2 = store_checkvalue[i * 2 + 3] -'W';
		checkvalue[i] = checkvalue1 * 16 + checkvalue2;
	}

	if ((checkvalue[0] != hdev->rawedid[0x7f])  ||
	    (checkvalue[1] != hdev->rawedid[0xff])  ||
	    (checkvalue[2] != hdev->rawedid[0x17f]) ||
	    (checkvalue[3] != hdev->rawedid[0x1ff])) {
		hdev->RXCap.edid_changed = 1;

		checksum[0] = '0';
		checksum[1] = 'x';
		for (i = 0; i < 4; i++)
			xtochar(0x80 * i + 0x7f, &checksum[2* i + 2]);
		checksum[10] = '\0';
		memcpy(hdev->RXCap.checksum, checksum, 10);
		printf("TV has changed, now crc: %s\n", checksum);
	} else {
		memcpy(hdev->RXCap.checksum, store_checkvalue, 10);
		printf("TV is the same, checksum: %s\n", hdev->RXCap.checksum);
	}

	/* check user have selected both mode/color or not */
	if (!hdmimode || !strcmp(hdmimode, "none") ||
		!colorattribute || !strcmp(colorattribute, "none"))
		no_manual_output = true;
	else
		no_manual_output = false;

	if (!no_manual_output) {
		/* check current user selected mode + color support or not */
		para = hdmi_tst_fmt_name(hdmimode, colorattribute);
		if (hdmitx_edid_check_valid_mode(hdev, para))
			mode_support = true;
		else
			mode_support = false;

		/* if user selected mode/color/dv type which saved in ubootenv of
		 * hdmimode/user_colorattribute/user_prefer_dv_type are different
		 * with last actual output mode/color/dv type which saved in
		 * ubootenv of outputmode/colorattribute/dolby_status, then it means
		 * that the user selected format is over-writen by policy(for example:
		 * firstly user has selected HDR priority to HDR, and select color
		 * to rgb,12bit(now the "user_colorattribute" env will be "rgb,12bit"),
		 * but then it selected HDR priority to DV, the actual output color
		 * will be "444,8bit" or "422,12bit" according to dv type, and
		 * the ubootenv "colorattribute" will be "444,8bit" or "422,12bit"),
		 * then uboot should use the policy to select the output format,
		 * otherwise, uboot use hdmimode/user_colorattribute/user_prefer_dv_type
		 * env, while system use outputmode/colorattribute/dolby_status env,
		 * there will be always a mode change during bootup
		 */
		if (mode_support) {
			if (!last_output_mode || strcmp(hdmimode, last_output_mode))
				over_write = true;
			else if (!last_colorattribute ||
				strcmp(colorattribute, last_colorattribute))
				over_write = true;
			else if (user_dv_mode != last_dv_status)
				over_write = true;
			else
				over_write = false;

			if (over_write)
				printf("last output_mode:%s, colorattribute:%s, dolby_status:%d\n",
				last_output_mode ? last_output_mode : "null",
				last_colorattribute ? last_colorattribute : "null",
				last_dv_status);
		}
	}
	/* 4 cases need to decide output by uboot mode select policy:
	 * 1.TV changed
	 * 2.either hdmimode or colorattribute is NULL or "none",
	 * which means that user have not selected mode or colorattribute,
	 * and need to select the auto best mode or best colorattribute.
	 * 3.user selected mode not supportted by uboot (probably
	 * means mode select policy or edid parse between sysctrl and
	 * uboot have some gap), then need to find proper output mode
	 * with uboot policy.
	 * 4.user selected mode is over writen by system policy
	 */
	if (hdev->RXCap.edid_changed || no_manual_output || !mode_support || over_write) {
		/* find proper mode if EDID changed */
		scene_process(hdev, &scene_output_info);
		setenv("hdmichecksum", hdev->RXCap.checksum);
		if (edid_parsing_ok(hdev)) {
			setenv("outputmode",
			       scene_output_info.final_displaymode);
			setenv("colorattribute",
			       scene_output_info.final_deepcolor);
			/* if change from DV TV to HDR/SDR TV, don't change
			 * DV status to disabled, as DV core need to be enabled.
			 * that's to say connect DV TV & output DV-> power down box ->
			 * connect HDR/SDR TV -> power on box, the dolby_status
			 * will keep the same as that when connect DV TV under follow sink.
			 */
			if ((scene_output_info.final_dv_type !=
			    get_ubootenv_dv_status()) &&
			    (scene_output_info.final_dv_type !=
			     DOLBY_VISION_DISABLE)) {
				sprintf(dv_type, "%d", scene_output_info.final_dv_type);
				setenv("dolby_status", dv_type);
				/* according to the policy of systemcontrol,
				 * if current DV mode is not supported by TV
				 * EDID, DV type maybe changed to one witch
				 * TV support, and need VPP/DV module to
				 * update new DV output mode.
				 */
				printf("update dolby_status: %d\n",
				       scene_output_info.final_dv_type);
			}
		} else {
			save_default_720p();
		}
		printf("update outputmode: %s\n", getenv("outputmode"));
		printf("update colorattribute: %s\n", getenv("colorattribute"));
		printf("update hdmichecksum: %s\n", getenv("hdmichecksum"));
	} else {
		setenv("outputmode", hdmimode);
		setenv("colorattribute", colorattribute);
	}
	/* ubootenv dolby_status is used for is_dv_preference() decision,
	 * system_control save current dv output status in it.
	 * it will be used by dv module later to decide DV output later.
	 * if currently adaptive hdr, then we should set dolby_status to
	 * 0, so that DV module won't enable DV.
	 */
	if (get_hdr_policy() == 1)
		setenv("dolby_status", 0);
	hdev->vic = hdmi_get_fmt_vic(getenv("outputmode"));
	hdev->para = hdmi_get_fmt_paras(hdev->vic);
	hdmitx_mask_rx_info(hdev);
	return 0;
}

static cmd_tbl_t cmd_hdmi_sub[] = {
	U_BOOT_CMD_MKENT(hpd, 1, 1, do_hpd_detect, "", ""),
	U_BOOT_CMD_MKENT(edid, 3, 1, do_edid, "", ""),
	U_BOOT_CMD_MKENT(rx_det, 1, 1, do_rx_det, "", ""),
	U_BOOT_CMD_MKENT(output, 3, 1, do_output, "", ""),
	U_BOOT_CMD_MKENT(blank, 3, 1, do_blank, "", ""),
	U_BOOT_CMD_MKENT(off, 1, 1, do_off, "", ""),
	U_BOOT_CMD_MKENT(dump, 1, 1, do_dump, "", ""),
	U_BOOT_CMD_MKENT(info, 1, 1, do_info, "", ""),
	U_BOOT_CMD_MKENT(get_parse_edid, 1, 1, do_get_parse_edid, "", ""),
};

static int do_hdmitx(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	cmd_tbl_t *c;

	if (argc < 2)
		return cmd_usage(cmdtp);

	argc--;
	argv++;

	c = find_cmd_tbl(argv[0], &cmd_hdmi_sub[0], ARRAY_SIZE(cmd_hdmi_sub));

	if (c)
		return  c->cmd(cmdtp, flag, argc, argv);
	else
		return cmd_usage(cmdtp);
}

U_BOOT_CMD(hdmitx, CONFIG_SYS_MAXARGS, 0, do_hdmitx,
	   "HDMITX sub-system 20190123",
	"hdmitx hpd\n"
	"    Detect hdmi rx plug-in\n"
#if 0
	"hdmitx edid read ADDRESS\n"
	"    Read hdmi rx edid from ADDRESS(0x00~0xff)\n"
#endif
	"hdmitx output [list | FORMAT | bist PATTERN]\n"
	"    list: list support formats\n"
	"    FORMAT can be 720p60/50hz, 1080i60/50hz, 1080p60hz, etc\n"
	"       extend with 8bits/10bits, y444/y422/y420/rgb\n"
	"       such as 2160p60hz,10bits,y420\n"
	"    PATTERN: can be as: line, dot, off, or 1920(width)\n"
	"hdmitx blank [0|1]\n"
	"    1: output blank  0: output normal\n"
	"hdmitx off\n"
	"    Turn off hdmitx output\n"
	"hdmitx info\n"
	"    current mode info\n"
	"hdmitx rx_det\n"
	"    Auto detect if RX is FBC and set outputmode\n"
);

struct hdr_info *hdmitx_get_rx_hdr_info(void)
{
	struct hdmitx_dev *hdev = &hdmitx_device;

	return &hdev->RXCap.hdr_info;
}
