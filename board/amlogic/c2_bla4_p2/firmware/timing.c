/*
 * board/amlogic/c2_bla4_p2/firmware/timing.c
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

#include <asm/arch/secure_apb.h>
#include <asm/arch/timing.h>
#include <asm/arch/ddr_define.h>

/* board clk defines */
#define CPU_CLK					1512
#define TIMMING_MAX_CONFIG		1
#define ENABLE_DDR3    0
#define ENABLE_DDR4    1
#define ENABLE_LPDDR4  0

ddr_set_t __ddr_setting[] __attribute__ ((section(".ddr_settings"))) = {
#if ENABLE_DDR4
{
	.cfg_board_common_setting.	board_id					=	CONFIG_BOARD_ID_MASK,
	.cfg_board_common_setting.	DramType					=	CONFIG_DDR_TYPE_DDR4,
	.cfg_board_common_setting.	dram_rank_config			=	CONFIG_DDR0_32BIT_RANK0_CH0,
	.cfg_board_common_setting.	dram_cs0_size_MB			=	CONFIG_DDR0_SIZE_AUTO_SIZE,//CONFIG_DDR0_SIZE_AUTO_SIZE,CONFIG_DDR0_SIZE_2048MB,1024MB,512MB,0MB,256,128,1536,768,3072,4096
	.cfg_board_common_setting.	dram_cs1_size_MB			=	CONFIG_DDR1_SIZE_0MB,
	.cfg_board_common_setting.	pll_ssc_mode				=	DDR_PLL_SSC_DISABLE,
	.cfg_board_common_setting.	ddr_rdbi_wr_enable			=	DDR_WRITE_READ_DBI_DISABLE,
	.cfg_board_common_setting.	dram_x4x8x16_mode			=	CONFIG_DRAM_MODE_X16,
	.cfg_board_common_setting.	ddr_dmc_remap				=	DDR_DMC_REMAP_DDR4_32BIT,
	.cfg_board_common_setting.	fast_boot					=	{0,0,(1<<3)|(4)},
	.cfg_board_common_setting.	fast_boot[0]				=	0x1,
	.cfg_board_common_setting.	fast_boot[3]				=	0x46,

	//DDR frequercy 1
	.cfg_board_SI_setting_ps[0].DRAMFreq					=	1272,//1320,1200,1104,1008,912,792
	.cfg_board_SI_setting_ps[0].PllBypassEn					=	0,
	.cfg_board_SI_setting_ps[0].training_SequenceCtrl		=	0,
	.cfg_board_SI_setting_ps[0].ddr_odt_config				=	DDR_DRAM_ODT_W_CS0_ODT0,
	.cfg_board_SI_setting_ps[0].clk_drv_ohm					=	DDR_SOC_AC_DRV_40_OHM,
	.cfg_board_SI_setting_ps[0].cs_drv_ohm					=	DDR_SOC_AC_DRV_40_OHM,
	.cfg_board_SI_setting_ps[0].ac_drv_ohm					=	DDR_SOC_AC_DRV_40_OHM,
	.cfg_board_SI_setting_ps[0].soc_data_drv_ohm_p			=	DDR_SOC_DATA_DRV_ODT_40_OHM,
	.cfg_board_SI_setting_ps[0].soc_data_drv_ohm_n			=	DDR_SOC_DATA_DRV_ODT_40_OHM,
	.cfg_board_SI_setting_ps[0].soc_data_odt_ohm_p			=	DDR_SOC_DATA_DRV_ODT_60_OHM,
	.cfg_board_SI_setting_ps[0].soc_data_odt_ohm_n			=	DDR_SOC_DATA_DRV_ODT_0_OHM,
	.cfg_board_SI_setting_ps[0].dram_data_drv_ohm			=	DDR_DRAM_DDR4_DRV_48_OHM,
	.cfg_board_SI_setting_ps[0].dram_data_odt_ohm			=	DDR_DRAM_DDR4_ODT_60_OHM,
	.cfg_board_SI_setting_ps[0].dram_data_wr_odt_ohm		=	DDR_DRAM_DDR_WR_ODT_0_OHM,
	.cfg_board_SI_setting_ps[0].dram_ac_odt_ohm				=	DDR_DRAM_DDR_AC_ODT_0_OHM,
	.cfg_board_SI_setting_ps[0].dram_data_drv_pull_up_calibration_ohm	=	DDR_DRAM_LPDDR4_ODT_40_OHM,
	.cfg_board_SI_setting_ps[0].lpddr4_dram_vout_voltage_range_setting	=	DDR_DRAM_LPDDR4_OUTPUT_1_3_VDDQ,
	.cfg_board_SI_setting_ps[0].reserve2					=	0,
	.cfg_board_SI_setting_ps[0].vref_ac_permil 				=	0,
	.cfg_board_SI_setting_ps[0].vref_soc_data_permil 		=	0,
	.cfg_board_SI_setting_ps[0].vref_dram_data_permil		=	0,
	.cfg_board_SI_setting_ps[0].max_core_timmming_frequency				=	0,
	.cfg_board_SI_setting_ps[0].training_phase_parameter				=	{0},
	.cfg_board_SI_setting_ps[0].ac_trace_delay_org = {
	128,128,128-20,128,128,128,128,128,
	384,384,384,384,384,384,384,384,
	384,384,384,384,384,384,384,384,
	384,384,384,384,384,384,384,384,
	384,384,384,384,
	},//total 36

	.cfg_ddr_training_delay_ps[0].	ac_trace_delay = {
	128,128,128-20,128,128,128,128,128,
	384,384,384,384,384,384,384,384,
	384,384,384,384,384,384,384,384,
	384,384,384,384,384,384,384,384,
	384,384,384,384,
	},//total 36

	.cfg_ddr_training_delay_ps[0].write_dqs_delay[0]=0x0000005f,// 95
	.cfg_ddr_training_delay_ps[0].write_dqs_delay[1]=0x00000073,// 115
	.cfg_ddr_training_delay_ps[0].write_dqs_delay[2]=0x0000005f,// 95
	.cfg_ddr_training_delay_ps[0].write_dqs_delay[3]=0x00000087,// 135
	.cfg_ddr_training_delay_ps[0].write_dqs_delay[4]=0x00000080,// 128
	.cfg_ddr_training_delay_ps[0].write_dqs_delay[5]=0x00000080,// 128
	.cfg_ddr_training_delay_ps[0].write_dqs_delay[6]=0x00000080,// 128
	.cfg_ddr_training_delay_ps[0].write_dqs_delay[7]=0x00000080,// 128
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[0]=0x0000009f,// 159
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[1]=0x00000095,// 149
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[2]=0x00000092,// 146
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[3]=0x0000009f,// 159
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[4]=0x000000bb,// 187
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[5]=0x000000cd,// 205
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[6]=0x000000b1,// 177
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[7]=0x000000c5,// 197
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[8]=0x000000ad,// 173
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[9]=0x000000a1,// 161
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[10]=0x000000d3,// 211
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[11]=0x000000a2,// 162
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[12]=0x000000d1,// 209
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[13]=0x000000d3,// 211
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[14]=0x000000d0,// 208
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[15]=0x000000a7,// 167
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[16]=0x000000a9,// 169
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[17]=0x000000b9,// 185
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[18]=0x000000ae,// 174
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[19]=0x000000b1,// 177
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[20]=0x000000b2,// 178
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[21]=0x000000bd,// 189
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[22]=0x000000d1,// 209
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[23]=0x000000d0,// 208
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[24]=0x000000c6,// 198
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[25]=0x000000d1,// 209
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[26]=0x000000bf,// 191
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[27]=0x000000c1,// 193
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[28]=0x000000d3,// 211
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[29]=0x000000c1,// 193
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[30]=0x000000dc,// 220
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[31]=0x000000ae,// 174
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[32]=0x000000a1,// 161
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[33]=0x000000ab,// 171
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[34]=0x000000ac,// 172
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[35]=0x000000bd,// 189
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[36]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[37]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[38]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[39]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[40]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[41]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[42]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[43]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[44]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[45]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[46]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[47]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[48]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[49]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[50]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[51]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[52]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[53]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[54]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[55]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[56]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[57]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[58]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[59]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[60]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[61]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[62]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[63]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[64]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[65]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[66]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[67]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[68]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[69]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[70]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[71]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dqs_gate_delay[0]=0x0000037a,// 890
	.cfg_ddr_training_delay_ps[0].read_dqs_gate_delay[1]=0x00000359,// 857
	.cfg_ddr_training_delay_ps[0].read_dqs_gate_delay[2]=0x00000352,// 850
	.cfg_ddr_training_delay_ps[0].read_dqs_gate_delay[3]=0x0000034f,// 847
	.cfg_ddr_training_delay_ps[0].read_dqs_gate_delay[4]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dqs_gate_delay[5]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dqs_gate_delay[6]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dqs_gate_delay[7]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dqs_delay[0]=0x00000075,// 117
	.cfg_ddr_training_delay_ps[0].read_dqs_delay[1]=0x0000006c,// 108
	.cfg_ddr_training_delay_ps[0].read_dqs_delay[2]=0x00000058,// 88
	.cfg_ddr_training_delay_ps[0].read_dqs_delay[3]=0x0000007c,// 124
	.cfg_ddr_training_delay_ps[0].read_dqs_delay[4]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dqs_delay[5]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dqs_delay[6]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dqs_delay[7]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[0]=0x00000034,// 52
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[1]=0x00000033,// 51
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[2]=0x0000002b,// 43
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[3]=0x00000032,// 50
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[4]=0x00000050,// 80
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[5]=0x00000062,// 98
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[6]=0x00000043,// 67
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[7]=0x00000058,// 88
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[8]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[9]=0x00000028,// 40
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[10]=0x00000047,// 71
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[11]=0x00000028,// 40
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[12]=0x00000036,// 54
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[13]=0x00000044,// 68
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[14]=0x00000042,// 66
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[15]=0x00000026,// 38
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[16]=0x0000002c,// 44
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[17]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[18]=0x0000002a,// 42
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[19]=0x00000032,// 50
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[20]=0x00000029,// 41
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[21]=0x0000002c,// 44
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[22]=0x00000048,// 72
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[23]=0x0000003e,// 62
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[24]=0x0000003b,// 59
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[25]=0x00000046,// 70
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[26]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[27]=0x0000003b,// 59
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[28]=0x00000050,// 80
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[29]=0x0000003d,// 61
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[30]=0x00000047,// 71
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[31]=0x00000030,// 48
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[32]=0x0000002e,// 46
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[33]=0x0000002f,// 47
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[34]=0x00000033,// 51
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[35]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[36]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[37]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[38]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[39]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[40]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[41]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[42]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[43]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[44]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[45]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[46]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[47]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[48]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[49]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[50]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[51]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[52]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[53]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[54]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[55]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[56]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[57]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[58]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[59]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[60]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[61]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[62]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[63]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[64]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[65]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[66]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[67]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[68]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[69]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[70]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[71]=0x00000000,// 0

	.cfg_ddr_training_delay_ps[0].	soc_bit_vref = {
	0,40,40,40,40,40,40,40,40,40,40,
	40,40,40,40,40,40,40,40,40,40,40,
	40,40,40,40,40,40,40,40,40,40,40,
	40,40,40,40,50,40,50,40,50,40,50},//total 44
	.cfg_ddr_training_delay_ps[0].	dram_bit_vref = {
	0,32,32,32,32,32,32,32,32,32,32,
	32,32,32,32,32,32,32,32,32,32,32,
	32,32,32,32,32,32,32,32,32,32,32,
	32,32,32},//total 36
	.cfg_ddr_training_delay_ps[0].reserve_training_parameter = {
		(0 << 7) | 6, (0 << 7) | 6, (1 << 7) | 5, (0 << 7) | 0,
		(0 << 7) | 0, (0 << 7) | 0, (0 << 7) | 0, (0 << 7) | 0,
		(1 << 7) | 10, (1 << 7) | 10, (1 << 7) | 10, (1 << 7) | 10,
		(0 << 7) | 0, (0 << 7) | 0, (0 << 7) | 0, (0 << 7) | 0,
	},

	//pinmux setting
	.cfg_board_common_setting.	ac_pinmux	= {
	0,	0,	0,	1,	2,	3,
	8,	27,	10,	2,	9,	21,	5,	14,	1,	0,	25,	13,	12,	4,	7,	22,	0,	0,	0,	0,	6,	3,	20,	0,	0,	15,	26,	11,
	},
	.cfg_board_common_setting.	ddr_dqs_swap	=	0,

	.cfg_board_common_setting.	ddr_dq_remap	=	{
	0,1,2,3,4,5,6,7,
	8,9,10,11,12,13,14,15,
	16,17,18,19,20,21,22,23,
	24,25,26,27,28,29,30,31,
	32,33,34,35
	},//d0-d31 dm0 dm1 dm2 dm3

	.cfg_board_common_setting.	timming_magic				=	0x66223333,//ddr_set_t start
	.cfg_board_common_setting.	timming_max_valid_configs	=	TIMMING_MAX_CONFIG,//sizeof(__ddr_setting)/sizeof(ddr_set_t),
	.cfg_board_common_setting.	timming_struct_version		=	0,
	.cfg_board_common_setting.	timming_struct_org_size		=	sizeof(ddr_set_t),
	.cfg_board_common_setting.	timming_struct_real_size	=	0,	//0
	.cfg_board_common_setting.	ddr_func					=	0,
	.cfg_board_common_setting.	DisabledDbyte				=	CONFIG_DISABLE_D32_D63,
	.cfg_board_common_setting.	dram_cs0_base_add			=	0,
	.cfg_board_common_setting.	dram_cs1_base_add			=	0,

	.cfg_board_common_setting.	Is2Ttiming					=	CONFIG_USE_DDR_2T_MODE,
	.cfg_board_common_setting.	log_level					=	LOG_LEVEL_BASIC,
	.cfg_board_common_setting.	org_tdqs2dq					=	0,
	.cfg_board_common_setting.	reserve1_test_function		=	{0},
	.cfg_board_common_setting.	ddr_vddee_setting			=	{0},

	//DDR frequercy 2
	.cfg_board_SI_setting_ps[1].DRAMFreq					=	600,
	.cfg_board_SI_setting_ps[1].PllBypassEn					=	0,
	.cfg_board_SI_setting_ps[1].training_SequenceCtrl		=	0,
	.cfg_board_SI_setting_ps[1].ddr_odt_config				=	DDR_DRAM_ODT_W_CS0_ODT0,
	.cfg_board_SI_setting_ps[1].clk_drv_ohm					=	DDR_SOC_AC_DRV_40_OHM,
	.cfg_board_SI_setting_ps[1].cs_drv_ohm					=	DDR_SOC_AC_DRV_40_OHM,
	.cfg_board_SI_setting_ps[1].ac_drv_ohm					=	DDR_SOC_AC_DRV_40_OHM,
	.cfg_board_SI_setting_ps[1].soc_data_drv_ohm_p			=	DDR_SOC_DATA_DRV_ODT_40_OHM,
	.cfg_board_SI_setting_ps[1].soc_data_drv_ohm_n			=	DDR_SOC_DATA_DRV_ODT_40_OHM,
	.cfg_board_SI_setting_ps[1].soc_data_odt_ohm_p			=	DDR_SOC_DATA_DRV_ODT_120_OHM,
	.cfg_board_SI_setting_ps[1].soc_data_odt_ohm_n			=	DDR_SOC_DATA_DRV_ODT_0_OHM,
	.cfg_board_SI_setting_ps[1].dram_data_drv_ohm			=	DDR_DRAM_DDR4_DRV_34_OHM,
	.cfg_board_SI_setting_ps[1].dram_data_odt_ohm			=	DDR_DRAM_DDR4_ODT_60_OHM,
	.cfg_board_SI_setting_ps[1].dram_data_wr_odt_ohm		=	DDR_DRAM_DDR_WR_ODT_0_OHM,
	.cfg_board_SI_setting_ps[1].dram_ac_odt_ohm				=	DDR_DRAM_DDR_AC_ODT_0_OHM,
	.cfg_board_SI_setting_ps[1].dram_data_drv_pull_up_calibration_ohm	=	DDR_DRAM_LPDDR4_ODT_40_OHM,
	.cfg_board_SI_setting_ps[1].lpddr4_dram_vout_voltage_range_setting	=	DDR_DRAM_LPDDR4_OUTPUT_1_3_VDDQ,
	.cfg_board_SI_setting_ps[1].reserve2					=	0,
	.cfg_board_SI_setting_ps[1].vref_ac_permil 				=	0,
	.cfg_board_SI_setting_ps[1].vref_soc_data_permil 		=	0,
	.cfg_board_SI_setting_ps[1].vref_dram_data_permil		=	0,
	.cfg_board_SI_setting_ps[1].max_core_timmming_frequency				=	0,
	.cfg_board_SI_setting_ps[1].training_phase_parameter				=	{0},
	.cfg_board_SI_setting_ps[1].ac_trace_delay_org = {
	128,128,128,128,128,128,128,128,
	128,128,128,128,128,128,128,128,
	128,128,128,128,128,128,128,128,
	128,128,128,128,128,128,128,128,
	128,128,128,128,
	},//total 36

	.cfg_ddr_training_delay_ps[1]. ac_trace_delay = {
	128,128,128,128,128,128,128,128,
	128,128,128,128,128,128,128,128,
	128,128,128,128,128,128,128,128,
	128,128,128,128,128,128,128,128,
	128,128,128,128,
	},//total 36
	.cfg_ddr_training_delay_ps[1]. write_dqs_delay = {0,0,0,0,0,0,0,0},
	//.cfg_ddr_training_delay_ps[0]. write_dqs_delay = {32,32,32,32,32,32,32,32},
	.cfg_ddr_training_delay_ps[1]. write_dq_bit_delay = {
	50,50,50,50,50,50,50,50,
	50,50,50,50,50,50,50,50,
	50,50,50,50,50,50,50,50,
	50,50,50,50,50,50,50,50,
	50,50,50,50,50,50,50,50,
	50,50,50,50,50,50,50,50,
	50,50,50,50,50,50,50,50,
	50,50,50,50,50,50,50,50,
	50,50,50,50,50,50,50,50,},

	//.cfg_ddr_training_delay_ps[0]. read_dqs_gate_delay = {256,256,256,256,256,256,256,256},//total 8
	.cfg_ddr_training_delay_ps[1]. read_dqs_gate_delay = {192,192,192,192,192,192,192,192},//total 8
	//.cfg_ddr_training_delay_ps[0]. read_dqs_gate_delay = {64,64,64,64,64,64,64,64},//total 8
	.cfg_ddr_training_delay_ps[1]. read_dqs_delay = {64,64,64,64,64,64,64,64},//total 8
	.cfg_ddr_training_delay_ps[1]. read_dq_bit_delay = {
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0},//total 72
	.cfg_ddr_training_delay_ps[1]. soc_bit_vref = {
	40,40,40,40,40,40,40,40,40,40,40,
	40,40,40,40,40,40,40,40,40,40,40,
	40,40,40,40,40,40,40,40,40,40,40,
	40,40,40,40,40,40,40,40,40,40,40},//total 44
	.cfg_ddr_training_delay_ps[1]. dram_bit_vref = {
	32,32,32,32,32,32,32,32,32,32,32,
	32,32,32,32,32,32,32,32,32,32,32,
	32,32,32,32,32,32,32,32,32,32,32,
	32,32,32},//total 36

	.cfg_ddr_training_delay_ps[1].reserve_training_parameter	=	{0},//ddr_set_t end
},
#endif //end ENABLE_DDR4
#if ENABLE_DDR3
{
	.cfg_board_common_setting.timming_magic = 0,
	.cfg_board_common_setting.timming_max_valid_configs = TIMMING_MAX_CONFIG,
	.cfg_board_common_setting.timming_struct_version = 0,
	.cfg_board_common_setting.timming_struct_org_size = sizeof(ddr_set_t),
	.cfg_board_common_setting.timming_struct_real_size = 0,	//0
	.cfg_board_common_setting.fast_boot = {0},
	.cfg_board_common_setting.ddr_func = 0,
	.cfg_board_common_setting.board_id = CONFIG_BOARD_ID_MASK,
	.cfg_board_common_setting.DramType = CONFIG_DDR_TYPE_DDR3,
	.cfg_board_common_setting.dram_rank_config = CONFIG_DDR0_32BIT_16BIT_RANK0_CH0,
	.cfg_board_common_setting.DisabledDbyte = CONFIG_DISABLE_D32_D63,
	.cfg_board_common_setting.dram_cs0_base_add = 0,
	.cfg_board_common_setting.dram_cs1_base_add = 0,
	.cfg_board_common_setting.dram_cs0_size_MB = CONFIG_DDR0_SIZE_768MB,
	.cfg_board_common_setting.dram_cs1_size_MB = CONFIG_DDR1_SIZE_0MB,
	.cfg_board_common_setting.dram_x4x8x16_mode = CONFIG_DRAM_MODE_X16,
	.cfg_board_common_setting.Is2Ttiming = CONFIG_USE_DDR_2T_MODE,
	.cfg_board_common_setting.log_level = LOG_LEVEL_BASIC,
	.cfg_board_common_setting.ddr_rdbi_wr_enable = DDR_WRITE_READ_DBI_DISABLE,
	.cfg_board_common_setting.pll_ssc_mode = DDR_PLL_SSC_DISABLE,
	.cfg_board_common_setting.org_tdqs2dq = 0,
	.cfg_board_common_setting.reserve1_test_function = {0},
	.cfg_board_common_setting.ddr_dmc_remap = DDR_DMC_REMAP_DDR3_32BIT,
	.cfg_board_common_setting.ac_pinmux =
	{
		0, 0, 0, 1, 2, 3, 0, 1,
		2, 3, 4, 5, 6, 7, 8, 9,
		10, 11, 12, 13, 14, 15, 0, 0,
		0, 0, 20, 21, 22, 0, 0, 25,
		26, 27, 0
	},
	.cfg_board_common_setting.ddr_dqs_swap = 0,
	.cfg_board_common_setting.ddr_dq_remap = {
		0, 1, 2, 3, 4, 5, 6, 7,
		8, 9, 10, 11, 12, 13, 14, 15,
		16, 17, 18, 19, 20, 21, 22, 23,
		24, 25, 26, 27, 28, 29, 30, 31,
		32, 33, 34, 35,
	},	 //d0-d31 dm0 dm1 dm2 dm3
	.cfg_board_common_setting.ddr_vddee_setting = {0},
	.cfg_board_SI_setting_ps[0].DRAMFreq = 912,
	.cfg_board_SI_setting_ps[0].PllBypassEn = 0,
	.cfg_board_SI_setting_ps[0].training_SequenceCtrl = 0,
	.cfg_board_SI_setting_ps[0].ddr_odt_config = DDR_DRAM_ODT_W_CS0_ODT0,
	.cfg_board_SI_setting_ps[0].clk_drv_ohm = DDR_SOC_AC_DRV_40_OHM,
	.cfg_board_SI_setting_ps[0].cs_drv_ohm = DDR_SOC_AC_DRV_40_OHM,
	.cfg_board_SI_setting_ps[0].ac_drv_ohm = DDR_SOC_AC_DRV_40_OHM,
	.cfg_board_SI_setting_ps[0].soc_data_drv_ohm_p = DDR_SOC_DATA_DRV_ODT_40_OHM,
	.cfg_board_SI_setting_ps[0].soc_data_drv_ohm_n = DDR_SOC_DATA_DRV_ODT_40_OHM,
	.cfg_board_SI_setting_ps[0].soc_data_odt_ohm_p = DDR_SOC_DATA_DRV_ODT_120_OHM,
	.cfg_board_SI_setting_ps[0].soc_data_odt_ohm_n = DDR_SOC_DATA_DRV_ODT_120_OHM,
	.cfg_board_SI_setting_ps[0].dram_data_drv_ohm = DDR_DRAM_DDR3_DRV_40_OHM,
	.cfg_board_SI_setting_ps[0].dram_data_odt_ohm = DDR_DRAM_DDR3_ODT_60_OHM,
	.cfg_board_SI_setting_ps[0].dram_data_wr_odt_ohm = DDR_DRAM_DDR_WR_ODT_0_OHM,
	.cfg_board_SI_setting_ps[0].dram_ac_odt_ohm = DDR_DRAM_DDR_AC_ODT_0_OHM,
	.cfg_board_SI_setting_ps[0].dram_data_drv_pull_up_calibration_ohm = DDR_DRAM_LPDDR4_ODT_40_OHM,
	.cfg_board_SI_setting_ps[0].lpddr4_dram_vout_voltage_range_setting = DDR_DRAM_LPDDR4_OUTPUT_1_3_VDDQ,
	.cfg_board_SI_setting_ps[0].reserve2 = 0,
	.cfg_board_SI_setting_ps[0].vref_ac_permil = 0,
	.cfg_board_SI_setting_ps[0].vref_soc_data_permil = 0,
	.cfg_board_SI_setting_ps[0].vref_dram_data_permil = 0,
	.cfg_board_SI_setting_ps[0].max_core_timmming_frequency = 0,
	.cfg_board_SI_setting_ps[0].training_phase_parameter = {0},
	.cfg_board_SI_setting_ps[0].ac_trace_delay_org = {
		128, 128, 128 - 20, 128, 128, 128, 128, 128,
		384, 384, 384, 384, 384, 384, 384, 384,
		384, 384, 384, 384, 384, 384, 384, 384,
		384, 384, 384, 384, 384, 384, 384, 384,
		384, 384, 384, 384,
	}, //total 36

	.cfg_ddr_training_delay_ps[0].ac_trace_delay = {
		128, 128, 128 - 20, 128, 128, 128, 128, 128,
		384, 384, 384, 384, 384, 384, 384, 384,
		384, 384, 384, 384, 384, 384, 384, 384,
		384, 384, 384, 384, 384, 384, 384, 384,
		384, 384, 384, 384,
	}, //total 36
	.cfg_ddr_training_delay_ps[0].write_dqs_delay = {
		128 - 1, 128 - 1, 128 + 48, 128 + 58, 128 + 32, 128 + 32, 128 + 32, 128 + 32
	},
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay = {
		96 + 128, 96 + 128, 96 + 128, 96 + 128, 96 + 128, 96 + 128, 96 + 128, 96 + 128,
		96 + 128, 96 + 128, 96 + 128, 96 + 128, 96 + 128, 96 + 128, 96 + 128, 96 + 128,
		96 + 128, 96 + 128, 128 + 128, 128 + 128, 128 + 128, 128 + 128, 128 + 128, 128 + 128,
		128 + 128, 128 + 128, 128 + 128, 128 + 128, 128 + 128, 128 + 128, 128 + 128, 128 + 128,
		128 + 128, 128 + 128, 128 + 128, 128 + 128, 96 + 128, 96 + 128, 96 + 128, 96 + 128,
		96 + 128, 96 + 128, 96 + 128, 96 + 128, 96 + 128, 96 + 128, 96 + 128, 96 + 128,
		96 + 128, 96 + 128, 96 + 128, 96 + 128, 96 + 128, 96 + 128, 96 + 128, 96 + 128,
		96 + 128, 96 + 128, 96 + 128, 96 + 128, 96 + 128, 96 + 128, 96 + 128, 96 + 128,
		96 + 128, 96 + 128, 96 + 128, 96 + 128, 96 + 128, 96 + 128, 96 + 128, 96 + 128,
	},
	.cfg_ddr_training_delay_ps[0].read_dqs_gate_delay = {
		896, 896, 896, 896, 896, 896, 896, 896
	}, //total 8
	.cfg_ddr_training_delay_ps[0].read_dqs_delay = {
		64, 64, 50, 40, 64, 64, 64, 64
	}, //total 8
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay = {
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0
	}, //total 72
	.cfg_ddr_training_delay_ps[0].soc_bit_vref = {
		32, 32, 32, 32, 32, 32, 32, 32,
		32, 32, 32, 32, 32, 32, 32, 32,
		32, 32, 32, 32, 32, 32, 32, 32,
		32, 32, 32, 32, 32, 32, 32, 32,
		32, 32, 32, 32, 32, 32 + 5, 32, 32 + 5,
		32, 32 + 5, 32, 32 + 5
	},
	.cfg_ddr_training_delay_ps[0].dram_bit_vref = {
		32, 32, 32, 32, 32, 32, 32, 32,
		32, 32, 32, 32, 32, 32, 32, 32,
		32, 32, 32, 32, 32, 32, 32, 32,
		32, 32, 32, 32, 32, 32, 32, 32,
		32, 32, 32, 32
	}, //total 36
	.cfg_ddr_training_delay_ps[0].write_dqs_delay[0] = 0x0000007d,// 125
	.cfg_ddr_training_delay_ps[0].write_dqs_delay[1] = 0x00000091,// 145
	.cfg_ddr_training_delay_ps[0].write_dqs_delay[2] = 0x00000091,// 145
	.cfg_ddr_training_delay_ps[0].write_dqs_delay[3] = 0x0000009b,// 155
	.cfg_ddr_training_delay_ps[0].write_dqs_delay[4] = 0x000000a0,// 160
	.cfg_ddr_training_delay_ps[0].write_dqs_delay[5] = 0x000000a0,// 160
	.cfg_ddr_training_delay_ps[0].write_dqs_delay[6] = 0x000000a0,// 160
	.cfg_ddr_training_delay_ps[0].write_dqs_delay[7] = 0x000000a0,// 160
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[0] = 0x000000c2,// 194
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[1] = 0x000000b0,// 176
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[2] = 0x000000af,// 175
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[3] = 0x000000b0,// 176
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[4] = 0x000000c0,// 192
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[5] = 0x000000c3,// 195
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[6] = 0x000000b4,// 180
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[7] = 0x000000be,// 190
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[8] = 0x000000ba,// 186
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[9] = 0x000000c1,// 193
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[10] = 0x000000d5,// 213
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[11] = 0x000000c1,// 193
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[12] = 0x000000d0,// 208
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[13] = 0x000000cc,// 204
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[14] = 0x000000c3,// 195
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[15] = 0x000000be,// 190
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[16] = 0x000000c0,// 192
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[17] = 0x000000c8,// 200
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[18] = 0x000000e0,// 224
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[19] = 0x000000e5,// 229
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[20] = 0x000000e2,// 226
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[21] = 0x000000ec,// 236
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[22] = 0x000000ec,// 236
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[23] = 0x000000e7,// 231
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[24] = 0x000000e4,// 228
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[25] = 0x000000e9,// 233
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[26] = 0x000000e6,// 230
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[27] = 0x000000eb,// 235
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[28] = 0x000000f5,// 245
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[29] = 0x000000e7,// 231
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[30] = 0x000000fd,// 253
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[31] = 0x000000e8,// 232
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[32] = 0x000000e4,// 228
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[33] = 0x000000ee,// 238
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[34] = 0x000000ec,// 236
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[35] = 0x000000f2,// 242
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[36] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[37] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[38] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[39] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[40] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[41] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[42] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[43] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[44] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[45] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[46] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[47] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[48] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[49] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[50] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[51] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[52] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[53] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[54] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[55] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[56] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[57] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[58] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[59] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[60] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[61] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[62] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[63] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[64] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[65] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[66] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[67] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[68] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[69] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[70] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[71] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dqs_gate_delay[0] = 0x0000038c,// 908
	.cfg_ddr_training_delay_ps[0].read_dqs_gate_delay[1] = 0x0000037a,// 890
	.cfg_ddr_training_delay_ps[0].read_dqs_gate_delay[2] = 0x0000039a,// 922
	.cfg_ddr_training_delay_ps[0].read_dqs_gate_delay[3] = 0x00000395,// 917
	.cfg_ddr_training_delay_ps[0].read_dqs_gate_delay[4] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dqs_gate_delay[5] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dqs_gate_delay[6] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dqs_gate_delay[7] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dqs_delay[0] = 0x0000007f-0x30,// 127
	.cfg_ddr_training_delay_ps[0].read_dqs_delay[1] = 0x0000007d-0x30,// 125
	.cfg_ddr_training_delay_ps[0].read_dqs_delay[2] = 0x0000006d-0x30,// 109
	.cfg_ddr_training_delay_ps[0].read_dqs_delay[3] = 0x0000006f-0x30,// 111
	.cfg_ddr_training_delay_ps[0].read_dqs_delay[4] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dqs_delay[5] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dqs_delay[6] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dqs_delay[7] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[0] = 0x00000048-0x30,// 72
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[1] = 0x00000044-0x30,// 68
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[2] = 0x00000038-0x30,// 56
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[3] = 0x0000003b-0x30,// 59
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[4] = 0x00000049-0x30,// 73
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[5] = 0x0000004b-0x30,// 75
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[6] = 0x0000003b-0x30,// 59
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[7] = 0x00000049-0x30,// 73
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[8] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[9] = 0x00000040-0x30,// 64
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[10] = 0x00000052-0x30,// 82
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[11] = 0x00000039-0x30,// 57
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[12] = 0x00000040-0x30,// 64
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[13] = 0x00000045-0x30,// 69
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[14] = 0x00000042-0x30,// 66
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[15] = 0x00000034-0x30,// 52
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[16] = 0x0000003c-0x30,// 60
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[17] = 0x00000086-0x30,// 134
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[18] = 0x0000003f-0x30,// 63
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[19] = 0x00000048-0x30,// 72
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[20] = 0x00000037-0x30,// 55
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[21] = 0x00000048-0x30,// 72
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[22] = (0x0000004e)-0x30,// 78
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[23] = 0x00000044-0x30,// 68
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[24] = 0x00000042-0x30,// 66
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[25] = 0x00000047-0x30,// 71
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[26] = 0x0000007f-0x30,// 127
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[27] = 0x00000040-0x30,// 64
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[28] = 0x00000052-0x30,// 82
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[29] = 0x0000003d-0x30,// 61
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[30] = (0x0000004e)-0x30,// 78
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[31] = 0x0000003c-0x30,// 60
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[32] = (0x0000003e)-0x30,// 62
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[33] = 0x0000003f-0x30,// 63
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[34] = 0x0000004c-0x30,// 76
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[35] = 0x00000091-0x30,// 145
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[36] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[37] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[38] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[39] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[40] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[41] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[42] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[43] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[44] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[45] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[46] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[47] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[48] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[49] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[50] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[51] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[52] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[53] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[54] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[55] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[56] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[57] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[58] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[59] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[60] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[61] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[62] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[63] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[64] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[65] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[66] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[67] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[68] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[69] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[70] = 0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[71] = 0x00000000,// 0

	.cfg_ddr_training_delay_ps[0].reserve_training_parameter = {0},
	.cfg_board_SI_setting_ps[1].DRAMFreq = 667,
	.cfg_board_SI_setting_ps[1].PllBypassEn = 0,
	.cfg_board_SI_setting_ps[1].training_SequenceCtrl = 0,
	.cfg_board_SI_setting_ps[1].ddr_odt_config = DDR_DRAM_ODT_W_CS0_ODT0,
	.cfg_board_SI_setting_ps[1].clk_drv_ohm = DDR_SOC_AC_DRV_40_OHM,
	.cfg_board_SI_setting_ps[1].cs_drv_ohm = DDR_SOC_AC_DRV_40_OHM,
	.cfg_board_SI_setting_ps[1].ac_drv_ohm = DDR_SOC_AC_DRV_40_OHM,
	.cfg_board_SI_setting_ps[1].soc_data_drv_ohm_p = DDR_SOC_DATA_DRV_ODT_40_OHM,
	.cfg_board_SI_setting_ps[1].soc_data_drv_ohm_n = DDR_SOC_DATA_DRV_ODT_40_OHM,
	.cfg_board_SI_setting_ps[1].soc_data_odt_ohm_p = DDR_SOC_DATA_DRV_ODT_60_OHM,
	.cfg_board_SI_setting_ps[1].soc_data_odt_ohm_n = DDR_SOC_DATA_DRV_ODT_0_OHM,
	.cfg_board_SI_setting_ps[1].dram_data_drv_ohm = DDR_DRAM_DDR4_DRV_34_OHM,
	.cfg_board_SI_setting_ps[1].dram_data_odt_ohm = DDR_DRAM_DDR4_ODT_60_OHM,
	.cfg_board_SI_setting_ps[1].dram_data_wr_odt_ohm = DDR_DRAM_DDR_WR_ODT_0_OHM,
	.cfg_board_SI_setting_ps[1].dram_ac_odt_ohm = DDR_DRAM_DDR_AC_ODT_0_OHM,
	.cfg_board_SI_setting_ps[1].dram_data_drv_pull_up_calibration_ohm = DDR_DRAM_LPDDR4_ODT_40_OHM,
	.cfg_board_SI_setting_ps[1].lpddr4_dram_vout_voltage_range_setting = DDR_DRAM_LPDDR4_OUTPUT_1_3_VDDQ,
	.cfg_board_SI_setting_ps[1].reserve2 = 0,
	.cfg_board_SI_setting_ps[1].vref_ac_permil = 0,
	.cfg_board_SI_setting_ps[1].vref_soc_data_permil = 0,
	.cfg_board_SI_setting_ps[1].vref_dram_data_permil = 0,
	.cfg_board_SI_setting_ps[1].max_core_timmming_frequency = 0,
	.cfg_board_SI_setting_ps[1].training_phase_parameter = {0},
	.cfg_board_SI_setting_ps[1].ac_trace_delay_org = {
		128, 128, 128, 128, 128, 128, 128, 128,
		128, 128, 128, 128, 128, 128, 128, 128,
		128, 128, 128, 128, 128, 128, 128, 128,
		128, 128, 128, 128, 128, 128, 128, 128,
		128, 128, 128, 128,
	}, //total 36
	.cfg_ddr_training_delay_ps[1].ac_trace_delay = {
		128, 128, 128, 128, 128, 128, 128, 128,
		128, 128, 128, 128, 128, 128, 128, 128,
		128, 128, 128, 128, 128, 128, 128, 128,
		128, 128, 128, 128, 128, 128, 128, 128,
		128, 128, 128, 128,
	}, //total 36
	.cfg_ddr_training_delay_ps[1].write_dqs_delay = {
		0, 0, 0, 0, 0, 0, 0, 0
	},
	.cfg_ddr_training_delay_ps[1].write_dq_bit_delay = {
		50, 50, 50, 50, 50, 50, 50, 50,
		50, 50, 50, 50, 50, 50, 50, 50,
		50, 50, 50, 50, 50, 50, 50, 50,
		50, 50, 50, 50, 50, 50, 50, 50,
		50, 50, 50, 50, 50, 50, 50, 50,
		50, 50, 50, 50, 50, 50, 50, 50,
		50, 50, 50, 50, 50, 50, 50, 50,
		50, 50, 50, 50, 50, 50, 50, 50,
		50, 50, 50, 50, 50, 50, 50, 50
	},
	.cfg_ddr_training_delay_ps[1].read_dqs_gate_delay = {
		192, 192, 192, 192, 192, 192, 192, 192
	}, //total 8
	.cfg_ddr_training_delay_ps[1].read_dqs_delay = {
		64, 64, 64, 64, 64, 64, 64, 64
	}, //total 8
	.cfg_ddr_training_delay_ps[1].read_dq_bit_delay = {
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0
	},  //total 72
	.cfg_ddr_training_delay_ps[1].soc_bit_vref = {
		40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
		40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
		40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
		40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40
	},  //total 44
	.cfg_ddr_training_delay_ps[1].dram_bit_vref = {
		32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
		32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
		32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
		32, 32, 32
	},  //total 36
	.cfg_ddr_training_delay_ps[1].reserve_training_parameter = {0},
},
#endif

#if ENABLE_LPDDR4  //LPDDR4 rank0,timing_config
{
	.cfg_board_common_setting.timming_magic					= 0,
	.cfg_board_common_setting.timming_max_valid_configs			= TIMMING_MAX_CONFIG,
	.cfg_board_common_setting.timming_struct_version			= 0,
	.cfg_board_common_setting.timming_struct_org_size			= sizeof(ddr_set_t),
	.cfg_board_common_setting.timming_struct_real_size			= 0,                                       //0
	.cfg_board_common_setting.fast_boot					= { 0 },
	.cfg_board_common_setting.ddr_func					= 0,
	.cfg_board_common_setting.board_id					= CONFIG_BOARD_ID_MASK,
	.cfg_board_common_setting.DramType					= CONFIG_DDR_TYPE_LPDDR4,
	.cfg_board_common_setting.dram_rank_config				= CONFIG_DDR0_32BIT_RANK0_CH01,
	.cfg_board_common_setting.DisabledDbyte					= CONFIG_DISABLE_D32_D63,
	.cfg_board_common_setting.dram_cs0_base_add				= 0,
	.cfg_board_common_setting.dram_cs1_base_add				= 0,
	.cfg_board_common_setting.dram_cs0_size_MB				= CONFIG_DDR0_SIZE_1024MB,
	.cfg_board_common_setting.dram_cs1_size_MB				= CONFIG_DDR1_SIZE_0MB,
	.cfg_board_common_setting.dram_x4x8x16_mode				= CONFIG_DRAM_MODE_X16,
	.cfg_board_common_setting.Is2Ttiming					= CONFIG_USE_DDR_1T_MODE,
	.cfg_board_common_setting.log_level					= LOG_LEVEL_BASIC,
	.cfg_board_common_setting.ddr_rdbi_wr_enable				= DDR_WRITE_READ_DBI_DISABLE,              //DDR_WRITE_READ_DBI_DISABLE	,
	.cfg_board_common_setting.pll_ssc_mode					= DDR_PLL_SSC_DISABLE,
	.cfg_board_common_setting.org_tdqs2dq					= 0,
	.cfg_board_common_setting.reserve1_test_function			= { 0 },
	.cfg_board_common_setting.ddr_dmc_remap					= DDR_DMC_REMAP_LPDDR4_32BIT,
#ifdef CONFIG_DDR_WINDOWING_BOOT
	.cfg_board_common_setting.fast_boot[0]                                  = 0x1,
	.cfg_board_common_setting.fast_boot[3]                                  = 0x46,
#endif
	//lpddr4 ac pinmux
	.cfg_board_common_setting.ac_pinmux					= {
		0, 0, 0, 1, 0, 1, 1, 3,
		0, 4, 4, 2, 0, 0, 1, 0,
		2, 0, 0, 0, 0, 3, 0, 0,
		0, 0, 5, 5, 0, 0, 0, 0,
		0, 0,
	},
	.cfg_board_common_setting.ddr_dqs_swap					= 0,

	.cfg_board_common_setting.ddr_dq_remap					= {
		1, 0, 3, 2, 7, 5, 6, 4,
		9, 15, 13, 14, 10, 11, 12, 8,
		21, 19, 20, 18, 17, 16, 22, 23,
		25, 26, 24, 27, 31, 29, 30, 28,
		32, 33, 34, 35
	},       //d0-d31 dm0 dm1 dm2 dm3

	.cfg_board_common_setting.ddr_vddee_setting				= { 0 },
	.cfg_board_SI_setting_ps[0].DRAMFreq = 1320,
	.cfg_board_SI_setting_ps[0].PllBypassEn = 0,
	.cfg_board_SI_setting_ps[0].training_SequenceCtrl = 0,
	.cfg_board_SI_setting_ps[0].ddr_odt_config = DDR_DRAM_ODT_W_CS0_ODT0,
	.cfg_board_SI_setting_ps[0].clk_drv_ohm = DDR_SOC_AC_DRV_40_OHM,
	.cfg_board_SI_setting_ps[0].cs_drv_ohm = DDR_SOC_AC_DRV_40_OHM,
	.cfg_board_SI_setting_ps[0].ac_drv_ohm = DDR_SOC_AC_DRV_40_OHM,
	.cfg_board_SI_setting_ps[0].soc_data_drv_ohm_p = DDR_SOC_DATA_DRV_ODT_40_OHM,
	.cfg_board_SI_setting_ps[0].soc_data_drv_ohm_n = DDR_SOC_DATA_DRV_ODT_40_OHM,
	.cfg_board_SI_setting_ps[0].soc_data_odt_ohm_p = DDR_SOC_DATA_DRV_ODT_0_OHM,
	.cfg_board_SI_setting_ps[0].soc_data_odt_ohm_n = DDR_SOC_DATA_DRV_ODT_60_OHM,
	.cfg_board_SI_setting_ps[0].dram_data_drv_ohm = DDR_DRAM_LPDDR4_DRV_40_OHM,
	.cfg_board_SI_setting_ps[0].dram_data_odt_ohm = DDR_DRAM_LPDDR4_ODT_60_OHM,
	.cfg_board_SI_setting_ps[0].dram_data_wr_odt_ohm = DDR_DRAM_DDR_WR_ODT_0_OHM,
	.cfg_board_SI_setting_ps[0].dram_ac_odt_ohm = DDR_DRAM_LPDDR4_AC_ODT_80_OHM,
	.cfg_board_SI_setting_ps[0].dram_data_drv_pull_up_calibration_ohm = DDR_DRAM_LPDDR4_ODT_40_OHM,
	.cfg_board_SI_setting_ps[0].lpddr4_dram_vout_voltage_range_setting = DDR_DRAM_LPDDR4_OUTPUT_1_3_VDDQ,
	.cfg_board_SI_setting_ps[0].reserve2 = 0,
	.cfg_board_SI_setting_ps[0].vref_ac_permil = 0,  //3000/11,
	.cfg_board_SI_setting_ps[0].vref_soc_data_permil = 0,  //170,
	.cfg_board_SI_setting_ps[0].vref_dram_data_permil = 0,  //280, //330
	.cfg_board_SI_setting_ps[0].max_core_timmming_frequency = 0,
	.cfg_board_SI_setting_ps[0].training_phase_parameter = { 0 },
	.cfg_board_SI_setting_ps[0].ac_trace_delay_org = {
		64 + 128,	64 + 128,	64 + 128,	64 + 128,	64 + 128,      64 + 128,      0 + 128,	     0 + 128,      //0,0,0,0,0,0,0,0,
		128 + 20 + 128, 128 + 20 + 128, 128 + 60 + 128, 128 + 20 + 128, 128 + 0 + 128, 128 + 0 + 128, 128 + 0 + 128, 128 + 0 + 128,
		128 + 0 + 128,	128 + 0 + 128,	128 + 0 + 128,	128 + 0 + 128,	128 + 0 + 128, 128 + 0 + 128, 128 + 0 + 128, 128 + 0 + 128,
		128 + 0 + 128,	128 + 0 + 128,	128 + 0 + 128,	128 + 0 + 128,	128 + 0 + 128, 128 + 0 + 128, 128 + 0 + 128, 128 + 0 + 128,
		128 + 0 + 128,	128 + 0 + 128,	128 + 0 + 128,	128 + 0 + 128,
	}, //total 36

	.cfg_ddr_training_delay_ps[0].ac_trace_delay = {
		64 + 128,	64 + 128,	64 + 128-64,	64 + 128,	64 + 128,      64 + 128,      0 + 128,	     128 + 128,   //0,0,0,0,0,0,0,0,
		128 + 20 + 128, 128 + 20 + 128, 128 + 60 + 128-40, 128 + 20 + 128-10, 128 + 0 + 128, 128 + 0 + 128+10, 128 + 0 + 128, 128 + 0 + 128,
		128 + 0 + 128,	128 + 0 + 128,	128 + 0 + 128,	128 + 0 + 128,	128 + 0 + 128, 128 + 0 + 128, 128 + 0 + 128, 128 + 0 + 128,
		128 + 0 + 128,	128 + 0 + 128,	128 + 0 + 128,	128 + 0 + 128,	128 + 0 + 128, 128 + 0 + 128, 128 + 0 + 128, 128 + 0 + 128,
		128 + 0 + 128,	128 + 0 + 128,	128 + 0 + 128,	128 + 0 + 128,
	}, //total 36
	.cfg_ddr_training_delay_ps[0].reserve_training_parameter = { 0 },

	.cfg_ddr_training_delay_ps[0].write_dqs_delay[0]=0x000000af,// 175
	.cfg_ddr_training_delay_ps[0].write_dqs_delay[1]=0x000000cd,// 205
	.cfg_ddr_training_delay_ps[0].write_dqs_delay[2]=0x000000aa,// 170
	.cfg_ddr_training_delay_ps[0].write_dqs_delay[3]=0x000000cd,// 205
	.cfg_ddr_training_delay_ps[0].write_dqs_delay[4]=0x000000af,// 175
	.cfg_ddr_training_delay_ps[0].write_dqs_delay[5]=0x000000cd,// 205
	.cfg_ddr_training_delay_ps[0].write_dqs_delay[6]=0x000000aa,// 170
	.cfg_ddr_training_delay_ps[0].write_dqs_delay[7]=0x000000cd,// 205
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[0]=0x000001cd,// 461
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[1]=0x000001cc,// 460
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[2]=0x000001ba,// 442
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[3]=0x000001d8,// 472
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[4]=0x000001e1,// 481
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[5]=0x000001ce,// 462
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[6]=0x000001c6,// 454
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[7]=0x000001d0,// 464
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[8]=0x000001cc,// 460
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[9]=0x000001d9,// 473
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[10]=0x000001da,// 474
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[11]=0x000001c4,// 452
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[12]=0x000001d5,// 469
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[13]=0x000001ea,// 490
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[14]=0x000001e9,// 489
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[15]=0x000001e2,// 482
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[16]=0x000001ea,// 490
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[17]=0x000001d5,// 469
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[18]=0x000001c6,// 454
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[19]=0x000001ce,// 462
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[20]=0x000001b5,// 437
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[21]=0x000001d7,// 471
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[22]=0x000001c7,// 455
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[23]=0x000001d3,// 467
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[24]=0x000001d1,// 465
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[25]=0x000001d7,// 471
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[26]=0x000001c4,// 452
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[27]=0x000001d7,// 471
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[28]=0x000001eb,// 491
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[29]=0x000001d1,// 465
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[30]=0x000001f7,// 503
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[31]=0x000001d8,// 472
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[32]=0x000001cf,// 463
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[33]=0x000001d3,// 467
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[34]=0x000001c9,// 457
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[35]=0x000001de,// 478
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[36]=0x000001cd,// 461
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[37]=0x000001cc,// 460
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[38]=0x000001ba,// 442
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[39]=0x000001d8,// 472
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[40]=0x000001e1,// 481
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[41]=0x000001ce,// 462
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[42]=0x000001c6,// 454
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[43]=0x000001d0,// 464
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[44]=0x000001cc,// 460
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[45]=0x000001d9,// 473
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[46]=0x000001da,// 474
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[47]=0x000001c4,// 452
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[48]=0x000001d5,// 469
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[49]=0x000001ea,// 490
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[50]=0x000001e9,// 489
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[51]=0x000001e2,// 482
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[52]=0x000001ea,// 490
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[53]=0x000001d5,// 469
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[54]=0x000001c6,// 454
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[55]=0x000001ce,// 462
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[56]=0x000001b5,// 437
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[57]=0x000001d7,// 471
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[58]=0x000001c7,// 455
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[59]=0x000001d3,// 467
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[60]=0x000001d1,// 465
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[61]=0x000001d7,// 471
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[62]=0x000001c4,// 452
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[63]=0x000001d7,// 471
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[64]=0x000001eb,// 491
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[65]=0x000001d1,// 465
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[66]=0x000001f7,// 503
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[67]=0x000001d8,// 472
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[68]=0x000001cf,// 463
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[69]=0x000001d3,// 467
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[70]=0x000001c9,// 457
	.cfg_ddr_training_delay_ps[0].write_dq_bit_delay[71]=0x000001de,// 478
	.cfg_ddr_training_delay_ps[0].read_dqs_gate_delay[0]=0x000003ab,// 939
	.cfg_ddr_training_delay_ps[0].read_dqs_gate_delay[1]=0x00000383,// 899
	.cfg_ddr_training_delay_ps[0].read_dqs_gate_delay[2]=0x00000395,// 917
	.cfg_ddr_training_delay_ps[0].read_dqs_gate_delay[3]=0x00000378,// 888
	.cfg_ddr_training_delay_ps[0].read_dqs_gate_delay[4]=0x000003ab,// 939
	.cfg_ddr_training_delay_ps[0].read_dqs_gate_delay[5]=0x00000383,// 899
	.cfg_ddr_training_delay_ps[0].read_dqs_gate_delay[6]=0x00000395,// 917
	.cfg_ddr_training_delay_ps[0].read_dqs_gate_delay[7]=0x00000378,// 888
	.cfg_ddr_training_delay_ps[0].read_dqs_delay[0]=0x0000007e,// 126
	.cfg_ddr_training_delay_ps[0].read_dqs_delay[1]=0x00000088,// 136
	.cfg_ddr_training_delay_ps[0].read_dqs_delay[2]=0x00000070,// 112
	.cfg_ddr_training_delay_ps[0].read_dqs_delay[3]=0x0000007a,// 122
	.cfg_ddr_training_delay_ps[0].read_dqs_delay[4]=0x0000007e,// 126
	.cfg_ddr_training_delay_ps[0].read_dqs_delay[5]=0x00000088,// 136
	.cfg_ddr_training_delay_ps[0].read_dqs_delay[6]=0x00000070,// 112
	.cfg_ddr_training_delay_ps[0].read_dqs_delay[7]=0x0000007a,// 122
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[0]=0x00000044,// 68
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[1]=0x00000051,// 81
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[2]=0x00000036,// 54
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[3]=0x00000046,// 70
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[4]=0x00000054,// 84
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[5]=0x00000047,// 71
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[6]=0x0000003d,// 61
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[7]=0x00000046,// 70
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[8]=0x00000045,// 69
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[9]=0x00000047,// 71
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[10]=0x00000051,// 81
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[11]=0x00000037,// 55
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[12]=0x0000003d,// 61
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[13]=0x00000052,// 82
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[14]=0x0000005b,// 91
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[15]=0x0000004c,// 76
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[16]=0x0000005a,// 90
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[17]=0x0000004b,// 75
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[18]=0x00000042,// 66
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[19]=0x0000004f,// 79
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[20]=0x00000030,// 48
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[21]=0x00000046,// 70
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[22]=0x00000042,// 66
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[23]=0x0000004c,// 76
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[24]=0x00000041,// 65
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[25]=0x00000058,// 88
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[26]=0x00000045,// 69
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[27]=0x00000041,// 65
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[28]=0x00000056,// 86
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[29]=0x0000003f,// 63
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[30]=0x00000055,// 85
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[31]=0x0000003e,// 62
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[32]=0x00000034,// 52
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[33]=0x00000031,// 49
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[34]=0x00000036,// 54
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[35]=0x00000040,// 64
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[36]=0x00000044,// 68
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[37]=0x00000051,// 81
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[38]=0x00000036,// 54
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[39]=0x00000046,// 70
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[40]=0x00000054,// 84
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[41]=0x00000047,// 71
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[42]=0x0000003d,// 61
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[43]=0x00000046,// 70
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[44]=0x00000045,// 69
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[45]=0x00000047,// 71
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[46]=0x00000051,// 81
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[47]=0x00000037,// 55
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[48]=0x0000003d,// 61
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[49]=0x00000052,// 82
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[50]=0x0000005b,// 91
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[51]=0x0000004c,// 76
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[52]=0x0000005a,// 90
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[53]=0x0000004b,// 75
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[54]=0x00000042,// 66
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[55]=0x0000004f,// 79
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[56]=0x00000030,// 48
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[57]=0x00000046,// 70
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[58]=0x00000042,// 66
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[59]=0x0000004c,// 76
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[60]=0x00000041,// 65
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[61]=0x00000058,// 88
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[62]=0x00000045,// 69
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[63]=0x00000041,// 65
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[64]=0x00000056,// 86
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[65]=0x0000003f,// 63
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[66]=0x00000055,// 85
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[67]=0x0000003e,// 62
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[68]=0x00000034,// 52
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[69]=0x00000031,// 49
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[70]=0x00000036,// 54
	.cfg_ddr_training_delay_ps[0].read_dq_bit_delay[71]=0x00000040,// 64
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[0]=0,// 21
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[1]=0x00000014,// 20
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[2]=0x00000013,// 19
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[3]=0x00000013,// 19
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[4]=0x00000013,// 19
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[5]=0x00000014,// 20
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[6]=0x00000014,// 20
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[7]=0x00000014,// 20
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[8]=0x00000010,// 16
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[9]=0x00000014,// 20
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[10]=0x00000013,// 19
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[11]=0x00000014,// 20
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[12]=0x00000013,// 19
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[13]=0x00000013,// 19
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[14]=0x00000014,// 20
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[15]=0x00000015,// 21
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[16]=0x00000014,// 20
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[17]=0x00000010,// 16
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[18]=0x00000015,// 21
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[19]=0x00000014,// 20
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[20]=0x00000014,// 20
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[21]=0x00000015,// 21
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[22]=0x00000014,// 20
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[23]=0x00000015,// 21
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[24]=0x00000016,// 22
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[25]=0x00000015,// 21
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[26]=0x00000010,// 16
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[27]=0x00000015,// 21
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[28]=0x00000015,// 21
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[29]=0x00000014,// 20
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[30]=0x00000016,// 22
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[31]=0x00000015,// 21
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[32]=0x00000015,// 21
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[33]=0x00000016,// 22
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[34]=0x00000015,// 21
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[35]=0x00000010,// 16
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[36]=0x00000010,// 16
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[37]=0x00000010,// 16
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[38]=0x00000010,// 16
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[39]=0x00000010,// 16
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[40]=0x00000010,// 16
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[41]=0x00000010,// 16
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[42]=0x00000010,// 16
	.cfg_ddr_training_delay_ps[0].soc_bit_vref[43]=0x00000010,// 16
	.cfg_ddr_training_delay_ps[0].dram_bit_vref[0]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].dram_bit_vref[1]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].dram_bit_vref[2]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].dram_bit_vref[3]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].dram_bit_vref[4]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].dram_bit_vref[5]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].dram_bit_vref[6]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].dram_bit_vref[7]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].dram_bit_vref[8]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].dram_bit_vref[9]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].dram_bit_vref[10]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].dram_bit_vref[11]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].dram_bit_vref[12]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].dram_bit_vref[13]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].dram_bit_vref[14]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].dram_bit_vref[15]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].dram_bit_vref[16]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].dram_bit_vref[17]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].dram_bit_vref[18]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].dram_bit_vref[19]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].dram_bit_vref[20]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].dram_bit_vref[21]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].dram_bit_vref[22]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].dram_bit_vref[23]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].dram_bit_vref[24]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].dram_bit_vref[25]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].dram_bit_vref[26]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].dram_bit_vref[27]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].dram_bit_vref[28]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].dram_bit_vref[29]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].dram_bit_vref[30]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].dram_bit_vref[31]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].dram_bit_vref[32]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].dram_bit_vref[33]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].dram_bit_vref[34]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].dram_bit_vref[35]=0x00000000,// 0
	.cfg_ddr_training_delay_ps[0].reserve_training_parameter = {
		(0 << 7) | 0,  (0 << 7) | 0,  (0 << 7) | 0,  (0 << 7) | 0,
		(1 << 7) | 0,  (0 << 7) | 0,  (0 << 7) | 0,  (0 << 7) | 0,
		(1 << 7) | 8, (1 << 7) | 8, (1 << 7) | 8, (1 << 7) | 8,
		(1 << 7) | 0, (1 << 7) | 0, (1 << 7) | 0, (1 << 7) | 0,
	},
	.cfg_board_SI_setting_ps[1].DRAMFreq = 667,
	.cfg_board_SI_setting_ps[1].PllBypassEn = 0,
	.cfg_board_SI_setting_ps[1].training_SequenceCtrl = 0,
	.cfg_board_SI_setting_ps[1].ddr_odt_config = DDR_DRAM_ODT_W_CS0_ODT0,
	.cfg_board_SI_setting_ps[1].clk_drv_ohm = DDR_SOC_AC_DRV_40_OHM,
	.cfg_board_SI_setting_ps[1].cs_drv_ohm = DDR_SOC_AC_DRV_40_OHM,
	.cfg_board_SI_setting_ps[1].ac_drv_ohm = DDR_SOC_AC_DRV_40_OHM,
	.cfg_board_SI_setting_ps[1].soc_data_drv_ohm_p = DDR_SOC_DATA_DRV_ODT_40_OHM,
	.cfg_board_SI_setting_ps[1].soc_data_drv_ohm_n = DDR_SOC_DATA_DRV_ODT_40_OHM,
	.cfg_board_SI_setting_ps[1].soc_data_odt_ohm_p = DDR_SOC_DATA_DRV_ODT_60_OHM,
	.cfg_board_SI_setting_ps[1].soc_data_odt_ohm_n = DDR_SOC_DATA_DRV_ODT_0_OHM,
	.cfg_board_SI_setting_ps[1].dram_data_drv_ohm = DDR_DRAM_DDR4_DRV_34_OHM,
	.cfg_board_SI_setting_ps[1].dram_data_odt_ohm = DDR_DRAM_DDR4_ODT_60_OHM,
	.cfg_board_SI_setting_ps[1].dram_data_wr_odt_ohm = DDR_DRAM_DDR_WR_ODT_0_OHM,
	.cfg_board_SI_setting_ps[1].dram_ac_odt_ohm = DDR_DRAM_DDR_AC_ODT_0_OHM,
	.cfg_board_SI_setting_ps[1].dram_data_drv_pull_up_calibration_ohm = DDR_DRAM_LPDDR4_ODT_40_OHM,
	.cfg_board_SI_setting_ps[1].lpddr4_dram_vout_voltage_range_setting = DDR_DRAM_LPDDR4_OUTPUT_1_3_VDDQ,
	.cfg_board_SI_setting_ps[1].reserve2 = 0,
	.cfg_board_SI_setting_ps[1].vref_ac_permil = 0,
	.cfg_board_SI_setting_ps[1].vref_soc_data_permil = 0,
	.cfg_board_SI_setting_ps[1].vref_dram_data_permil = 0,
	.cfg_board_SI_setting_ps[1].max_core_timmming_frequency = 0,
	.cfg_board_SI_setting_ps[1].training_phase_parameter = { 0 },
	.cfg_board_SI_setting_ps[1].ac_trace_delay_org = {
		128, 128, 128, 128, 128, 128, 128, 128,
		128, 128, 128, 128, 128, 128, 128, 128,
		128, 128, 128, 128, 128, 128, 128, 128,
		128, 128, 128, 128, 128, 128, 128, 128,
		128, 128, 128, 128,
	}, //total 36
	.cfg_ddr_training_delay_ps[1].ac_trace_delay = {
		128, 128, 128, 128, 128, 128, 128, 128,
		128, 128, 128, 128, 128, 128, 128, 128,
		128, 128, 128, 128, 128, 128, 128, 128,
		128, 128, 128, 128, 128, 128, 128, 128,
		128, 128, 128, 128,
	}, //total 36

	.cfg_ddr_training_delay_ps[1].write_dqs_delay = {
		0, 0, 0, 0, 0, 0, 0, 0
	},
	.cfg_ddr_training_delay_ps[1].write_dq_bit_delay = {
		50, 50, 50, 50, 50, 50, 50, 50,
		50, 50, 50, 50, 50, 50, 50, 50,
		50, 50, 50, 50, 50, 50, 50, 50,
		50, 50, 50, 50, 50, 50, 50, 50,
		50, 50, 50, 50, 50, 50, 50, 50,
		50, 50, 50, 50, 50, 50, 50, 50,
		50, 50, 50, 50, 50, 50, 50, 50,
		50, 50, 50, 50, 50, 50, 50, 50,
		50, 50, 50, 50, 50, 50, 50, 50,
	},
	.cfg_ddr_training_delay_ps[1].read_dqs_gate_delay = {
		192, 192, 192, 192, 192, 192, 192, 192
	}, //total 8
	.cfg_ddr_training_delay_ps[1].read_dqs_delay = {
		64, 64, 64, 64, 64, 64, 64, 64
	},              //total 8
	.cfg_ddr_training_delay_ps[1].read_dq_bit_delay = {
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0
	}, //total 72
	.cfg_ddr_training_delay_ps[1].soc_bit_vref = {
		0,  40, 40, 40, 40, 40, 40, 40,
		40, 40, 40, 40, 40, 40, 40, 40,
		40, 40, 40, 40, 40, 40, 40, 40,
		40, 40, 40, 40, 40, 40, 40, 40,
		40, 40, 40, 40, 40, 40, 40, 40,
		40, 40, 40, 40
	}, //total 44
	.cfg_ddr_training_delay_ps[1].dram_bit_vref = {
		0,  32, 32, 32, 32, 32, 32, 32,
		32, 32, 32, 32, 32, 32, 32, 32,
		32, 32, 32, 32, 32, 32, 32, 32,
		32, 32, 32, 32, 32, 32, 32, 32,
		32, 32, 32, 32
	}, //total 36
	.cfg_ddr_training_delay_ps[1].reserve_training_parameter = { 0 },
},
#endif
};
//ddr timing end

