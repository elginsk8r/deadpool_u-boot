// SPDX-License-Identifier: GPL-2.0
/*
 * USB Type-C Connector Class
 *
 * Copyright (C) 2017, Intel Corporation
 * Author: Heikki Krogerus <heikki.krogerus@linux.intel.com>
 */

#include <dm/device.h>
#include <dm/device_compat.h>
#include <linux/compat.h>
#include <linux/usb/typec.h>
#include <linux/string.h>
#include <linux/device_type.h>

struct typec_partner {
	struct udevice			dev;
	unsigned int			usb_pd:1;
	struct usb_pd_identity		*identity;
	enum typec_accessory		accessory;
};

struct typec_port {
	unsigned int			id;
	struct udevice			dev;

	int				prefer_role;
	enum typec_data_role		data_role;
	enum typec_role			pwr_role;
	enum typec_role			vconn_role;
	enum typec_pwr_opmode		pwr_opmode;
	enum typec_port_type		port_type;
	struct mutex			port_type_lock;

	enum typec_orientation		orientation;

	const struct typec_capability	*cap;
	const struct typec_operations   *ops;
};

#define to_typec_port(_dev_) container_of(_dev_, struct typec_port, dev)
#define to_typec_partner(_dev_) container_of(_dev_, struct typec_partner, dev)

struct typec_platdata {
	const struct device_type *type;
};

/*
 * copy from mm/util.c
 */
void *kmemdup(const void *src, size_t len, gfp_t gfp)
{
	void *p;

	p = kmalloc(len, gfp);
	if (p)
		memcpy(p, src, len);
	return p;
}

/*
 * replace ida_simple_get() in kernel as uboot is a single thread scenario.
 */
static unsigned int get_port_id(void) {
	static unsigned int port_id_counter = 0;
	return port_id_counter++;
}

const struct device_type typec_port_dev_type = {
	.name = "typec_port",
};

static int match_string(const char * const *array, size_t n, const char *string)
{
	int index;
	const char *item;

	for (index = 0; index < n; index++) {
		item = array[index];
		if (!item)
			break;
		if (!strcmp(item, string))
			return index;
	}

	return -EINVAL;
}

/* ------------------------------------------------------------------------- */
/* Type-C Partners */

static const struct device_type typec_partner_dev_type = {
	.name = "typec_partner",
};

/**
 * typec_partner_set_identity - Report result from Discover Identity command
 * @partner: The partner updated identity values
 *
 * This routine is used to report that the result of Discover Identity USB power
 * delivery command has become available.
 */
int typec_partner_set_identity(struct typec_partner *partner)
{
	if (!partner->identity)
		return -EINVAL;
	return 0;
}
EXPORT_SYMBOL_GPL(typec_partner_set_identity);

/**
 * typec_register_partner - Register a USB Type-C Partner
 * @port: The USB Type-C Port the partner is connected to
 * @desc: Description of the partner
 *
 * Registers a device for USB Type-C Partner described in @desc.
 *
 * Returns handle to the partner on success or ERR_PTR on failure.
 */
struct typec_partner *typec_register_partner(struct typec_port *port,
					     struct typec_partner_desc *desc)
{
	struct typec_partner *partner;
	struct typec_platdata *plat;
	char *name;

	partner = kzalloc(sizeof(*partner), GFP_KERNEL);
	if (!partner)
		return ERR_PTR(-ENOMEM);

	partner->usb_pd = desc->usb_pd;
	partner->accessory = desc->accessory;

	if (desc->identity) {
		/*
		 * Creating directory for the identity only if the driver is
		 * able to provide data to it.
		 */
		partner->identity = desc->identity;
	}

	partner->dev.parent = &port->dev;

	plat = kzalloc(sizeof(*plat), GFP_KERNEL);
	if (!plat)
		return ERR_PTR(-ENOMEM);
	plat->type = &typec_partner_dev_type;
	partner->dev.platdata = plat;

	name = kzalloc(32, GFP_KERNEL);
	snprintf(name, 32, "%s-partner", port->dev.name);
	partner->dev.name = name;

	INIT_LIST_HEAD(&partner->dev.uclass_node);
	INIT_LIST_HEAD(&partner->dev.child_head);
	INIT_LIST_HEAD(&partner->dev.sibling_node);

	/* put partner dev into port's successor list */
	list_add_tail(&partner->dev.sibling_node, &port->dev.child_head);

	return partner;
}
EXPORT_SYMBOL_GPL(typec_register_partner);

/**
 * typec_unregister_partner - Unregister a USB Type-C Partner
 * @partner: The partner to be unregistered
 *
 * Unregister device created with typec_register_partner().
 */
