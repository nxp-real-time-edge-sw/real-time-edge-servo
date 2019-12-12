// SPDX-License-Identifier: (GPL-2.0 OR MIT)
/*
 * Copyright 2018-2020 NXP
 */

#ifndef NSER_SDO_H_
#define NSER_SDO_H_

#include <stdio.h>
#include "config.h"
#include "nservo.h"

/**************** sdo API ***************************/

#define sdo_set(name, type) \
	static inline uint32_t nser_sdo_set_##name(nser_axle *ns_axle, type v) \
	{\
		return set_sdo(name, ns_axle, (void *)&v, sizeof(v)); \
	}

#define sdo_get(name, type) \
	static inline uint32_t nser_sdo_get_##name(nser_axle *ns_axle, type *v) \
	{\
		return get_sdo(name, ns_axle, (void *)v, sizeof(*v), NULL); \
	}

#define sdo_set_array(name) \
	static inline uint32_t nser_sdo_set_##name(nser_axle *ns_axle, uint8_t *p, size_t len) \
	{\
		return set_sdo(name, ns_axle, p, len); \
	}

#define sdo_get_array(name) \
	static inline uint32_t nser_sdo_get_##name(nser_axle *ns_axle, uint8_t *p, size_t size, size_t *result) \
	{\
		return get_sdo(name, ns_axle, p, size, result); \
	}
#define sdo_set_string(name) \
	static inline uint32_t nser_sdo_set_##name(nser_axle *ns_axle, char *p) \
	{\
		return set_sdo(name, ns_axle, p, strlen(p)); \
	}

#define sdo_get_string(name) \
	static inline uint32_t nser_sdo_get_##name(nser_axle *ns_axle, uint8_t *p, size_t size) \
	{\
		return get_sdo(name, ns_axle, p, size, NULL); \
	}

/********** Common functions **********************/

sdo_set(Abort_connection_option_code, int16_t)
sdo_get(Abort_connection_option_code, int16_t)
sdo_get(Error_code, int16_t)
sdo_get(Motor_type, uint16_t)

sdo_set_string(Motor_catalog_number)
sdo_get_string(Motor_catalog_number)
sdo_set_string(Motor_manufacturer)
sdo_get_string(Motor_manufacturer)
sdo_set_string(http_motor_catalog_address)
sdo_get_string(http_motor_catalog_address)
sdo_set_array(Motor_calibration_date)
sdo_get_array(Motor_calibration_date)
sdo_set(Motor_service_period, uint32_t)
sdo_get(Motor_service_period, uint32_t)

sdo_set(Supported_drive_modes, uint32_t)
sdo_get(Supported_drive_modes, uint32_t)

sdo_get_string(Drive_catalog_number)
sdo_get_string(Drive_manufacturer)
sdo_get_string(http_drive_catalog_address)

sdo_set(Digital_inputs, uint32_t)
sdo_get(Digital_inputs, uint32_t)
sdo_set(DO_Physical_outputs, uint32_t)
sdo_get(DO_Physical_outputs, uint32_t)
sdo_set(DO_Bitmask, uint32_t)
sdo_get(DO_Bitmask, uint32_t)

/********* Factor group functions ***************/
sdo_set(Position_notation_index, int8_t)
sdo_get(Position_notation_index, int8_t)
sdo_set(Position_dimension_index, uint8_t)
sdo_get(Position_dimension_index, uint8_t)

sdo_set(Velocity_notation_index, int8_t)
sdo_get(Velocity_notation_index, int8_t)
sdo_set(Velocity_dimension_index, uint8_t)
sdo_get(Velocity_dimension_index, uint8_t)

sdo_set(Acceleration_notation_index, int8_t)
sdo_get(Acceleration_notation_index, int8_t)
sdo_set(Acceleration_dimension_index, uint8_t)
sdo_get(Acceleration_dimension_index, uint8_t)

sdo_set(PRE_Encoder_increments, uint32_t)
sdo_get(PRE_Encoder_increments, uint32_t)
sdo_set(PRE_Motor_revolutions, uint32_t)
sdo_get(PRE_Motor_revolutions, uint32_t)

sdo_set(VNR_Encoder_increments_per_second, uint32_t)
sdo_get(VNR_Encoder_increments_per_second, uint32_t)
sdo_set(VNR_Motor_revolutions_per_second, uint32_t)
sdo_get(VNR_Motor_revolutions_per_second, uint32_t)