pll_set_t __pll_setting = {
	.cpu_clk = CPU_CLK / 24 * 24,
#ifdef CONFIG_PXP_EMULATOR
	.pxp = 1,
#else
	.pxp = 0,
#endif
	.spi_ctrl = 0,
	.lCustomerID = AML_CUSTOMER_ID,
	.log_chl = 0x3, /* 0x77: all channel enable. 0xFF: with stream info */
	.log_ctrl = (1 << 7) | /* (1<<7), print bl2 log into buffer */
#ifdef CONFIG_SILENT_CONSOLE
		(1<<6),    /* do not print log buffer */
#else
		(0<<6),    /* print log buffer before run bl31 */
#endif
	.ddr_timming_save_mode = 1,
};

ddr_reg_t __ddr_reg[] = {
	/* demo, user defined override register */
	{0xaabbccdd, 0, 0, 0, 0, 0},
	{0x11223344, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0},
};



#define VCCK_VAL				AML_VCCK_INIT_VOLTAGE
#define VDDEE_VAL				AML_VDDEE_INIT_VOLTAGE
/* VCCK PWM table */
#if   (VCCK_VAL == 1039)
	#define VCCK_VAL_REG	0x00000022
#elif (VCCK_VAL == 1029)
	#define VCCK_VAL_REG	0x00010021
