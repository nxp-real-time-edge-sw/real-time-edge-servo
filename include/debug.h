// SPDX-License-Identifier: (GPL-2.0 OR MIT)
/*
 * Copyright 2018-2020 NXP
 */

#ifndef __DEBUG____H_____
#define __DEBUG____H_____
#include <stdio.h>
#include <stdarg.h>
#define DEBUG_LEVEL_INFO	3
#define DEBUG_LEVEL_WARNING 	2
#define DEBUG_LEVEL_ERROR	1
#define DEBUG_LEVEL_NONE	0

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL  DEBUG_LEVEL_ERROR
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

#if (DEBUG_LEVEL >= DEBUG_LEVEL_INFO)
#define debug_info(fmt, args...)  do_debug_info(fmt, ##args)
#define debug_warning(fmt, args...)  do_debug_warning(fmt, ##args)
#define debug_error(fmt, args...)  do_debug_error(fmt, ##args)
#elif (DEBUG_LEVEL >= DEBUG_LEVEL_WARNING)
#define debug_info(fmt, ...)
#define debug_warning(fmt, args...)  do_debug_warning(fmt, ##args)
#define debug_error(fmt, args...)  do_debug_error(fmt, ##args)
#elif (DEBUG_LEVEL >= DEBUG_LEVEL_ERROR)
#define debug_info(fmt, ...)
#define debug_warning(fmt, ...)
#define debug_error(fmt, args...)  do_debug_error(fmt, ##args)
#else
#define debug_info(fmt, ...)
#define debug_warning(fmt, ...)
#define debug_error(fmt, ...)
#endif

#endif