sdo_set(GR_Motor_revolutions, uint32_t)
sdo_get(GR_Motor_revolutions, uint32_t)
sdo_set(GR_Shaft_revolutions, uint32_t)
sdo_get(GR_Shaft_revolutions, uint32_t)

sdo_get(FC_Feed, uint32_t)
sdo_set(FC_Feed, uint32_t)
sdo_get(FC_Shaft_revolutions, uint32_t)
sdo_set(FC_Shaft_revolutions, uint32_t)

sdo_get(PF_Numerator, uint32_t)
sdo_set(PF_Numerator, uint32_t)
sdo_get(PF_Feed_constant, uint32_t)
sdo_set(PF_Feed_constant, uint32_t)

sdo_set(VEF_Numerator, uint32_t)
sdo_get(VEF_Numerator, uint32_t)
sdo_set(VEF_Divisor, uint32_t)
sdo_get(VEF_Divisor, uint32_t)

sdo_set(VF1_Numerator, uint32_t)
sdo_get(VF1_Numerator, uint32_t)
sdo_set(VF1_Divisor, uint32_t)
sdo_get(VF1_Divisor, uint32_t)

sdo_set(VF2_Numerator, uint32_t)
sdo_get(VF2_Numerator, uint32_t)
sdo_set(VF2_Divisor, uint32_t)
sdo_get(VF2_Divisor, uint32_t)

sdo_set(AF_Numerator, uint32_t)
sdo_get(AF_Numerator, uint32_t)
sdo_set(AF_Divisor, uint32_t)
sdo_get(AF_Divisor, uint32_t)

sdo_set(Polarity, uint8_t)
sdo_get(Polarity, uint8_t)

/************ Device control func: *******************/

sdo_set(Controlword, uint16_t)
sdo_set(Shutdown_option_code, uint16_t)
sdo_set(Disable_operation_option_code, uint16_t)
sdo_set(Quick_stop_option_code, uint16_t)
sdo_set(Halt_option_code, uint16_t)
sdo_set(Fault_reaction_option_code, uint16_t)
sdo_set(Modes_of_operation, int16_t)

sdo_get(Controlword, uint16_t)
sdo_get(Statusword, uint16_t)
sdo_get(Shutdown_option_code, uint16_t)
sdo_get(Disable_operation_option_code, uint16_t)
sdo_get(Quick_stop_option_code, uint16_t)
sdo_get(Halt_option_code, uint16_t)
sdo_get(Fault_reaction_option_code, uint16_t)
sdo_get(Modes_of_operation, int16_t)
sdo_get(Modes_of_operation_display, int16_t)

/***** Profile posion mode ******/
sdo_set(Target_position, int32_t)
sdo_set(PRL_Min_position_range_limit, int32_t)
// PRL :  Position range limit
sdo_set(PRL_Max_position_range_limit, int32_t)
sdo_set(SPL_Min_position_limit, int32_t)
// SPL : software position limit
sdo_set(SPL_Max_position_limit, int32_t)
sdo_set(Max_profile_velocity, uint32_t)
sdo_set(Max_motor_speed, uint32_t)
sdo_set(Profile_velocity, uint32_t)
sdo_set(End_velocity, uint32_t)
sdo_set(Profile_acceleration, uint32_t)
sdo_set(Profile_deceleration, uint32_t)
sdo_set(Quick_stop_deceleration, uint32_t)
sdo_set(Motion_profile_type, int16_t)
sdo_set(Max_acceleration, uint32_t)
sdo_set(Max_deceleration, uint32_t)

sdo_get(Target_position, int32_t)
sdo_get(PRL_Min_position_range_limit, int32_t)
// PRL :  Position range limit
sdo_get(PRL_Max_position_range_limit, int32_t)
sdo_get(SPL_Min_position_limit, int32_t)
// SPL : software position limit
sdo_get(SPL_Max_position_limit, int32_t)
sdo_get(Max_profile_velocity, uint32_t)
sdo_get(Max_motor_speed, uint32_t)
sdo_get(Profile_velocity, uint32_t)
sdo_get(End_velocity, uint32_t)
sdo_get(Profile_acceleration, uint32_t)
sdo_get(Profile_deceleration, uint32_t)
sdo_get(Quick_stop_deceleration, uint32_t)
sdo_get(Motion_profile_type, int16_t)
sdo_get(Max_acceleration, uint32_t)
sdo_get(Max_deceleration, uint32_t)