void typec_unregister_partner(struct typec_partner *partner)
{
	if (!IS_ERR_OR_NULL(partner))
		device_unregister(&partner->dev);
}
EXPORT_SYMBOL_GPL(typec_unregister_partner);

/* ------------------------------------------------------------------------- */
/* USB Type-C ports */

static const char * const typec_roles[] = {
	[TYPEC_SINK]	= "sink",
	[TYPEC_SOURCE]	= "source",
};

static const char * const typec_port_power_roles[] = {
	[TYPEC_PORT_SRC] = "source",
	[TYPEC_PORT_SNK] = "sink",
	[TYPEC_PORT_DRP] = "dual",
};

static const char * const typec_port_data_roles[] = {
	[TYPEC_PORT_DFP] = "host",
	[TYPEC_PORT_UFP] = "device",
	[TYPEC_PORT_DRD] = "dual",
};

/* --------------------------------------- */
/* Driver callbacks to report role updates */

/**
 * typec_set_data_role - Report data role change
 * @port: The USB Type-C Port where the role was changed
 * @role: The new data role
 *
 * This routine is used by the port drivers to report data role changes.
 */
void typec_set_data_role(struct typec_port *port, enum typec_data_role role)
{
	if (port->data_role == role)
		return;

	port->data_role = role;
}
EXPORT_SYMBOL_GPL(typec_set_data_role);

/**
 * typec_set_pwr_role - Report power role change
 * @port: The USB Type-C Port where the role was changed
 * @role: The new data role
 *
 * This routine is used by the port drivers to report power role changes.
 */
void typec_set_pwr_role(struct typec_port *port, enum typec_role role)
{
	if (port->pwr_role == role)
		return;

	port->pwr_role = role;
}
EXPORT_SYMBOL_GPL(typec_set_pwr_role);

/**
 * typec_set_vconn_role - Report VCONN source change
 * @port: The USB Type-C Port which VCONN role changed
 * @role: Source when @port is sourcing VCONN, or Sink when it's not
 *
 * This routine is used by the port drivers to report if the VCONN source is
 * changes.
 */
void typec_set_vconn_role(struct typec_port *port, enum typec_role role)
{
	if (port->vconn_role == role)
		return;

	port->vconn_role = role;
}
EXPORT_SYMBOL_GPL(typec_set_vconn_role);

/**
 * typec_set_pwr_opmode - Report changed power operation mode
 * @port: The USB Type-C Port where the mode was changed
 * @opmode: New power operation mode
 *
 * This routine is used by the port drivers to report changed power operation
 * mode in @port. The modes are USB (default), 1.5A, 3.0A as defined in USB
 * Type-C specification, and "USB Power Delivery" when the power levels are
 * negotiated with methods defined in USB Power Delivery specification.
 */
void typec_set_pwr_opmode(struct typec_port *port,
			  enum typec_pwr_opmode opmode)
{
	struct udevice *partner_dev;

	if (port->pwr_opmode == opmode)
		return;

	port->pwr_opmode = opmode;

	device_find_first_child(&port->dev, &partner_dev);
	if (partner_dev) {
		struct typec_partner *partner = to_typec_partner(partner_dev);

		if (opmode == TYPEC_PWR_MODE_PD && !partner->usb_pd) {
			partner->usb_pd = 1;
		}
	}
}
EXPORT_SYMBOL_GPL(typec_set_pwr_opmode);

/**
 * typec_find_port_power_role - Get the typec port power capability
 * @name: port power capability string
 *
 * This routine is used to find the typec_port_type by its string name.
 *
 * Returns typec_port_type if success, otherwise negative error code.
 */
int typec_find_port_power_role(const char *name)
{
	return match_string(typec_port_power_roles,
			    ARRAY_SIZE(typec_port_power_roles), name);
}
EXPORT_SYMBOL_GPL(typec_find_port_power_role);

/**
 * typec_find_power_role - Find the typec one specific power role
 * @name: power role string
 *
 * This routine is used to find the typec_role by its string name.
 *
 * Returns typec_role if success, otherwise negative error code.
 */
int typec_find_power_role(const char *name)
{
	return match_string(typec_roles, ARRAY_SIZE(typec_roles), name);
}
EXPORT_SYMBOL_GPL(typec_find_power_role);

/**
 * typec_find_port_data_role - Get the typec port data capability
 * @name: port data capability string
 *
 * This routine is used to find the typec_port_data by its string name.
 *
 * Returns typec_port_data if success, otherwise negative error code.
 */
int typec_find_port_data_role(const char *name)
{
	return match_string(typec_port_data_roles,
			    ARRAY_SIZE(typec_port_data_roles), name);
}
EXPORT_SYMBOL_GPL(typec_find_port_data_role);

/* ------------------------------------------ */
/* API for Multiplexer/DeMultiplexer Switches */