#elif (VCCK_VAL == 1019)
	#define VCCK_VAL_REG	0x00020020
#elif (VCCK_VAL == 1009)
	#define VCCK_VAL_REG	0x0003001f
#elif (VCCK_VAL == 999)
	#define VCCK_VAL_REG	0x0004001e
#elif (VCCK_VAL == 989)
	#define VCCK_VAL_REG	0x0005001d
#elif (VCCK_VAL == 979)
	#define VCCK_VAL_REG	0x0006001c
#elif (VCCK_VAL == 969)
	#define VCCK_VAL_REG	0x0007001b
#elif (VCCK_VAL == 959)
	#define VCCK_VAL_REG	0x0008001a
#elif (VCCK_VAL == 949)
	#define VCCK_VAL_REG	0x00090019
#elif (VCCK_VAL == 939)
	#define VCCK_VAL_REG	0x000a0018
#elif (VCCK_VAL == 929)
	#define VCCK_VAL_REG	0x000b0017
#elif (VCCK_VAL == 919)
	#define VCCK_VAL_REG	0x000c0016
#elif (VCCK_VAL == 909)
	#define VCCK_VAL_REG	0x000d0015
#elif (VCCK_VAL == 899)
	#define VCCK_VAL_REG	0x000e0014
#elif (VCCK_VAL == 889)
	#define VCCK_VAL_REG	0x000f0013