/**** Homing mode ******/
sdo_set(Home_offset, int32_t)
sdo_set(Home_method, int8_t)
sdo_set(HS_Speed_during_search_for_switch, uint32_t)
sdo_set(HS_Speed_during_search_for_zero, uint32_t)
sdo_set(Homing_acceleration, uint32_t)

sdo_get(Home_offset, int32_t)
sdo_get(Home_method, int8_t)
sdo_get(HS_Speed_during_search_for_switch, uint32_t)
sdo_get(HS_Speed_during_search_for_zero, uint32_t)
sdo_get(Homing_acceleration, uint32_t)

/****Position control function*******/
sdo_set(Following_error_window, uint32_t)
sdo_set(Following_error_time_out, uint16_t)
sdo_set(Position_window, uint32_t)
sdo_set(Position_window_time, uint16_t)
sdo_set_array(Position_control_parameter_set)

sdo_get(Position_demand_value, int32_t)
sdo_get(Position_actual_value_inc, int32_t)
sdo_get(Position_actual_value, int32_t)
sdo_get(Following_error_window, uint32_t)
sdo_get(Following_error_time_out, uint16_t)
sdo_get(Position_window, uint32_t)
sdo_get(Position_window_time, uint16_t)
sdo_get(Following_error_actual_value, int32_t)
sdo_get(Control_effort, int32_t)
sdo_get_array(Position_control_parameter_set)
sdo_get(Position_demand_value_inc, int32_t)

/*****Interpolate position mode ******/
sdo_set(Interpolation_sub_mode_select, int16_t)
sdo_set_array(Interpolation_data_record_0)
sdo_set(ITP_ip_time_units, uint8_t)
// ITP : Interpolation time period
sdo_set(ITP_ip_time_index, int8_t)
sdo_set(ISD_Synchronize_on_group, uint8_t)
// ISD : Interpolation sync definition
sdo_set(ISD_ip_sync_every_n_event, uint8_t)
sdo_set(IDC_Actual_buffer_size, uint32_t)
sdo_set(IDC_Buffer_organization, uint32_t)
sdo_set(IDC_Buffer_position, uint32_t)
sdo_set(IDC_Size_of_data_record, uint32_t)
sdo_set(IDC_Buffer_clear, uint32_t)

sdo_get(Interpolation_sub_mode_select, int16_t)
sdo_get_array(Interpolation_data_record_0)
sdo_get(ITP_ip_time_units, uint8_t)
// ITP : Interpolation time period
sdo_get(ITP_ip_time_index, int8_t)
sdo_get(ISD_Synchronize_on_group, uint8_t)
// ISD : Interpolation sync definition
sdo_get(ISD_ip_sync_every_n_event, uint8_t)
sdo_get(IDC_Maxinum_buffer_size, uint32_t)
// IDC : Interpolation data configuration
sdo_get(IDC_Actual_buffer_size, uint32_t)
sdo_get(IDC_Buffer_organization, uint32_t)
sdo_get(IDC_Buffer_position, uint32_t)
sdo_get(IDC_Size_of_data_record, uint32_t)
sdo_get(IDC_Buffer_clear, uint32_t)

/******* Profile velocity mode*******/
sdo_set(Velocity_sensor_actual_value, int32_t)
sdo_set(Sensor_selection_code, int16_t)
sdo_set(Velocity_demand_value, int32_t)
sdo_set(Velocity_actual_value, int32_t)
sdo_set(Velocity_window, uint16_t)
sdo_set(Velocity_window_time, uint16_t)
sdo_set(Velocity_threshold, uint16_t)
sdo_set(Velocity_threshold_time, uint16_t)
sdo_set(Target_velocity, int32_t)
sdo_set(Max_slippage, int32_t)
sdo_set(VCPS_Gain, uint16_t)
// VCPS : Velocity control parameter set
sdo_set(VCPS_integration_time_constant, uint16_t)