/**
 * typec_set_orientation - Set USB Type-C cable plug orientation
 * @port: USB Type-C Port
 * @orientation: USB Type-C cable plug orientation
 *
 * Set cable plug orientation for @port.
 */
int typec_set_orientation(struct typec_port *port,
			  enum typec_orientation orientation)
{
	port->orientation = orientation;

	return 0;
}
EXPORT_SYMBOL_GPL(typec_set_orientation);

/**
 * typec_get_orientation - Get USB Type-C cable plug orientation
 * @port: USB Type-C Port
 *
 * Get current cable plug orientation for @port.
 */
enum typec_orientation typec_get_orientation(struct typec_port *port)
{
	return port->orientation;
}
EXPORT_SYMBOL_GPL(typec_get_orientation);

/**
 * typec_set_mode - Set mode of operation for USB Type-C connector
 * @port: USB Type-C connector
 * @mode: Accessory Mode, USB Operation or Safe State
 *
 * Configure @port for Accessory Mode @mode. This function will configure the
 * muxes needed for @mode.
 */
int typec_set_mode(struct typec_port *port, int mode)
{
	return 0;
}
EXPORT_SYMBOL_GPL(typec_set_mode);

/* --------------------------------------- */

/**
 * typec_get_drvdata - Return private driver data pointer
 * @port: USB Type-C port
 */
void *typec_get_drvdata(struct typec_port *port)
{
	return dev_get_priv(&port->dev);
}
EXPORT_SYMBOL_GPL(typec_get_drvdata);

/**
 * typec_register_port - Register a USB Type-C Port
 * @parent: Parent device
 * @cap: Description of the port
 *
 * Registers a device for USB Type-C Port described in @cap.
 *
 * Returns handle to the port on success or ERR_PTR on failure.
 */
struct typec_port *typec_register_port(struct udevice *parent,
				       const struct typec_capability *cap)
{
	struct typec_port *port;
	struct typec_platdata *plat;
	int id;
	char *name;

	port = kzalloc(sizeof(*port), GFP_KERNEL);
	if (!port)
		return ERR_PTR(-ENOMEM);

	id = get_port_id();
	if (id < 0) {
		kfree(port);
		return ERR_PTR(id);
	}

	switch (cap->type) {
	case TYPEC_PORT_SRC:
		port->pwr_role = TYPEC_SOURCE;
		port->vconn_role = TYPEC_SOURCE;
		break;
	case TYPEC_PORT_SNK:
		port->pwr_role = TYPEC_SINK;
		port->vconn_role = TYPEC_SINK;
		break;
	case TYPEC_PORT_DRP:
		if (cap->prefer_role != TYPEC_NO_PREFERRED_ROLE)
			port->pwr_role = cap->prefer_role;
		else
			port->pwr_role = TYPEC_SINK;
		break;
	}

	switch (cap->data) {
	case TYPEC_PORT_DFP:
		port->data_role = TYPEC_HOST;
		break;
	case TYPEC_PORT_UFP:
		port->data_role = TYPEC_DEVICE;
		break;
	case TYPEC_PORT_DRD:
		if (cap->prefer_role == TYPEC_SOURCE)
			port->data_role = TYPEC_HOST;
		else
			port->data_role = TYPEC_DEVICE;
		break;
	}

	mutex_init(&port->port_type_lock);

	port->id = id;
	port->ops = cap->ops;
	port->port_type = cap->type;
	port->prefer_role = cap->prefer_role;

	port->dev.parent = parent;
	name = kzalloc(16, GFP_KERNEL);
	snprintf(name, 16, "port%d", id);
	port->dev.name = name;

	plat = kzalloc(sizeof(*plat), GFP_KERNEL);
	if (!plat)
		return ERR_PTR(-ENOMEM);
	plat->type = &typec_port_dev_type;
	port->dev.platdata = plat;

	port->dev.priv = cap->driver_data;

	INIT_LIST_HEAD(&port->dev.uclass_node);
	INIT_LIST_HEAD(&port->dev.child_head);
	INIT_LIST_HEAD(&port->dev.sibling_node);

	port->cap = kmemdup(cap, sizeof(*cap), GFP_KERNEL);
	if (!port->cap) {
		return ERR_PTR(-ENOMEM);
	}
	return port;
}
EXPORT_SYMBOL_GPL(typec_register_port);

/**
 * typec_unregister_port - Unregister a USB Type-C Port
 * @port: The port to be unregistered
 *
 * Unregister device created with typec_register_port().
 */
void typec_unregister_port(struct typec_port *port)
{
	if (!IS_ERR_OR_NULL(port))
		device_unregister(&port->dev);
}
EXPORT_SYMBOL_GPL(typec_unregister_port);
