// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2015-2021 Google, Inc
 *
 * USB Type-C Port Controller Interface.
 */

#include <linux/kernel.h>
#include <linux/types.h>
#include <i2c.h>
#include <linux/usb/pd.h>
#include <linux/usb/tcpm.h>
#include <linux/usb/typec.h>
#include <time.h>
#include <linux/delay.h>
#include <dm/read.h>

#include "tcpci.h"

#define PD_RETRY_COUNT 3
#define MAIN_LOOP_TIMEOUT_MS 5000
/* buffer time after PS_RDY to handle following messages */
#define MAIN_LOOP_BUFFER_MS 2000

/* FIXME: bypass warn_on for pd testing */
#if defined(WARN_ON)
#undef WARN_ON
#endif
#define WARN_ON(_x) _x

struct tcpci {
	struct udevice *dev;

	struct tcpm_port *port;

	struct udevice *i2c;

	bool controls_vbus;

	struct tcpc_dev tcpc;
	struct tcpci_data *data;
};

struct tcpci_chip {
	struct tcpci *tcpci;
	struct tcpci_data data;
};

static inline struct tcpci *tcpc_to_tcpci(struct tcpc_dev *tcpc)
{
	return container_of(tcpc, struct tcpci, tcpc);
}

static int tcpci_read16(struct tcpci *tcpci, unsigned int reg, u16 *val)
{
	return dm_i2c_read(tcpci->i2c, reg, (uint8_t*) val, sizeof(u16));
}

static int tcpci_write16(struct tcpci *tcpci, unsigned int reg, u16 val)
{
	return dm_i2c_write(tcpci->i2c, reg, (uint8_t*) &val, sizeof(u16));
}

static int tcpci_set_cc(struct tcpc_dev *tcpc, enum typec_cc_status cc)
{
	struct tcpci *tcpci = tcpc_to_tcpci(tcpc);
	uint8_t reg;
	int ret;

	switch (cc) {
	case TYPEC_CC_RA:
		reg = (TCPC_ROLE_CTRL_CC_RA << TCPC_ROLE_CTRL_CC1_SHIFT) |
			(TCPC_ROLE_CTRL_CC_RA << TCPC_ROLE_CTRL_CC2_SHIFT);
		break;
	case TYPEC_CC_RD:
		reg = (TCPC_ROLE_CTRL_CC_RD << TCPC_ROLE_CTRL_CC1_SHIFT) |
			(TCPC_ROLE_CTRL_CC_RD << TCPC_ROLE_CTRL_CC2_SHIFT);
		break;
	case TYPEC_CC_RP_DEF:
		reg = (TCPC_ROLE_CTRL_CC_RP << TCPC_ROLE_CTRL_CC1_SHIFT) |
			(TCPC_ROLE_CTRL_CC_RP << TCPC_ROLE_CTRL_CC2_SHIFT) |
			(TCPC_ROLE_CTRL_RP_VAL_DEF <<
			 TCPC_ROLE_CTRL_RP_VAL_SHIFT);
		break;
	case TYPEC_CC_RP_1_5:
		reg = (TCPC_ROLE_CTRL_CC_RP << TCPC_ROLE_CTRL_CC1_SHIFT) |
			(TCPC_ROLE_CTRL_CC_RP << TCPC_ROLE_CTRL_CC2_SHIFT) |
			(TCPC_ROLE_CTRL_RP_VAL_1_5 <<
			 TCPC_ROLE_CTRL_RP_VAL_SHIFT);
		break;
	case TYPEC_CC_RP_3_0:
		reg = (TCPC_ROLE_CTRL_CC_RP << TCPC_ROLE_CTRL_CC1_SHIFT) |
			(TCPC_ROLE_CTRL_CC_RP << TCPC_ROLE_CTRL_CC2_SHIFT) |
			(TCPC_ROLE_CTRL_RP_VAL_3_0 <<
			 TCPC_ROLE_CTRL_RP_VAL_SHIFT);
		break;
	case TYPEC_CC_OPEN:
	default:
		reg = (TCPC_ROLE_CTRL_CC_OPEN << TCPC_ROLE_CTRL_CC1_SHIFT) |
			(TCPC_ROLE_CTRL_CC_OPEN << TCPC_ROLE_CTRL_CC2_SHIFT);
		break;
	}

	ret = dm_i2c_reg_write(tcpci->i2c, TCPC_ROLE_CTRL, reg);
	if (ret < 0)
		return ret;

	return 0;
}