#elif (VCCK_VAL == 879)
	#define VCCK_VAL_REG	0x00100012
#elif (VCCK_VAL == 869)
	#define VCCK_VAL_REG	0x00110011
#elif (VCCK_VAL == 859)
	#define VCCK_VAL_REG	0x00120010
#elif (VCCK_VAL == 849)
	#define VCCK_VAL_REG	0x0013000f
#elif (VCCK_VAL == 839)
	#define VCCK_VAL_REG	0x0014000e
#elif (VCCK_VAL == 829)
	#define VCCK_VAL_REG	0x0015000d
#elif (VCCK_VAL == 819)
	#define VCCK_VAL_REG	0x0016000c
#elif (VCCK_VAL == 809)
	#define VCCK_VAL_REG	0x0017000b
#elif (VCCK_VAL == 799)
	#define VCCK_VAL_REG	0x0018000a
#elif (VCCK_VAL == 789)
	#define VCCK_VAL_REG	0x00190009
#elif (VCCK_VAL == 779)
	#define VCCK_VAL_REG	0x001a0008
#elif (VCCK_VAL == 769)
	#define VCCK_VAL_REG	0x001b0007
#elif (VCCK_VAL == 759)
	#define VCCK_VAL_REG	0x001c0006
#elif (VCCK_VAL == 749)
	#define VCCK_VAL_REG	0x001d0005
