#include <common.h>
#include <command.h>
#include <usb_tcpc.h>

static int do_tcpc(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	extern char tcpc_started;

	if (argc != 2)
		return CMD_RET_USAGE;

	if (strncmp(argv[1], "start", 5) == 0) {
		if (tcpc_started)
			return 0; /* Already started */
		printf("starting TCPC...\n");
		tcpc_init();
		return 0;
	}

	return CMD_RET_USAGE;
}

U_BOOT_CMD(tcpc, 2, 0, do_tcpc,
		"run commands and summarize execution time",
		"start - start USB-C port controller and PD explicit contract\n");
