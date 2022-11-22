#include <common.h>
#include <dm.h>
#include <dm/device-internal.h>
#include <dm/root.h>
#include <dm/uclass-internal.h>
#include <errno.h>
#include <linux/printk.h>
#include <stdio.h>
#include <usb_tcpc.h>

/* flag for the tcpc started / stopped status */
char tcpc_started = 0;

int tcpc_init(void)
{
	struct udevice *dev;
	struct uclass *uc;
	int ret;

	pr_info("tcpc_init\n");
	ret = uclass_get(UCLASS_USB_TCPC, &uc);
	if (ret)
		return ret;
	for (uclass_find_first_device(UCLASS_USB_TCPC, &dev);
	     dev;
	     uclass_find_next_device(&dev)) {

		pr_info("tcpc_init device_probe(%s) start \n", dev->name);
		ret = device_probe(dev);
		pr_info("tcpc_init device_probe(%s) done: %d\n", dev->name, ret);
		if (ret)
			return ret;
	}

	pr_info("tcpc_init done: %d \n", ret);
	tcpc_started = 1;
	return ret;
}

UCLASS_DRIVER(tcpc) = {
	.id		= UCLASS_USB_TCPC,
	.name		= "tcpc",
	.per_device_platdata_auto_alloc_size = sizeof(struct tcpc_uc_plat),
};