sdo_get(Velocity_sensor_actual_value, int32_t)
sdo_get(Sensor_selection_code, int16_t)
sdo_get(Velocity_demand_value, int32_t)
sdo_get(Velocity_actual_value, int32_t)
sdo_get(Velocity_window, uint16_t)
sdo_get(Velocity_window_time, uint16_t)
sdo_get(Velocity_threshold, uint16_t)
sdo_get(Velocity_threshold_time, uint16_t)
sdo_get(Target_velocity, int32_t)
sdo_get(Max_slippage, int32_t)
sdo_get(VCPS_Gain, uint16_t)
// VCPS : Velocity control parameter set
sdo_get(VCPS_integration_time_constant, uint16_t)

/***** Profile torque mode*******/
sdo_set(Target_torque, int16_t)
sdo_set(Max_torque, uint16_t)
sdo_set(Max_current, uint16_t)
sdo_set(Motor_rated_currect, uint32_t)
sdo_set(Motor_reted_torque, uint32_t)
sdo_set(Torque_slope, uint32_t)
sdo_set(Torque_profile_type, int16_t)
sdo_set_array(Power_stage_parameters)
sdo_set_array(Torque_control_parameters)

sdo_get(Target_torque, int16_t)
sdo_get(Max_torque, uint16_t)
sdo_get(Max_current, uint16_t)
sdo_get(Torque_demand_value, int16_t)
sdo_get(Motor_rated_currect, uint32_t)
sdo_get(Motor_reted_torque, uint32_t)
sdo_get(Torque_actual_value, int16_t)
sdo_get(Current_actual_value, int16_t)
sdo_get(DC_link_circuit_voltage, uint32_t)
sdo_get(Torque_slope, uint32_t)
sdo_get(Torque_profile_type, int16_t)
sdo_get_array(Power_stage_parameters)
sdo_get_array(Torque_control_parameters)

/******Velocity mode*******/
sdo_set(vl_target_velocity, int16_t)
sdo_set(vl_velocity_demand, int16_t)
sdo_set(vl_control_effort, int16_t)
sdo_set(vl_manipulated_velocity, int16_t)
sdo_set(VV_min_amount, uint32_t)
//VV : Vl velocity
sdo_set(VV_max_amount, uint32_t)
sdo_set(VV_min_pos, uint32_t)
sdo_set(VV_max_pos, uint32_t)
sdo_set(VV_min_neg, uint32_t)
sdo_set(VV_max_neg, uint32_t)
sdo_set(VVA_Delta_speed, uint32_t)
// VVA : vl velocity acceleration
sdo_set(VVA_Delta_time, uint16_t)
sdo_set(VVD_Delta_speed, uint32_t)
// VVD : vl velocity deceleration
sdo_set(VVD_Delta_time, uint16_t)
sdo_set(VVQS_Delta_speed, uint32_t)
// VVQS : vl velocity quick stop
sdo_set(VVQS_Delta_time, uint16_t)
sdo_set(VSPF_numerator, int16_t)
// VSPF : vl set-point factor
sdo_set(VSPF_denominator, int16_t)
sdo_set(VDF_numerator, int32_t)
// VDF : vl dimension factor
sdo_set(VDF_denominator, int32_t)
sdo_set(vl_pole_number, uint8_t)
sdo_set(vl_velocity_reference, uint32_t)
sdo_set(vl_ramp_function_time, uint32_t)
sdo_set(vl_slow_down_time, uint32_t)
sdo_set(vl_quick_stop_time, uint32_t)
sdo_set(vl_nominal_percentage, int16_t)
sdo_set(vl_percentage_demand, int16_t)
sdo_set(vl_actual_percentage, int16_t)
sdo_set(vl_manipulated_percentage, int16_t)
sdo_set(VVMA_min, uint32_t)
// VVMA : vl velocity motor amount
sdo_set(VVMA_max, uint32_t)
sdo_set(VVM_min_pos, uint32_t)
// VVM	: vl velocity motor
sdo_set(VVM_max_pos, uint32_t)
sdo_set(VVM_min_neg, uint32_t)
sdo_set(VVM_max_neg, uint32_t)
sdo_set(VFMA_min, uint32_t)
// VFMA : vl frequency motor amount
sdo_set(VFMA_max, uint32_t)
sdo_set(VFM_min_pos, uint32_t)
// VFM	: vl frequency motor
sdo_set(VFM_max_pos, uint32_t)
sdo_set(VFM_min_neg, uint32_t)
sdo_set(VFM_max_neg, uint32_t)
#endif /* NSER_SDO_H_ */
