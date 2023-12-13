// SPDX-License-Identifier: GPL-2.0
/*
 * USB Role Switch Support
 *
 * Copyright (C) 2018 Intel Corporation
 * Author: Heikki Krogerus <heikki.krogerus@linux.intel.com>
 *         Hans de Goede <hdegoede@redhat.com>
 */

#include <dm/device.h>
#include <linux/usb/role.h>
#include <linux/device_type.h>
#include <linux/usb/role.h>

struct usb_role_switch {
	struct udevice dev;
	enum usb_role role;

	/* From descriptor */
	struct udevice *usb2_port;
	struct udevice *usb3_port;
	struct udevice *udc;
	usb_role_switch_set_t set;
	usb_role_switch_get_t get;
	bool allow_userspace_control;
};


#define to_role_switch(d)	container_of(d, struct usb_role_switch, dev)

/**
 * usb_role_switch_set_role - Set USB role for a switch
 * @sw: USB role switch
 * @role: USB role to be switched to
 *
 * Set USB role @role for @sw.
 */
int usb_role_switch_set_role(struct usb_role_switch *sw, enum usb_role role)
{
	// TODO: we don't have "usb-role-switch" in DTS
	// Hardcode to No-OP until we plan to support "usb-role-switch"
	int ret;

	if (IS_ERR_OR_NULL(sw))
		return 0;

	return ret;
}
EXPORT_SYMBOL_GPL(usb_role_switch_set_role);

/**
 * usb_role_switch_get_role - Get the USB role for a switch
 * @sw: USB role switch
 *
 * Depending on the role-switch-driver this function returns either a cached
 * value of the last set role, or reads back the actual value from the hardware.
 */
enum usb_role usb_role_switch_get_role(struct usb_role_switch *sw)
{
	// TODO: we don't have "usb-role-switch" in DTS
	// Hardcode to No-OP until we plan to support "usb-role-switch"
	enum usb_role role;

	if (IS_ERR_OR_NULL(sw))
		return USB_ROLE_NONE;

	return role;
}
EXPORT_SYMBOL_GPL(usb_role_switch_get_role);

/**
 * usb_role_switch_get - Find USB role switch linked with the caller
 * @dev: The caller device
 *
 * Finds and returns role switch linked with @dev. The reference count for the
 * found switch is incremented.
 */
struct usb_role_switch *usb_role_switch_get(struct udevice *dev)
{
	// TODO: we don't have "usb-role-switch" in DTS
	// Hardcode to NULL until we plan to support "usb-role-switch"
	return NULL;
}
EXPORT_SYMBOL_GPL(usb_role_switch_get);


/**
 * usb_role_switch_put - Release handle to a switch
 * @sw: USB Role Switch
 *
 * Decrement reference count for @sw.
 */
void usb_role_switch_put(struct usb_role_switch *sw)
{
	// TODO: we don't have "usb-role-switch" in DTS
	// Hardcode to No-OP until we plan to support "usb-role-switch"
}
EXPORT_SYMBOL_GPL(usb_role_switch_put);