static int tcpci_start_toggling(struct tcpc_dev *tcpc,
				enum typec_port_type port_type,
				enum typec_cc_status cc)
{
	int ret;
	struct tcpci *tcpci = tcpc_to_tcpci(tcpc);
	uint8_t reg = TCPC_ROLE_CTRL_DRP;

	if (port_type != TYPEC_PORT_DRP)
		return -EOPNOTSUPP;

	/* Handle vendor drp toggling */
	if (tcpci->data->start_drp_toggling) {
		ret = tcpci->data->start_drp_toggling(tcpci, tcpci->data, cc);
		if (ret < 0)
			return ret;
	}

	switch (cc) {
	default:
	case TYPEC_CC_RP_DEF:
		reg |= (TCPC_ROLE_CTRL_RP_VAL_DEF <<
			TCPC_ROLE_CTRL_RP_VAL_SHIFT);
		break;
	case TYPEC_CC_RP_1_5:
		reg |= (TCPC_ROLE_CTRL_RP_VAL_1_5 <<
			TCPC_ROLE_CTRL_RP_VAL_SHIFT);
		break;
	case TYPEC_CC_RP_3_0:
		reg |= (TCPC_ROLE_CTRL_RP_VAL_3_0 <<
			TCPC_ROLE_CTRL_RP_VAL_SHIFT);
		break;
	}

	if (cc == TYPEC_CC_RD)
		reg |= (TCPC_ROLE_CTRL_CC_RD << TCPC_ROLE_CTRL_CC1_SHIFT) |
			   (TCPC_ROLE_CTRL_CC_RD << TCPC_ROLE_CTRL_CC2_SHIFT);
	else
		reg |= (TCPC_ROLE_CTRL_CC_RP << TCPC_ROLE_CTRL_CC1_SHIFT) |
			   (TCPC_ROLE_CTRL_CC_RP << TCPC_ROLE_CTRL_CC2_SHIFT);
	ret = dm_i2c_reg_write(tcpci->i2c, TCPC_ROLE_CTRL, reg);
	if (ret < 0)
		return ret;
	return dm_i2c_reg_write(tcpci->i2c, TCPC_COMMAND,
			    TCPC_CMD_LOOK4CONNECTION);
}

static enum typec_cc_status tcpci_to_typec_cc(unsigned int cc, bool sink)
{
	switch (cc) {
	case 0x1:
		return sink ? TYPEC_CC_RP_DEF : TYPEC_CC_RA;
	case 0x2:
		return sink ? TYPEC_CC_RP_1_5 : TYPEC_CC_RD;
	case 0x3:
		if (sink)
			return TYPEC_CC_RP_3_0;
		/* fall through */
	case 0x0:
	default:
		return TYPEC_CC_OPEN;
	}
}

static int tcpci_get_cc(struct tcpc_dev *tcpc,
			enum typec_cc_status *cc1, enum typec_cc_status *cc2)
{
	struct tcpci *tcpci = tcpc_to_tcpci(tcpc);
	uint8_t reg;
	int ret;
	int cc1_sink = 0, cc2_sink = 0;

	if (tcpci->data->get_cc)
		return tcpci->data->get_cc(tcpci, tcpci->data, cc1, cc2);

	ret = dm_i2c_read(tcpci->i2c, TCPC_ROLE_CTRL, &reg, 1);
	if (!ret) {
		if (((reg >> TCPC_ROLE_CTRL_CC1_SHIFT) &
		     TCPC_ROLE_CTRL_CC1_MASK) == TCPC_ROLE_CTRL_CC_RD)
			cc1_sink = 1;
		if (((reg >> TCPC_ROLE_CTRL_CC2_SHIFT) &
		     TCPC_ROLE_CTRL_CC2_MASK) == TCPC_ROLE_CTRL_CC_RD)
			cc2_sink = 1;
	}

	ret = dm_i2c_read(tcpci->i2c, TCPC_CC_STATUS, &reg, 1);
	if (!ret)
		return ret;

	*cc1 = tcpci_to_typec_cc((reg >> TCPC_CC_STATUS_CC1_SHIFT) &
				 TCPC_CC_STATUS_CC1_MASK,
				 (reg & TCPC_CC_STATUS_TERM) | cc1_sink);
	*cc2 = tcpci_to_typec_cc((reg >> TCPC_CC_STATUS_CC2_SHIFT) &
				 TCPC_CC_STATUS_CC2_MASK,
				 (reg & TCPC_CC_STATUS_TERM) | cc2_sink);