#elif (VCCK_VAL == 739)
	#define VCCK_VAL_REG	0x001e0004
#elif (VCCK_VAL == 729)
	#define VCCK_VAL_REG	0x001f0003
#elif (VCCK_VAL == 719)
	#define VCCK_VAL_REG	0x00200002
#elif (VCCK_VAL == 709)
	#define VCCK_VAL_REG	0x00210001
#elif (VCCK_VAL == 699)
	#define VCCK_VAL_REG	0x00220000
#else
	#error "VCCK val out of range\n"
#endif

/* VDDEE_VAL_REG: VDDEE PWM table  0.69v-0.89v*/
#ifdef CONFIG_I2C_REGULATOR
#if    (VDDEE_VAL == 711)
	#define VDDEE_VAL_REG	0xb
#elif (VDDEE_VAL == 721)
	#define VDDEE_VAL_REG	0xc
#elif (VDDEE_VAL == 731)
	#define VDDEE_VAL_REG	0xd
#elif (VDDEE_VAL == 741)
	#define VDDEE_VAL_REG	0xe
#elif (VDDEE_VAL == 751)
	#define VDDEE_VAL_REG	0xf
#elif (VDDEE_VAL == 761)
	#define VDDEE_VAL_REG	0x10
#elif (VDDEE_VAL == 771)
	#define VDDEE_VAL_REG	0x11
