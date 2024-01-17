// SPDX-License-Identifier: (GPL-2.0+ OR MIT)
/*
 * Copyright (c) 2019 Amlogic, Inc. All rights reserved.
 */

#include <config.h>
#include <common.h>
#include <asm/arch/io.h>
#include <command.h>
#include <hexdump.h>
#include <malloc.h>
#include <asm/arch/efuse.h>
#include <asm/arch/bl31_apis.h>

#define CMD_EFUSE_WRITE            0
#define CMD_EFUSE_READ             1
#define CMD_EFUSE_SECURE_BOOT_SET  6
#define CMD_EFUSE_PASSWORD_SET     7
#define CMD_EFUSE_CUSTOMER_ID_SET  8

#define CMD_EFUSE_AMLOGIC_SET      20
#define CMD_EFUSE_MRK_CHECK        21


int cmd_efuse(int argc, char * const argv[], char *buf)
{
	int i, action = -1;
	u32 offset;
	u32 size, max_size;
	char *end;
	char *s;
	int ret;
	long lAddr1, lAddr2;
	unsigned long nType;

	if (strncmp(argv[1], "read", 4) == 0) {
		action = CMD_EFUSE_READ;
	} else if (strncmp(argv[1], "write", 5) == 0) {
		action = CMD_EFUSE_WRITE;
	} else if (strncmp(argv[1], "secure_boot_set", 15) == 0) {
		action = CMD_EFUSE_SECURE_BOOT_SET;
		goto efuse_action;
	} else if (strncmp(argv[1], "password_set", 12) == 0) {
		action = CMD_EFUSE_PASSWORD_SET;
		goto efuse_action;
	} else if (strncmp(argv[1], "customer_id_set", 15) == 0) {
		action = CMD_EFUSE_CUSTOMER_ID_SET;
		goto efuse_action;
	} else if (strncmp(argv[1], "amlogic_set", 11) == 0) {
		action = CMD_EFUSE_AMLOGIC_SET;
		goto efuse_action;
	} else if (strncmp(argv[1], "mrk_check", 9) == 0) {
		action = CMD_EFUSE_MRK_CHECK;
		goto efuse_action;
	} else {

		printf("%s arg error\n", argv[1]);
		return CMD_RET_USAGE;
	}

	if (argc < 4)
		return CMD_RET_USAGE;
	/*check efuse user data max size*/
	offset = simple_strtoul(argv[2], &end, 16);
	size = simple_strtoul(argv[3], &end, 16);
	printf("%s: offset is %d  size is  %d\n", __func__, offset, size);
	max_size = efuse_get_max();
	if (!size) {
		printf("\n error: size is zero!!!\n");
		return -1;
	}
	if (offset > max_size) {
		printf("\n error: offset is too large!!!\n");
		printf("\n offset should be less than %d!\n", max_size);
		return -1;
	}
	if (offset + size > max_size) {
		printf("\n error: offset + size is too large!!!\n");
		printf("\n offset + size should be less than %d!\n", max_size);
		return -1;
	}

efuse_action:

	/* efuse read */
	switch (action)
	{
	case CMD_EFUSE_READ:
	{
		memset(buf, 0, size);
		ret = efuse_read_usr(buf, size, (loff_t *)&offset);
		if (ret == -1) {
			printf("ERROR: efuse read user data fail!\n");
			return -1;
		}

		if (ret != size)
			printf("ERROR: read %d byte(s) not %d byte(s) data\n",
			       ret, size);
		printf("efuse read data");
		for (i = 0; i < size; i++) {
			if (i % 16 == 0)
				printf("\n");
			printf(":%02x", buf[i]);
		}
		printf("\n");
	}break;
	/* efuse write */
	case CMD_EFUSE_WRITE:
	{
		if (argc < 5) {
			printf("arg count error\n");
			return CMD_RET_USAGE;
		}
		memset(buf, 0, size);

		s = argv[4];
		memcpy(buf, s, strlen(s));
		if (efuse_write_usr(buf, size, (loff_t *)&offset) < 0) {
			printf("error: efuse write fail.\n");
			return -1;
		} else {
			printf("%s written done.\n", __func__);
		}

	}break;
	//efuse process
	case CMD_EFUSE_SECURE_BOOT_SET:
	case CMD_EFUSE_AMLOGIC_SET:
	case CMD_EFUSE_PASSWORD_SET:
	case CMD_EFUSE_CUSTOMER_ID_SET:
	{
		lAddr1 = GXB_IMG_LOAD_ADDR;

		if (argc > 2)
			lAddr1 = simple_strtoul(argv[2], &end, 16);

		lAddr2 = get_sharemem_info(GET_SHARE_MEM_INPUT_BASE);
		memcpy((void *)lAddr2, (void *)lAddr1, GXB_EFUSE_PATTERN_SIZE<<1);
		flush_cache(lAddr2, GXB_EFUSE_PATTERN_SIZE<<1);

		switch (action) {
		case CMD_EFUSE_SECURE_BOOT_SET:
			nType = AML_D_P_W_EFUSE_SECURE_BOOT;
			break;
		case CMD_EFUSE_AMLOGIC_SET:
			nType = AML_D_P_W_EFUSE_AMLOGIC;
			break;
		case CMD_EFUSE_PASSWORD_SET:
			nType = AML_D_P_W_EFUSE_PASSWORD;
			break;
		case CMD_EFUSE_CUSTOMER_ID_SET:
			nType = AML_D_P_W_EFUSE_CUSTOMER_ID;
			break;
		default:
			return -1;
		}

		ret = aml_sec_boot_check(nType, lAddr2, GXB_EFUSE_PATTERN_SIZE<<1, 0);
		if (ret)
			printf("aml log : EFUSE pattern programming fail [%d]!\n",
			       ret);
		else
			printf("aml log : EFUSE pattern programming success!\n");

		return ret;
	}break;
	case CMD_EFUSE_MRK_CHECK:
	{

#ifdef AML_D_P_MRK_CHECK
		#define MRK_CHK_ACGK        (0x4b474341)
		#define MRK_CHK_DVGK        (0x4b475644)
		#define MRK_CHK_DVUK        (0x4b555644)
		#define MRK_CHK_ACRK        (0x4b524341)

		#define MRK_CHK_SALT_LEN 8

		#define MKL_KL_DEPTH 3

		typedef unsigned int mkl_ek128_t[4];

		typedef struct {
			unsigned long nMrkType;
			unsigned char szReserved[MRK_CHK_SALT_LEN];
			unsigned char szSalt[MRK_CHK_SALT_LEN];
			unsigned char szCheck[MRK_CHK_SALT_LEN];
			mkl_ek128_t * mrk_chk_ekl[MKL_KL_DEPTH];
			mkl_ek128_t * mrk_chk_ekh[MKL_KL_DEPTH];
			mkl_ek128_t   ek3[2];
			mkl_ek128_t   ek2[2];
			mkl_ek128_t   ek1[2];
		}aml_mrk_chk_t;

		aml_mrk_chk_t chk = {
			MRK_CHK_DVGK, //type
			{0},
			{0x45, 0x7f, 0x03, 0x9a, 0x5d, 0x57, 0xc0, 0x24}, //salt
			{0xcf, 0x1e, 0x21, 0x21, 0x8e, 0xf3, 0xf1, 0x18}, //check
			//{&mrk_chk_ek3[0],&mrk_chk_ek2[0],&mrk_chk_ek1[0]},
			//{&mrk_chk_ek3[1],&mrk_chk_ek2[1],&mrk_chk_ek1[1]},
			{0},
			{0},
			{
			{0xcebcb44c, 0xf08c2197, 0x6cc856f3, 0x394212f7},
			{0xe5554629, 0xa762b526, 0x2ff51eda, 0xeb8541e1},
			},
			{
			{0x2b35fd84, 0xf24d6f9a, 0x1255c5f0, 0xb8418ebc},
			{0x12175b71, 0xd5f93abb, 0x93f8ebb3, 0xf011d5ec},
			},
			{
			{0xe43a4538, 0xd9851fd4, 0x18d8b578, 0xa522842b},
			{0x2c762493, 0xf035bb24, 0xb63d8c09, 0xbe48b494},
			},
		};

		if (argc == 4) {
			// efuse mrk_check <acgk|acrk|dvgk|dvuk> <16-byte check number as 32-char hex string>
			if (strcmp(argv[2], "acgk") == 0) {
				chk.nMrkType = MRK_CHK_ACGK;
			} else if (strcmp(argv[2], "acrk") == 0) {
				chk.nMrkType = MRK_CHK_ACRK;
			} else if (strcmp(argv[2], "dvgk") == 0) {
				chk.nMrkType = MRK_CHK_DVGK;
			} else if (strcmp(argv[2], "dvuk") == 0) {
				chk.nMrkType = MRK_CHK_DVUK;
			} else {
				printf("bl33: Err: bad mrk type\n");
				return -1;
			}

			if (strlen(argv[3]) != 32) {
				printf("bl33: Err: bad check number len\n");
				return -1;
			}
			if (hex2bin(&chk.szSalt[0], argv[3], 8) ||
					hex2bin(&chk.szCheck[0], &argv[3][0] + 16, 8)) {
				printf("bl33: Err: bad check number\n");
				return -1;
			}

			lAddr1 =(unsigned long)&chk;
		} else {
			// efuse mrk_check [address of aml_mrk_chk_t]
			lAddr1 = GXB_IMG_LOAD_ADDR;

			if (argc > 2)
				lAddr1 = simple_strtoul(argv[2], &end, 16);
		}

		lAddr2 = get_sharemem_info(GET_SHARE_MEM_INPUT_BASE);
		memcpy((void *)lAddr2, (void *)lAddr1, GXB_EFUSE_PATTERN_SIZE);
		flush_cache(lAddr2, GXB_EFUSE_PATTERN_SIZE);
		ret = aml_sec_boot_check(AML_D_P_MRK_CHECK, lAddr2, GXB_EFUSE_PATTERN_SIZE, 0);

		if (ret)
			printf("aml log : EFUSE mrk_check fail with ERR=0x%x!\n",ret);
		else
			printf("aml log : EFUSE mrk_check pass!\n");
#else
		printf("aml log : SoC not support efuse mrk_check!\n");
		ret = -1;
#endif
		return ret;

	}break;
	default:
	{
		printf("arg error\n");
		return CMD_RET_USAGE;
	}break;
	}

	return 0;
}

int do_efuse(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	char buf[EFUSE_BYTES];

	memset(buf, 0, sizeof(buf));

	if (argc < 2)
		return CMD_RET_USAGE;

	return cmd_efuse(argc, argv, buf);
}

static char efuse_help_text[] =
	"[read/write offset size [data]]\n"
	"  [read/write]  - read or write 'size' data from\n"
	"                  'offset' from efuse user data ;\n"
	"  [offset]      - the offset byte from the beginning\n"
	"                  of efuse user data zone;\n"
	"  [size]        - data size\n"
	"  [data]        - the optional argument for 'write',\n"
	"                  data is treated as characters\n"
	"  examples: efuse write 0xc 0xd abcdABCD1234\n"
	"[amlogic_set addr]\n"
	"mrk_check [address of aml_mrk_chk_t]\n"
	"mrk_check <acgk|acrk|dvgk|dvuk> <check number as 32-char hex string>\n";

U_BOOT_CMD(
	efuse,  5,  0 /*repeatable=no*/,  do_efuse,
	"efuse commands", efuse_help_text
);
