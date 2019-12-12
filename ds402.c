// SPDX-License-Identifier: (GPL-2.0 OR MIT)
/*
 * Copyright 2018-2020 NXP
 */

#include <stdio.h>
#include "debug.h"
#include "config.h"
#include "nservo.h"
#include "ds402.h"

device_type get_device_type(uint32_t v) {
	uint32_t v0 = (v >> 15);
	if ((v0 & 0xff) == 0x01)
		return dt_Frequency_converter;
	if ((v0 & 0xff) == 0x02)
		return dt_Servo_drive;
	if ((v0 & 0xff) == 0x04)
		return dt_Stepper_motor;
	if ((v0 & 0xfff) == 0xfff)
		return dt_Multiple_device_module;
	return dt_None;
}

char *mode_of_operation_to_str(uint8_t mode) {
	return mode_of_operation_str[mode];
}

axle_state get_axle_state(uint16_t status_word) {
	if ((status_word & 0x4F) == 0x40)
		return switch_on_disable;
	if ((status_word & 0x6F) == 0x21)
		return ready_to_switch_on;
	if ((status_word & 0x6F) == 0x23)
		return switched_on;
	if ((status_word & 0x6F) == 0x27)
		return operation_enable;
	if ((status_word & 0x6F) == 0x07)
		return quick_stop_active;
	if ((status_word & 0x4F) == 0xF)
		return fault_reaction_active;
	if ((status_word & 0x4F) == 0x08)
		return fault;
	else
		return no_ready_to_switch_on;
}

