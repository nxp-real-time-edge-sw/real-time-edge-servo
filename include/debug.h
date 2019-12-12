// SPDX-License-Identifier: (GPL-2.0 OR MIT)
/*
 * Copyright 2018-2020 NXP
 */

#ifndef __DEBUG____H_____
#define __DEBUG____H_____
#include <stdio.h>
#include <stdarg.h>
#define debug_level_info	3
#define debug_level_warning 	2
#define debug_level_error	1
#define debug_level_none	0

#ifndef debug_level
#define debug_level  debug_level_error
#endif

#define do_debug_info(fmt, args ...) do \
		{printf("-->Info:" fmt, \
		##args);} \
		while(0)

#define do_debug_warning(fmt, args ...) do \
		{printf("-->Warning: %s %s %d :" fmt,\
		__FILE__, __func__, __LINE__, ##args);} \
		while(0)

#define do_debug_error(fmt, args ...) do \
		{printf("-->Error: %s %s %d :" fmt,\
		__FILE__, __func__, __LINE__, ##args);} \
		while(0)

#if (debug_level >= debug_level_info)
#define debug_info(fmt, args...)  do_debug_info(fmt, ##args)
#define debug_warning(fmt, args...)  do_debug_warning(fmt, ##args)
#define debug_error(fmt, args...)  do_debug_error(fmt, ##args)
#elif (debug_level >= debug_level_warning)
#define debug_info(fmt, ...)
#define debug_warning(fmt, args...)  do_debug_warning(fmt, ##args)
#define debug_error(fmt, args...)  do_debug_error(fmt, ##args)
#elif (debug_level >= debug_level_error)
#define debug_info(fmt, ...)
#define debug_warning(fmt, ...)
#define debug_error(fmt, args...)  do_debug_error(fmt, ##args)
#else
#define debug_info(fmt, ...)
#define debug_warning(fmt, ...)
#define debug_error(fmt, ...)
#endif

#endif