#elif (VDDEE_VAL == 781)
	#define VDDEE_VAL_REG	0x12
#elif (VDDEE_VAL == 791)
	#define VDDEE_VAL_REG	0x13
#elif (VDDEE_VAL == 801)
	#define VDDEE_VAL_REG	0x14
#elif (VDDEE_VAL == 811)
	#define VDDEE_VAL_REG	0x15
#elif (VDDEE_VAL == 821)
	#define VDDEE_VAL_REG	0x16
#elif (VDDEE_VAL == 831)
	#define VDDEE_VAL_REG	0x17
#elif (VDDEE_VAL == 841)
	#define VDDEE_VAL_REG	0x18
#elif (VDDEE_VAL == 851)
	#define VDDEE_VAL_REG	0x19
#elif (VDDEE_VAL == 861)
	#define VDDEE_VAL_REG	0x1a
#elif (VDDEE_VAL == 871)
	#define VDDEE_VAL_REG	0x1b
#elif (VDDEE_VAL == 881)
	#define VDDEE_VAL_REG	0x1c
#elif (VDDEE_VAL == 891)
	#define VDDEE_VAL_REG	0x1d
#else
	#error "VDDEE val out of range\n"
#endif

#else

#if    (VDDEE_VAL == 711)
	#define VDDEE_VAL_REG	0x120000