	return 0;
}

static int tcpci_set_polarity(struct tcpc_dev *tcpc,
			      enum typec_cc_polarity polarity)
{
	struct tcpci *tcpci = tcpc_to_tcpci(tcpc);
	uint8_t reg;
	int ret;

	if (tcpci->data->set_polarity)
		return tcpci->data->set_polarity(tcpci, tcpci->data, polarity);

	/* Keep the disconnect cc line open */
	ret = dm_i2c_read(tcpci->i2c, TCPC_ROLE_CTRL, &reg, 1);
	if (ret < 0)
		return ret;

	if (polarity == TYPEC_POLARITY_CC2)
		reg |= TCPC_ROLE_CTRL_CC_OPEN << TCPC_ROLE_CTRL_CC1_SHIFT;
	else
		reg |= TCPC_ROLE_CTRL_CC_OPEN << TCPC_ROLE_CTRL_CC2_SHIFT;
	ret = dm_i2c_reg_write(tcpci->i2c, TCPC_ROLE_CTRL, reg);
	if (ret < 0)
		return ret;

	return dm_i2c_reg_write(tcpci->i2c, TCPC_TCPC_CTRL,
			   (polarity == TYPEC_POLARITY_CC2) ?
			   TCPC_TCPC_CTRL_ORIENTATION : 0);
}

static int tcpci_set_vconn(struct tcpc_dev *tcpc, bool enable)
{
	struct tcpci *tcpci = tcpc_to_tcpci(tcpc);
	int ret;
	uint8_t reg, mask, val;

	/* Handle vendor set vconn */
	if (tcpci->data->set_vconn) {
		ret = tcpci->data->set_vconn(tcpci, tcpci->data, enable);
		if (ret < 0)
			return ret;
	}


	ret = dm_i2c_read(tcpci->i2c, TCPC_POWER_CTRL, &reg, 1);
	if (ret < 0)
		return ret;
	mask = TCPC_POWER_CTRL_VCONN_ENABLE;
	reg &= ~mask;
	val = enable ? TCPC_POWER_CTRL_VCONN_ENABLE : 0;

	return dm_i2c_reg_write(tcpci->i2c, TCPC_POWER_CTRL,
				val | reg);
}

static int tcpci_set_roles(struct tcpc_dev *tcpc, bool attached,
			   enum typec_role role, enum typec_data_role data)
{
	struct tcpci *tcpci = tcpc_to_tcpci(tcpc);
	uint8_t reg;
	int ret;

	reg = PD_REV20 << TCPC_MSG_HDR_INFO_REV_SHIFT;
	if (role == TYPEC_SOURCE)
		reg |= TCPC_MSG_HDR_INFO_PWR_ROLE;
	if (data == TYPEC_HOST)
		reg |= TCPC_MSG_HDR_INFO_DATA_ROLE;
	ret = dm_i2c_reg_write(tcpci->i2c, TCPC_MSG_HDR_INFO, reg);
	if (ret < 0)
		return ret;

	return 0;
}

static int tcpci_set_pd_rx(struct tcpc_dev *tcpc, bool enable)
{
	struct tcpci *tcpci = tcpc_to_tcpci(tcpc);
	uint8_t reg = 0;
	int ret;

	if (enable)
		reg = TCPC_RX_DETECT_SOP | TCPC_RX_DETECT_HARD_RESET;
	ret = dm_i2c_reg_write(tcpci->i2c, TCPC_RX_DETECT, reg);
	if (ret < 0)
		return ret;

	return 0;
}

static int tcpci_get_vbus(struct tcpc_dev *tcpc)
{
	struct tcpci *tcpci = tcpc_to_tcpci(tcpc);
	uint8_t reg;
	int ret;

	ret = dm_i2c_read(tcpci->i2c, TCPC_POWER_STATUS, &reg, 1);
	if (ret < 0)
		return ret;

	return !!(reg & TCPC_POWER_STATUS_VBUS_PRES);
}

