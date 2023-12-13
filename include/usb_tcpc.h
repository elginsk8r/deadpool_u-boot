#ifndef __USB_TCPC_H
#define __USB_TCPC_H

/**
 * struct tcpc_uc_plat - Platform data the uclass stores about each device
 *
 * @label:	tcpc label
 */
struct tcpc_uc_plat {
	const char *label;
};

/**
 * struct tcpc_uc_priv - Private data the uclass stores about each device
 *
 */
struct tcpc_uc_priv {
};

int tcpc_init(void);

#endif