#elif (VDDEE_VAL == 721)
	#define VDDEE_VAL_REG	0x110001
#elif (VDDEE_VAL == 731)
	#define VDDEE_VAL_REG	0x100002
#elif (VDDEE_VAL == 741)
	#define VDDEE_VAL_REG	0xf0003
#elif (VDDEE_VAL == 751)
	#define VDDEE_VAL_REG	0xe0004
#elif (VDDEE_VAL == 761)
	#define VDDEE_VAL_REG	0xd0005
#elif (VDDEE_VAL == 771)
	#define VDDEE_VAL_REG	0xc0006
#elif (VDDEE_VAL == 781)
	#define VDDEE_VAL_REG	0xb0007
#elif (VDDEE_VAL == 791)
	#define VDDEE_VAL_REG	0xa0008
#elif (VDDEE_VAL == 801)
	#define VDDEE_VAL_REG	0x90009
#elif (VDDEE_VAL == 811)
	#define VDDEE_VAL_REG	0x8000a
#elif (VDDEE_VAL == 821)
	#define VDDEE_VAL_REG	0x7000b
#elif (VDDEE_VAL == 831)
	#define VDDEE_VAL_REG	0x6000c
#elif (VDDEE_VAL == 841)
	#define VDDEE_VAL_REG	0x5000d