static int tcpci_set_vbus(struct tcpc_dev *tcpc, bool source, bool sink)
{
	struct tcpci *tcpci = tcpc_to_tcpci(tcpc);
	int ret;

	/* Disable both source and sink first before enabling anything */

	if (!source) {
		ret = dm_i2c_reg_write(tcpci->i2c, TCPC_COMMAND,
				   TCPC_CMD_DISABLE_SRC_VBUS);
		if (ret < 0)
			return ret;
	}

	if (!sink) {
		ret = dm_i2c_reg_write(tcpci->i2c, TCPC_COMMAND,
					 TCPC_CMD_DISABLE_SINK_VBUS);
		if (ret < 0)
			return ret;
	}

	if (source) {
		ret = dm_i2c_reg_write(tcpci->i2c, TCPC_COMMAND,
				   TCPC_CMD_SRC_VBUS_DEFAULT);
		if (ret < 0)
			return ret;
	}

	if (sink) {
		ret = dm_i2c_reg_write(tcpci->i2c, TCPC_COMMAND,
				   TCPC_CMD_SINK_VBUS);
		if (ret < 0)
			return ret;
	}

	return 0;
}

static int tcpci_pd_transmit(struct tcpc_dev *tcpc,
			     enum tcpm_transmit_type type,
			     const struct pd_message *msg)
{
	struct tcpci *tcpci = tcpc_to_tcpci(tcpc);
	u16 header = msg ? le16_to_cpu(msg->header) : 0;
	uint8_t reg;
	unsigned int cnt;
	int ret;
	pr_debug("%s %d\n", __func__, (int) type);

	cnt = msg ? pd_header_cnt(header) * 4 : 0;
	ret = dm_i2c_reg_write(tcpci->i2c, TCPC_TX_BYTE_CNT, cnt + 2);
	if (ret < 0)
		return ret;

	ret = tcpci_write16(tcpci, TCPC_TX_HDR, header);
	if (ret < 0)
		return ret;

	if (cnt > 0) {
		ret = dm_i2c_write(tcpci->i2c, TCPC_TX_DATA,
				       (uint8_t*) &msg->payload, cnt);
		if (ret < 0)
			return ret;
	}

	reg = (PD_RETRY_COUNT << TCPC_TRANSMIT_RETRY_SHIFT) |
		(type << TCPC_TRANSMIT_TYPE_SHIFT);
	ret = dm_i2c_reg_write(tcpci->i2c, TCPC_TRANSMIT, reg);
	if (ret < 0)
		return ret;

	return 0;
}

static int tcpci_get_tx_status(struct tcpc_dev *tcpc,
			      enum tcpm_transmit_status *status)
{
	struct tcpci *tcpci = tcpc_to_tcpci(tcpc);
	u16 alert;
	int ret;

	ret = tcpci_read16(tcpci, TCPC_ALERT, &alert);
	pr_debug("%s read ALERT ret:%d\n", __func__, ret);
	if (ret) {
		return ret;
	}
	if (alert & TCPC_ALERT_TX_SUCCESS)
		*status = TCPC_TX_SUCCESS;
	else if (alert & TCPC_ALERT_TX_DISCARDED)
		*status = TCPC_TX_DISCARDED;
	else if (alert & TCPC_ALERT_TX_FAILED)
		*status = TCPC_TX_FAILED;

	pr_debug("%s status = %d\n", __func__, *status);
	return 0;
}

static int tcpci_init(struct tcpc_dev *tcpc)
{
	struct tcpci *tcpci = tcpc_to_tcpci(tcpc);
	unsigned long timeout_ms = get_timer(0) + 2000; /* XXX */
	uint8_t reg;
	int ret;

	while (time_before(get_timer(0), timeout_ms)) {
		ret = dm_i2c_read(tcpci->i2c, TCPC_POWER_STATUS, &reg, 1);
		if (ret < 0)
			return ret;
		if (!(reg & TCPC_POWER_STATUS_UNINIT))
			break;
		udelay(10000);
	}
	if (time_after(get_timer(0), timeout_ms))
		return -ETIMEDOUT;

	/* Handle vendor init */
	if (tcpci->data->init) {
		ret = tcpci->data->init(tcpci, tcpci->data);
		if (ret < 0)
			return ret;
	}

	/* Clear all events */
	ret = tcpci_write16(tcpci, TCPC_ALERT, 0xffff);
	if (ret < 0)
		return ret;

	if (tcpci->controls_vbus)
		reg = TCPC_POWER_STATUS_VBUS_PRES;
	else
		reg = 0;

	ret = dm_i2c_reg_write(tcpci->i2c, TCPC_POWER_STATUS_MASK, reg);
	if (ret < 0)
		return ret;

	/* Enable Vbus detection */
	ret = dm_i2c_reg_write(tcpci->i2c, TCPC_COMMAND,
			   TCPC_CMD_ENABLE_VBUS_DETECT);
	if (ret < 0)
		return ret;

	reg = TCPC_ALERT_TX_SUCCESS | TCPC_ALERT_TX_FAILED |
		TCPC_ALERT_TX_DISCARDED | TCPC_ALERT_RX_STATUS |
		TCPC_ALERT_RX_HARD_RST | TCPC_ALERT_CC_STATUS;
	if (tcpci->controls_vbus)
		reg |= TCPC_ALERT_POWER_STATUS;
	//tcpci_read16(tcpci, TCPC_ALERT_MASK, &mask);
	ret = tcpci_write16(tcpci, TCPC_ALERT_MASK, reg);
	//tcpci_read16(tcpci, TCPC_ALERT_MASK, &mask);
	return ret;
}

