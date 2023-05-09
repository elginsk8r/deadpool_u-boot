/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2020 Sean Anderson <seanga2@gmail.com>
 * Copyright (c) 2013 Google, Inc
 *
 * (C) Copyright 2012
 * Pavel Herrmann <morpheus.ibis@gmail.com>
 * Marek Vasut <marex@denx.de>
 */

#ifndef _DM_DEVICE_COMPAT_H
#define _DM_DEVICE_COMPAT_H

#include <log.h>
#include <linux/build_bug.h>
#include <linux/compat.h>

/*
 * Define a new identifier which can be tested on by C code. A similar
 * definition is made for DEBUG in <log.h>.
 */
#ifdef VERBOSE_DEBUG
#define _VERBOSE_DEBUG 1
#else
#define _VERBOSE_DEBUG 0
#endif

/**
 * dev_printk_emit() - Emit a formatted log message
 * @cat: Category of the message
 * @level: Log level of the message
 * @fmt: Format string
 * @...: Arguments for @fmt
 *
 * This macro logs a message through the appropriate channel. It is a macro so
 * the if statements can be optimized out (as @level should be a constant known
 * at compile-time).
 *
 * If DEBUG or VERBOSE_DEBUG is defined, then some messages are always printed
 * (through printf()). This is to match the historical behavior of the dev_xxx
 * functions.
 *
 * If LOG is enabled, use log() to emit the message, otherwise print it based on
 * the console loglevel.
 */
#define dev_printk_emit(cat, level, fmt, ...) \
({ \
	if ((_DEBUG && level == LOGL_DEBUG) || \
	    (_VERBOSE_DEBUG && level == LOGL_DEBUG_CONTENT)) \
		printf(fmt, ##__VA_ARGS__); \
	else if (CONFIG_IS_ENABLED(LOG)) \
		log(cat, level, fmt, ##__VA_ARGS__); \
	else if (level < CONFIG_VAL(LOGLEVEL)) \
		printf(fmt, ##__VA_ARGS__); \
})

#endif