#elif (VDDEE_VAL == 851)
	#define VDDEE_VAL_REG	0x4000e
#elif (VDDEE_VAL == 861)
	#define VDDEE_VAL_REG	0x3000f
#elif (VDDEE_VAL == 871)
	#define VDDEE_VAL_REG	0x20010
#elif (VDDEE_VAL == 881)
	#define VDDEE_VAL_REG	0x10011
#elif (VDDEE_VAL == 891)
	#define VDDEE_VAL_REG	0x12
#else
	#error "VDDEE val out of range\n"
#endif
#endif

/* for PWM use */
/*
 * GPIOE_0   PWM_A    VDDEE_PWM
 *
 * GPIOE_1   PWM_B    VCCK_PWM
 */
/* PWM driver check http://scgit.amlogic.com:8080/#/c/38093/ */
#define PADCTRL_PIN_MUX_REG2   	((0x0002  << 2) + 0xfe000400)
#define PADCTRL_GPIOE_DS        ((0x0035  << 2) + 0xfe000400)

/* pwm clock and pwm controller */
#define CLKTREE_PWM_CLK_AB_CTRL ((0x0031  << 2) + 0xfe000800)
#define PWMAB_PWM_A            	((0x0000  << 2) + 0xfe002400)
#define PWMAB_PWM_B             ((0x0001  << 2) + 0xfe002400)
#define PWMAB_MISC_REG_AB       ((0x0002  << 2) + 0xfe002400)

bl2_reg_t __bl2_reg[] = {
#ifdef CONFIG_I2C_REGULATOR
	/* demo, user defined override register */
	{0,			0,            		0xffffffff,   0, 0, 0},
	{PWMAB_PWM_B,		VCCK_VAL_REG,  		0xffffffff,	0, BL2_INIT_STAGE_1, 0},
	{PWMAB_MISC_REG_AB,	0x1 << 1, 		0x1 << 1, 	0, BL2_INIT_STAGE_1, 0},
	/* Set GPIOA_5 low */
	{PADCTRL_GPIOA_O,	0x0 << 5, 		0x1 << 5, 	0, BL2_INIT_STAGE_1, 0},
	{PADCTRL_GPIOA_OEN,	0x0 << 5, 		0x1 << 5, 	0, BL2_INIT_STAGE_1, 0},
	/* Set GPIOD_10 low */
	{PADCTRL_GPIOD_O,	0x0 << 10,		0x1 << 10,	0, BL2_INIT_STAGE_1, 0},
	{PADCTRL_GPIOD_OEN,	0x0 << 10,		0x1 << 10, 	0, BL2_INIT_STAGE_1, 0},
	/* Set GPIOX_11 low */
	{PADCTRL_GPIOX_O,	0x0 << 11, 		0x1 << 11, 	0, BL2_INIT_STAGE_1, 0},
	{PADCTRL_GPIOX_OEN,	0x0 << 11, 		0x1 << 11, 	0, BL2_INIT_STAGE_1, 0},
	/* set pwm A and pwm B clock rate to 24M, enable them */
	{CLKTREE_PWM_CLK_AB_CTRL, 1 << 24 , 	        0xffffffff, 	0, BL2_INIT_STAGE_1, 0},
	/* set GPIOE_0 GPIOE_1 drive strength to 3*/
	{PADCTRL_GPIOE_DS,	0x3 << 2, 		0x3 << 2,	0, BL2_INIT_STAGE_1, 0},
	{PADCTRL_PIN_MUX_REG2,	0x3 << 4,		0xf << 4,	0, BL2_INIT_STAGE_1, 0},
#ifdef CONFIG_PDVFS_ENABLE
	/* table_id,  ee_voltage, ...... */
	{1,                     0x11,                   0x0,            0, BL2_INIT_STAGE_VDDCORE_CONFIG_1, 0},
	{2,                     0x14,                   0x0,            0, BL2_INIT_STAGE_VDDCORE_CONFIG_2, 0},
	{3,                     0x16,                   0x0,            0, BL2_INIT_STAGE_VDDCORE_CONFIG_3, 0},
#else
	{0x0,			VDDEE_VAL_REG,		0x0,		0, BL2_INIT_STAGE_I2C_REGULATOR, 0},
#endif
#else
	/* demo, user defined override register */
	{0,			0,            		0xffffffff,   0, 0, 0},
#ifdef CONFIG_PDVFS_ENABLE
	{PWMAB_PWM_A, 0x000c0006, 0xffffffff, 0, BL2_INIT_STAGE_VDDCORE_CONFIG_1, 0},
	{PWMAB_PWM_A, 0x00090009, 0xffffffff, 0, BL2_INIT_STAGE_VDDCORE_CONFIG_2, 0},
	{PWMAB_PWM_A, 0x0007000b, 0xffffffff, 0, BL2_INIT_STAGE_VDDCORE_CONFIG_3, 0},
#else
	{PWMAB_PWM_A,		VDDEE_VAL_REG,  	0xffffffff,	0, BL2_INIT_STAGE_1, 0},
#endif
	{PWMAB_PWM_B,		VCCK_VAL_REG,  		0xffffffff,	0, BL2_INIT_STAGE_1, 0},
	{PWMAB_MISC_REG_AB,	0x3 << 0, 		0x3, 		0, BL2_INIT_STAGE_1, 0},
	/* enable vddcpu dc-dc, set GPIOD_10 high */
	{PADCTRL_GPIOD_OEN,	0x0 << 10, 		0x1 << 10, 	0, BL2_INIT_STAGE_1, 0},
	{PADCTRL_GPIOD_O,	0x1 << 10, 		0x1 << 10, 	0, BL2_INIT_STAGE_1, 0},
	/* set pwm A and pwm B clock rate to 24M, enable them */
	{CLKTREE_PWM_CLK_AB_CTRL,1 << 8 | 1 << 24 , 	0xffffffff, 	0, BL2_INIT_STAGE_1, 0},
	/* set GPIOE_0 GPIOE_1 drive strength to 3*/
	{PADCTRL_GPIOE_DS,	0xf, 			0xf,		0, BL2_INIT_STAGE_1, 0},
	{PADCTRL_PIN_MUX_REG2,	0x3,			0xf,		0, BL2_INIT_STAGE_1, 0},
	{PADCTRL_PIN_MUX_REG2,	0x3 << 4,		0xf << 4,	0, BL2_INIT_STAGE_1, 0},
#endif
};