int tcpci_check_alert(struct tcpci *tcpci)
{
	u16 status;

	tcpci_read16(tcpci, TCPC_ALERT, &status);

	/*
	 * Clear alert status for everything except RX_STATUS, which shouldn't
	 * be cleared until we have successfully retrieved message.
	 */
	if (status & ~TCPC_ALERT_RX_STATUS)
		tcpci_write16(tcpci, TCPC_ALERT,
			      status & ~TCPC_ALERT_RX_STATUS);

	if (status & TCPC_ALERT_CC_STATUS)
		tcpm_cc_change(tcpci->port);

	if (status & TCPC_ALERT_POWER_STATUS) {
		uint8_t reg;

		dm_i2c_read(tcpci->i2c, TCPC_POWER_STATUS_MASK, &reg, 1);

		/*
		 * If power status mask has been reset, then the TCPC
		 * has reset.
		 */
		if (reg == 0xff)
			tcpm_tcpc_reset(tcpci->port);
		else
			tcpm_vbus_change(tcpci->port);
	}

	if (status & TCPC_ALERT_RX_STATUS) {
		struct pd_message msg;
		uint8_t cnt;
		int payload_cnt;
		u16 header;

		pr_warn("tcpci got RX_STATUS\n");
		dm_i2c_read(tcpci->i2c, TCPC_RX_BYTE_CNT, &cnt, 1);
		/*
		 * 'cnt' corresponds to READABLE_BYTE_COUNT in section 4.4.14
		 * of the TCPCI spec [Rev 2.0 Ver 1.0 October 2017] and is
		 * defined in table 4-36 as one greater than the number of
		 * bytes received. And that number includes the header. So:
		 */
		if (cnt > 3)
			payload_cnt = cnt - (1 + sizeof(msg.header));
		else
			payload_cnt = 0;

		tcpci_read16(tcpci, TCPC_RX_HDR, &header);
		msg.header = cpu_to_le16(header);

		if (WARN_ON(payload_cnt > sizeof(msg.payload))) {
			pr_err("WARN_ON: cnt=%u > msg.payload=%d\n !!!!!", payload_cnt,
					(int)sizeof(msg.payload));
			payload_cnt = sizeof(msg.payload);
		}

		if (payload_cnt > 0)
			dm_i2c_read(tcpci->i2c, TCPC_RX_DATA,
					(uint8_t*) &msg.payload, payload_cnt);

		/* Read complete, clear RX status alert bit */
		tcpci_write16(tcpci, TCPC_ALERT, TCPC_ALERT_RX_STATUS);

		tcpm_pd_receive(tcpci->port, &msg);
	}

	if (status & TCPC_ALERT_RX_HARD_RST)
		tcpm_pd_hard_reset(tcpci->port);

	if (status & TCPC_ALERT_TX_SUCCESS)
		tcpm_pd_transmit_complete(tcpci->port, TCPC_TX_SUCCESS);
	else if (status & TCPC_ALERT_TX_DISCARDED)
		tcpm_pd_transmit_complete(tcpci->port, TCPC_TX_DISCARDED);
	else if (status & TCPC_ALERT_TX_FAILED)
		tcpm_pd_transmit_complete(tcpci->port, TCPC_TX_FAILED);

	return 0;
}

static int tcpci_remove(struct tcpci *tcpci)
{
	int err;

	/* Disable chip interrupts before unregistering port */
	err = tcpci_write16(tcpci, TCPC_ALERT_MASK, 0);
	if (err < 0)
		return err;

	tcpci_unregister_port(tcpci);
	return 0;
}

