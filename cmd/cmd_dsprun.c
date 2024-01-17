// SPDX-License-Identifier: (GPL-2.0+ OR MIT)
/*
 * Copyright (c) 2019 Amlogic, Inc. All rights reserved.
 */

#include <common.h>
#include <command.h>
#include <asm/arch/cpu.h>
#include <asm/arch/secure_apb.h>
#include <asm/arch/timer.h>
#include <asm/arch/bl31_apis.h>
#include <asm/arch/p_register.h>
#include <serial.h>

void dsp_reset(uint32_t id,uint32_t reset_addr)
{
	uint32_t StatVectorSel;
	uint32_t strobe = 1;
	//address remap
	pr_info("\n start_dsp \n");
	//*P_DSP_REMAP2 = 0x3000fff0; //replace the highest 12bits 0xfffxxxxx with 0x300xxxxx

	StatVectorSel = (reset_addr!= 0xfffa0000);

	//open the Dreset, Breset, irq_clken
	if (id == 0) { //dspa
		init_dsp(id,reset_addr, (0x1 | StatVectorSel<<1 | strobe<<2));
		udelay(50);
		pr_info("\n *P_DSP_CFG0 : ADDR_0X%p, value_0x%8x \n",P_DSP_CFG0,*P_DSP_CFG0);
		*P_DSP_CFG0 = (*P_DSP_CFG0 & ~(0xffff <<0)) | (0x2018 << 0) | (1<<29) | (0<<0) ;      //irq_clken
		udelay(10);
		*P_DSP_CFG0 = *P_DSP_CFG0 & ~(1<<31);     //Dreset assert
		udelay(10);
		// *P_DSP_CFG0 = *P_DSP_CFG0 | (1<<31);     //Dreset deassert
		// _udelay(10);
		*P_DSP_CFG0 = *P_DSP_CFG0 & ~(1<<30);    //Breset
		udelay(10);
		// *P_DSP_CFG0 = *P_DSP_CFG0 | (1<<30);    //Breset deassert
		// _udelay(10);
		pr_info("\n *P_DSP_CFG0 : ADDR_0X%p, value_0x%8x \n",P_DSP_CFG0,*P_DSP_CFG0);
	} else {
		init_dsp(id,reset_addr, (0x1 | StatVectorSel<<1 | strobe<<2));
		udelay(50);
		pr_info("\n *P_DSPB_CFG0 : ADDR_0X%p, value_0x%8x \n",P_DSPB_CFG0,*P_DSPB_CFG0);
		*P_DSPB_CFG0 = (*P_DSPB_CFG0 & ~(0xffff <<0)) | (0x2019 << 0) | (1<<29) | (0<<0) ;      //irq_clken
		udelay(10);
		*P_DSPB_CFG0 = *P_DSPB_CFG0 & ~(1<<31);     //Dreset
		udelay(10);
		*P_DSPB_CFG0 = *P_DSPB_CFG0 & ~(1<<30);    //Breset
		udelay(10);
		pr_info("\n *P_DSPB_CFG0 : ADDR_0X%p, value_0x%8x \n",P_DSPB_CFG0,*P_DSPB_CFG0);
	}
}

static int do_dsprun(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	unsigned long addr;
	unsigned int dspid;
	unsigned long dspfw_size;

	int ret=0;
	if (argc < 4) {
		pr_err("please input dsp boot args: id, address, fw size!\n");
		return CMD_RET_USAGE;
	}

	dspid = simple_strtoul(argv[1], NULL, 16);
	addr = simple_strtoul(argv[2], NULL, 16);
	dspfw_size = simple_strtoul(argv[3], NULL, 16);
	pr_info("dsp%d boot address:0x%lx size:0x%lx", dspid, addr, dspfw_size);

	flush_cache(addr, dspfw_size);

	dsp_reset(dspid, addr);
	pr_info("dsp init over! \n");
	return ret;
}


U_BOOT_CMD(
	dsprun,	4,	1,	do_dsprun,
	"load dspboot.bin from ddr address",
	"arg[0]: cmd\n"
	"arg[1]: dspid \n"
	"arg[2]: dspboot.bin load address!"
	"arg[3]: dsp firmware size\n"
);


static int do_flushmemory(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	unsigned long addr;
	unsigned long size;

	int ret=0;
	if (argc < 3) {
		pr_err("please input bootpackage args: address, fw size!\n");
		return CMD_RET_USAGE;
	}

	addr = simple_strtoul(argv[1], NULL, 16);
	size = simple_strtoul(argv[2], NULL, 16);
	pr_info("%s address:0x%lx size:0x%lx\n", argv[0], addr, size);
	flush_cache(addr, size);
	return ret;
}


U_BOOT_CMD(
	flushmemory,	3,	1,	do_flushmemory,
	"flush cache",
	"arg[0]: cmd\n"
	"arg[1]: address \n"
	"arg[2]: size \n"
);