int tcpci_set_bist(struct tcpci *tcpci, bool enable)
{
	int ret;
	u8 reg;

	ret = dm_i2c_read(tcpci->i2c, TCPC_TCPC_CTRL, &reg, 1);
	if (ret)
		return ret;

	if (enable)
		reg |= TCPC_TCPC_CTRL_BIST;
	else
		reg &= ~TCPC_TCPC_CTRL_BIST;

	return dm_i2c_reg_write(tcpci->i2c, TCPC_TCPC_CTRL, reg);
}

int tcpci_main_loop(struct tcpci *tcpci)
{
	ulong timeout = get_timer(0) + MAIN_LOOP_TIMEOUT_MS;
	ulong time = get_timer(0);

	pr_info("%s starts at %lu.\n", __func__, time);

	while (time_before(get_timer(0), timeout)) {
		time = get_timer(0);
		tcpci_check_alert(tcpci);
		tcpm_handle_pd_event(tcpci->port);
		tcpm_run_state_machine(tcpci->port);

		if (tcpm_is_snk_ready(tcpci->port)) {
			if (time_before(time + MAIN_LOOP_BUFFER_MS, timeout)) {
				/* Reduce timeout to PS_RDY timestamp + MAIN_LOOP_BUFFER_MS
				 * The buffer is to handle some follow-up messages like DR_SWAP,
				 * VCONN_SWAP.
				 */
				timeout = time + MAIN_LOOP_BUFFER_MS;
			}
		}
	}

	/* TCPC Spec 4.4.5.1: enable BIST to send GOOD_CRC unconditionally
	 * without buffering in the gap before kernel driver is ready.
	 */
	tcpci_set_bist(tcpci, 1);

	tcpci_remove(tcpci);

	pr_info("%s finishes at %lu.\n", __func__, time);
	pr_info("TCPM is SNK_READY: %d.\n", tcpm_is_snk_ready(tcpci->port));
	return 0;
}

static int tcpci_parse_config(struct tcpci *tcpci)
{
	tcpci->controls_vbus = true; /* XXX */
	ofnode ofnode;
	ofnode = ofnode_find_subnode(dev_ofnode(tcpci->dev),
							 "connector");
	if (!ofnode_valid(ofnode)) {
		dev_err(tcpci->dev, "Can't find connector node.\n");
		return -EINVAL;
	}

	tcpci->tcpc.ofnode = devm_kzalloc(tcpci->dev, sizeof(ofnode), GFP_KERNEL);
	if (!tcpci->tcpc.ofnode)
		return -ENOMEM;

	*tcpci->tcpc.ofnode = ofnode;
	return 0;
}

struct tcpci *tcpci_register_port(struct udevice *dev, struct tcpci_data *data)
{
	struct tcpci *tcpci;
	int err;

	tcpci = devm_kzalloc(dev, sizeof(*tcpci), GFP_KERNEL);
	if (!tcpci)
		return ERR_PTR(-ENOMEM);

	tcpci->dev = dev;
	tcpci->data = data;
	tcpci->i2c = data->i2c;

	tcpci->tcpc.init = tcpci_init;
	tcpci->tcpc.get_vbus = tcpci_get_vbus;
	tcpci->tcpc.set_vbus = tcpci_set_vbus;
	tcpci->tcpc.set_cc = tcpci_set_cc;
	tcpci->tcpc.get_cc = tcpci_get_cc;
	tcpci->tcpc.set_polarity = tcpci_set_polarity;
	tcpci->tcpc.set_vconn = tcpci_set_vconn;
	tcpci->tcpc.start_toggling = tcpci_start_toggling;

	tcpci->tcpc.set_pd_rx = tcpci_set_pd_rx;
	tcpci->tcpc.set_roles = tcpci_set_roles;
	tcpci->tcpc.pd_transmit = tcpci_pd_transmit;
	tcpci->tcpc.get_tx_status = tcpci_get_tx_status;;

	err = tcpci_parse_config(tcpci);
	if (err < 0)
		return ERR_PTR(err);

	tcpci->port = tcpm_register_port(tcpci->dev, &tcpci->tcpc);
	if (IS_ERR(tcpci->port))
		return ERR_CAST(tcpci->port);

	return tcpci;
}
EXPORT_SYMBOL_GPL(tcpci_register_port);

void tcpci_unregister_port(struct tcpci *tcpci)
{
	tcpm_unregister_port(tcpci->port);
}
EXPORT_SYMBOL_GPL(tcpci_unregister_port);
