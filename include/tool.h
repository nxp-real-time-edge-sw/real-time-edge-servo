// SPDX-License-Identifier: (GPL-2.0 OR MIT)
/*
 * Copyright 2018-2020 NXP
 */

#ifndef TOOL_H_
#define TOOL_H_
#include "nservo.h"
#include "nser_pdo.h"
#define MAX_DOMAIN_SIZE	100
typedef struct {
	uint8_t *domain;
	uint8_t *rx;
	uint8_t *tx;
	int isUpdate;
	int initialed;
	int rx_len;
	int tx_len;
} domain_data;

typedef struct {
	domain_data *d_data;
	nser_axle *ns_axle;
} tool_axle;

typedef struct {
	domain_data *d_data;
	pthread_mutex_t mutex;
	tool_axle *t_axle;
	int isLoadXML;
	int isActivate;
	int isStart;
	int isFinished;
} tool_data;
typedef void (*pdo_cmd_fun_t)(tool_axle *t_axle, tool_data *t_data, void *v,
		int *s);

typedef struct {
	char *str;
	pdo_cmd_fun_t fun;
} pdo_cmd_t;

typedef uint32_t (*sdo_cmd_fun_t)(nser_axle *ns_axle, void *v, int *s);
typedef struct {
	char *str;
	sdo_cmd_fun_t fun;
} sdo_cmd_t;

pdo_cmd_fun_t find_pdo_cmd_fun(char *cmd);

sdo_cmd_fun_t find_sdo_cmd_fun(char *cmd);

#define tool_pdo_get(name, U, u) \
	static inline void tool_pdo_get_##name(tool_axle *t_axle, tool_data *t_data, void *v, int *s) \
	{\
	*s = sizeof(u);\
	pthread_mutex_lock(&t_data->mutex); \
					*(u*)v = EC_READ_##U(t_axle->d_data->rx + t_axle->ns_axle->pdos_offset[n_##name]); \
				pthread_mutex_unlock(&t_data->mutex);\
			}

#define tool_pdo_set(name, U, u ) \
	static inline void tool_pdo_set_##name(tool_axle *t_axle, tool_data *t_data, void *v, int *s) \
	{\
		*s = sizeof(u);\
		pthread_mutex_lock(&t_data->mutex);\
		EC_WRITE_##U(t_axle->d_data->tx + t_axle->ns_axle->pdos_offset[n_##name], *(u *)v); \
		t_axle->d_data->isUpdate = 1; \
		pthread_mutex_unlock(&t_data->mutex);\
	}

/*Only read*/
tool_pdo_get(Error_code, U16, uint16_t)
tool_pdo_get(Statusword, U16, uint16_t)
tool_pdo_get(Supported_drive_modes, U32, uint32_t)
tool_pdo_get(Following_error_actual_value, S32, int32_t)
tool_pdo_get(Control_effort, S32, int32_t)
tool_pdo_get(Position_demand_value_inc, S32, int32_t)
tool_pdo_get(IDC_Maxinum_buffer_size, U32, uint32_t)
tool_pdo_get(Velocity_sensor_actual_value, S32, int32_t)
tool_pdo_get(Velocity_demand_value, S32, int32_t)
tool_pdo_get(Velocity_actual_value, S32, int32_t)
tool_pdo_get(Max_slippage, S32, int32_t)
tool_pdo_get(Torque_demand_value, S16, int16_t)
tool_pdo_get(Torque_actual_value, S16, int16_t)
tool_pdo_get(Current_actual_value, S16, int16_t)
tool_pdo_get(DC_link_circuit_voltage, U32, uint32_t)
tool_pdo_get(vl_velocity_demand, S16, int16_t)
tool_pdo_get(vl_control_effort, S16, int16_t)
tool_pdo_get(vl_manipulated_velocity, S16, int16_t)
tool_pdo_get(vl_percentage_demand, S16, int16_t)
tool_pdo_get(vl_actual_percentage, S16, int16_t)
tool_pdo_get(vl_manipulated_percentage, S16, int16_t)

/*Only write*/
tool_pdo_set(IDC_Size_of_data_record, U8, uint8_t)
tool_pdo_set(IDC_Buffer_clear, U8, uint8_t)

/*RW*/
tool_pdo_set(Abort_connection_option_code, S16, int16_t)
tool_pdo_set(Motor_type, U16, uint16_t)
tool_pdo_set(Motor_service_period, U32, uint32_t)
tool_pdo_set(Digital_inputs, U32, uint32_t)
tool_pdo_set(DO_Physical_outputs, U32, uint32_t)
tool_pdo_set(DO_Bitmask, U32, uint32_t)
tool_pdo_set(Controlword, U16, uint16_t)
tool_pdo_set(Modes_of_operation, S8, int8_t)
tool_pdo_set(Modes_of_operation_display, S8, int8_t)
tool_pdo_set(PRE_Encoder_increments, U32, uint32_t)
tool_pdo_set(PRE_Motor_revolutions, U32, uint32_t)
tool_pdo_set(VNR_Encoder_increments_per_second, U32, uint32_t)
tool_pdo_set(VNR_Motor_revolutions_per_second, U32, uint32_t)
tool_pdo_set(GR_Motor_revolutions, U32, uint32_t)
tool_pdo_set(GR_Shaft_revolutions, U32, uint32_t)
tool_pdo_set(FC_Feed, U32, uint32_t)
tool_pdo_set(FC_Shaft_revolutions, U32, uint32_t)
tool_pdo_set(PF_Numerator, U32, uint32_t)
tool_pdo_set(PF_Feed_constant, U32, uint32_t)
tool_pdo_set(VEF_Numerator, U32, uint32_t)
tool_pdo_set(VEF_Divisor, U32, uint32_t)
tool_pdo_set(VF1_Numerator, U32, uint32_t)
tool_pdo_set(VF1_Divisor, U32, uint32_t)
tool_pdo_set(VF2_Numerator, U32, uint32_t)
tool_pdo_set(VF2_Divisor, U32, uint32_t)
tool_pdo_set(AF_Numerator, U32, uint32_t)
tool_pdo_set(AF_Divisor, U32, uint32_t)
tool_pdo_set(Polarity, U8, uint8_t)
tool_pdo_set(Target_position, S32, int32_t)
tool_pdo_set(PRL_Min_position_range_limit, S32, int32_t)
tool_pdo_set(PRL_Max_position_range_limit, S32, int32_t)
tool_pdo_set(SPL_Min_position_limit, S32, int32_t)
tool_pdo_set(SPL_Max_position_limit, S32, int32_t)
tool_pdo_set(Max_profile_velocity, U32, uint32_t)
tool_pdo_set(Max_motor_speed, U32, uint32_t)
tool_pdo_set(Profile_velocity, U32, uint32_t)
tool_pdo_set(End_velocity, U32, uint32_t)
tool_pdo_set(Profile_acceleration, U32, uint32_t)
tool_pdo_set(Profile_deceleration, U32, uint32_t)
tool_pdo_set(Quick_stop_deceleration, U32, uint32_t)
tool_pdo_set(Motion_profile_type, S16, int16_t)
tool_pdo_set(Max_acceleration, U32, uint32_t)
tool_pdo_set(Max_deceleration, U32, uint32_t)
tool_pdo_set(Home_offset, U32, uint32_t)
tool_pdo_set(Home_method, S8, int8_t)
tool_pdo_set(HS_Speed_during_search_for_switch, U32, uint32_t)
tool_pdo_set(HS_Speed_during_search_for_zero, U32, uint32_t)
tool_pdo_set(Homing_acceleration, U32, uint32_t)
tool_pdo_set(Position_demand_value, S32, int32_t)
tool_pdo_set(Position_actual_value_inc, S32, int32_t)
tool_pdo_set(Position_actual_value, S32, int32_t)
tool_pdo_set(Following_error_window, U32, uint32_t)
tool_pdo_set(Following_error_time_out, U16, uint16_t)
tool_pdo_set(Position_window, U32, uint32_t)
tool_pdo_set(Position_window_time, U16, uint16_t)
tool_pdo_set(Interpolation_sub_mode_select, S16, int16_t)
tool_pdo_set(Interpolation_data_record_0, S16, int16_t)
tool_pdo_set(ITP_ip_time_units, U8, uint8_t)
tool_pdo_set(ITP_ip_time_index, S8, int8_t)
tool_pdo_set(ISD_Synchronize_on_group, U8, uint8_t)
tool_pdo_set(ISD_ip_sync_every_n_event, U8, uint8_t)
tool_pdo_set(IDC_Actual_buffer_size, U32, uint32_t)
tool_pdo_set(IDC_Buffer_organization, U8, uint8_t)
tool_pdo_set(IDC_Buffer_position, U16, uint16_t)
tool_pdo_set(Sensor_selection_code, S16, int16_t)
tool_pdo_set(Velocity_window, U16, uint16_t)
tool_pdo_set(Velocity_window_time, U16, uint16_t)
tool_pdo_set(Velocity_threshold, U16, uint16_t)
tool_pdo_set(Velocity_threshold_time, U16, uint16_t)
tool_pdo_set(Target_velocity, S32, int32_t)
tool_pdo_set(VCPS_Gain, U16, uint16_t)
tool_pdo_set(VCPS_integration_time_constant, U16, uint16_t)
tool_pdo_set(VCPS_Gain_1, U16, uint16_t)
tool_pdo_set(VCPS_integration_time_constant_1, U16, uint16_t)
tool_pdo_set(Target_torque, S16, int16_t)
tool_pdo_set(Max_torque, U16, uint16_t)
tool_pdo_set(Max_current, U16, uint16_t)
tool_pdo_set(Motor_rated_currect, U32, uint32_t)
tool_pdo_set(Motor_reted_torque, U32, uint32_t)
tool_pdo_set(Torque_slope, U32, uint32_t)
tool_pdo_set(Torque_profile_type, S16, int16_t)
tool_pdo_set(vl_target_velocity, S16, int16_t)
tool_pdo_set(VV_min_amount, U32, uint32_t)
tool_pdo_set(VV_max_amount, U32, uint32_t)
tool_pdo_set(VV_min_pos, U32, uint32_t)
tool_pdo_set(VV_max_pos, U32, uint32_t)
tool_pdo_set(VV_min_neg, U32, uint32_t)
tool_pdo_set(VV_max_neg, U32, uint32_t)
tool_pdo_set(VVA_Delta_speed, U32, uint32_t)
tool_pdo_set(VVA_Delta_time, U16, uint16_t)
tool_pdo_set(VVD_Delta_speed, U32, uint32_t)
tool_pdo_set(VVD_Delta_time, U16, uint16_t)
tool_pdo_set(VVQS_Delta_speed, U32, uint32_t)
tool_pdo_set(VVQS_Delta_time, U16, uint16_t)
tool_pdo_set(VSPF_numerator, U16, uint16_t)
tool_pdo_set(VSPF_denominator, U16, uint16_t)
tool_pdo_set(VDF_numerator, S32, int32_t)
tool_pdo_set(VDF_denominator, S32, int32_t)
tool_pdo_set(vl_pole_number, U8, uint8_t)
tool_pdo_set(vl_velocity_reference, U32, uint32_t)
tool_pdo_set(vl_ramp_function_time, U32, uint32_t)
tool_pdo_set(vl_slow_down_time, U32, uint32_t)
tool_pdo_set(vl_quick_stop_time, U32, uint32_t)
tool_pdo_set(vl_nominal_percentage, S16, int16_t)
tool_pdo_set(VVMA_min, U32, uint32_t)
tool_pdo_set(VVMA_max, U32, uint32_t)
tool_pdo_set(VVM_min_pos, U32, uint32_t)
tool_pdo_set(VVM_max_pos, U32, uint32_t)
tool_pdo_set(VVM_min_neg, U32, uint32_t)
tool_pdo_set(VVM_max_neg, U32, uint32_t)
tool_pdo_set(VFMA_min, U32, uint32_t)
tool_pdo_set(VFMA_max, U32, uint32_t)
tool_pdo_set(VFM_min_pos, U32, uint32_t)
tool_pdo_set(VFM_max_pos, U32, uint32_t)
tool_pdo_set(VFM_min_neg, U32, uint32_t)
tool_pdo_set(VFM_max_neg, U32, uint32_t)
tool_pdo_get(Abort_connection_option_code, S16, int16_t)
tool_pdo_get(Motor_type, U16, uint16_t)
tool_pdo_get(Motor_service_period, U32, uint32_t)
tool_pdo_get(Digital_inputs, U32, uint32_t)
tool_pdo_get(DO_Physical_outputs, U32, uint32_t)
tool_pdo_get(DO_Bitmask, U32, uint32_t)
tool_pdo_get(Controlword, U16, uint16_t)
tool_pdo_get(Modes_of_operation, S8, int8_t)
tool_pdo_get(Modes_of_operation_display, S8, int8_t)
tool_pdo_get(PRE_Encoder_increments, U32, uint32_t)
tool_pdo_get(PRE_Motor_revolutions, U32, uint32_t)
tool_pdo_get(VNR_Encoder_increments_per_second, U32, uint32_t)
tool_pdo_get(VNR_Motor_revolutions_per_second, U32, uint32_t)
tool_pdo_get(GR_Motor_revolutions, U32, uint32_t)
tool_pdo_get(GR_Shaft_revolutions, U32, uint32_t)
tool_pdo_get(FC_Feed, U32, uint32_t)
tool_pdo_get(FC_Shaft_revolutions, U32, uint32_t)
tool_pdo_get(PF_Numerator, U32, uint32_t)
tool_pdo_get(PF_Feed_constant, U32, uint32_t)
tool_pdo_get(VEF_Numerator, U32, uint32_t)
tool_pdo_get(VEF_Divisor, U32, uint32_t)
tool_pdo_get(VF1_Numerator, U32, uint32_t)
tool_pdo_get(VF1_Divisor, U32, uint32_t)
tool_pdo_get(VF2_Numerator, U32, uint32_t)
tool_pdo_get(VF2_Divisor, U32, uint32_t)
tool_pdo_get(AF_Numerator, U32, uint32_t)
tool_pdo_get(AF_Divisor, U32, uint32_t)
tool_pdo_get(Polarity, U8, uint8_t)
tool_pdo_get(Target_position, S32, int32_t)
tool_pdo_get(PRL_Min_position_range_limit, S32, int32_t)
tool_pdo_get(PRL_Max_position_range_limit, S32, int32_t)
tool_pdo_get(SPL_Min_position_limit, S32, int32_t)
tool_pdo_get(SPL_Max_position_limit, S32, int32_t)
tool_pdo_get(Max_profile_velocity, U32, uint32_t)
tool_pdo_get(Max_motor_speed, U32, uint32_t)
tool_pdo_get(Profile_velocity, U32, uint32_t)
tool_pdo_get(End_velocity, U32, uint32_t)
tool_pdo_get(Profile_acceleration, U32, uint32_t)
tool_pdo_get(Profile_deceleration, U32, uint32_t)
tool_pdo_get(Quick_stop_deceleration, U32, uint32_t)
tool_pdo_get(Motion_profile_type, S16, int16_t)
tool_pdo_get(Max_acceleration, U32, uint32_t)
tool_pdo_get(Max_deceleration, U32, uint32_t)
tool_pdo_get(Home_offset, U32, uint32_t)
tool_pdo_get(Home_method, S8, int8_t)
tool_pdo_get(HS_Speed_during_search_for_switch, U32, uint32_t)
tool_pdo_get(HS_Speed_during_search_for_zero, U32, uint32_t)
tool_pdo_get(Homing_acceleration, U32, uint32_t)
tool_pdo_get(Position_demand_value, S32, int32_t)
tool_pdo_get(Position_actual_value_inc, S32, int32_t)
tool_pdo_get(Position_actual_value, S32, int32_t)
tool_pdo_get(Following_error_window, U32, uint32_t)
tool_pdo_get(Following_error_time_out, U16, uint16_t)
tool_pdo_get(Position_window, U32, uint32_t)
tool_pdo_get(Position_window_time, U16, uint16_t)
tool_pdo_get(Interpolation_sub_mode_select, S16, int16_t)
tool_pdo_get(Interpolation_data_record_0, S16, int16_t)
tool_pdo_get(ITP_ip_time_units, U8, uint8_t)
tool_pdo_get(ITP_ip_time_index, S8, int8_t)
tool_pdo_get(ISD_Synchronize_on_group, U8, uint8_t)
tool_pdo_get(ISD_ip_sync_every_n_event, U8, uint8_t)
tool_pdo_get(IDC_Actual_buffer_size, U32, uint32_t)
tool_pdo_get(IDC_Buffer_organization, U8, uint8_t)
tool_pdo_get(IDC_Buffer_position, U16, uint16_t)
tool_pdo_get(Sensor_selection_code, S16, int16_t)
tool_pdo_get(Velocity_window, U16, uint16_t)
tool_pdo_get(Velocity_window_time, U16, uint16_t)
tool_pdo_get(Velocity_threshold, U16, uint16_t)
tool_pdo_get(Velocity_threshold_time, U16, uint16_t)
tool_pdo_get(Target_velocity, S32, int32_t)
tool_pdo_get(VCPS_Gain, U16, uint16_t)
tool_pdo_get(VCPS_integration_time_constant, U16, uint16_t)
tool_pdo_get(VCPS_Gain_1, U16, uint16_t)
tool_pdo_get(VCPS_integration_time_constant_1, U16, uint16_t)
tool_pdo_get(Target_torque, S16, int16_t)
tool_pdo_get(Max_torque, U16, uint16_t)
tool_pdo_get(Max_current, U16, uint16_t)
tool_pdo_get(Motor_rated_currect, U32, uint32_t)
tool_pdo_get(Motor_reted_torque, U32, uint32_t)
tool_pdo_get(Torque_slope, U32, uint32_t)
tool_pdo_get(Torque_profile_type, S16, int16_t)
tool_pdo_get(vl_target_velocity, S16, int16_t)
tool_pdo_get(VV_min_amount, U32, uint32_t)
tool_pdo_get(VV_max_amount, U32, uint32_t)
tool_pdo_get(VV_min_pos, U32, uint32_t)
tool_pdo_get(VV_max_pos, U32, uint32_t)
tool_pdo_get(VV_min_neg, U32, uint32_t)
tool_pdo_get(VV_max_neg, U32, uint32_t)
tool_pdo_get(VVA_Delta_speed, U32, uint32_t)
tool_pdo_get(VVA_Delta_time, U16, uint16_t)
tool_pdo_get(VVD_Delta_speed, U32, uint32_t)
tool_pdo_get(VVD_Delta_time, U16, uint16_t)
tool_pdo_get(VVQS_Delta_speed, U32, uint32_t)
tool_pdo_get(VVQS_Delta_time, U16, uint16_t)
tool_pdo_get(VSPF_numerator, U16, uint16_t)
tool_pdo_get(VSPF_denominator, U16, uint16_t)
tool_pdo_get(VDF_numerator, S32, int32_t)
tool_pdo_get(VDF_denominator, S32, int32_t)
tool_pdo_get(vl_pole_number, U8, uint8_t)
tool_pdo_get(vl_velocity_reference, U32, uint32_t)
tool_pdo_get(vl_ramp_function_time, U32, uint32_t)
tool_pdo_get(vl_slow_down_time, U32, uint32_t)
tool_pdo_get(vl_quick_stop_time, U32, uint32_t)
tool_pdo_get(vl_nominal_percentage, S16, int16_t)
tool_pdo_get(VVMA_min, U32, uint32_t)
tool_pdo_get(VVMA_max, U32, uint32_t)
tool_pdo_get(VVM_min_pos, U32, uint32_t)
tool_pdo_get(VVM_max_pos, U32, uint32_t)
tool_pdo_get(VVM_min_neg, U32, uint32_t)
tool_pdo_get(VVM_max_neg, U32, uint32_t)
tool_pdo_get(VFMA_min, U32, uint32_t)
tool_pdo_get(VFMA_max, U32, uint32_t)
tool_pdo_get(VFM_min_pos, U32, uint32_t)
tool_pdo_get(VFM_max_pos, U32, uint32_t)
tool_pdo_get(VFM_min_neg, U32, uint32_t)
tool_pdo_get(VFM_max_neg, U32, uint32_t)

static pdo_cmd_t pdo_cmd_list[]__attribute__((unused)) = { { "get_Error_code",
		tool_pdo_get_Error_code },
		{ "get_Statusword", tool_pdo_get_Statusword },
		{ "get_Supported_drive_modes", tool_pdo_get_Supported_drive_modes }, {
				"get_Following_error_actual_value",
				tool_pdo_get_Following_error_actual_value }, {
				"get_Control_effort", tool_pdo_get_Control_effort }, {
				"get_Position_demand_value_inc",
				tool_pdo_get_Position_demand_value_inc }, {
				"get_IDC_Maxinum_buffer_size",
				tool_pdo_get_IDC_Maxinum_buffer_size }, {
				"get_Velocity_sensor_actual_value",
				tool_pdo_get_Velocity_sensor_actual_value },
		{ "get_Velocity_demand_value", tool_pdo_get_Velocity_demand_value },
		{ "get_Velocity_actual_value", tool_pdo_get_Velocity_actual_value }, {
				"get_Max_slippage", tool_pdo_get_Max_slippage }, {
				"get_Torque_demand_value", tool_pdo_get_Torque_demand_value }, {
				"get_Torque_actual_value", tool_pdo_get_Torque_actual_value }, {
				"get_Current_actual_value", tool_pdo_get_Current_actual_value },
		{ "get_DC_link_circuit_voltage", tool_pdo_get_DC_link_circuit_voltage },
		{ "get_vl_velocity_demand", tool_pdo_get_vl_velocity_demand }, {
				"get_vl_control_effort", tool_pdo_get_vl_control_effort }, {
				"get_vl_manipulated_velocity",
				tool_pdo_get_vl_manipulated_velocity }, {
				"get_vl_percentage_demand", tool_pdo_get_vl_percentage_demand },
		{ "get_vl_actual_percentage", tool_pdo_get_vl_actual_percentage }, {
				"get_vl_manipulated_percentage",
				tool_pdo_get_vl_manipulated_percentage }, {
				"set_IDC_Size_of_data_record",
				tool_pdo_set_IDC_Size_of_data_record }, {
				"set_IDC_Buffer_clear", tool_pdo_set_IDC_Buffer_clear }, {
				"set_Abort_connection_option_code",
				tool_pdo_set_Abort_connection_option_code }, { "set_Motor_type",
				tool_pdo_set_Motor_type }, { "set_Motor_service_period",
				tool_pdo_set_Motor_service_period }, { "set_Digital_inputs",
				tool_pdo_set_Digital_inputs }, { "set_DO_Physical_outputs",
				tool_pdo_set_DO_Physical_outputs }, { "set_DO_Bitmask",
				tool_pdo_set_DO_Bitmask }, { "set_Controlword",
				tool_pdo_set_Controlword }, { "set_Modes_of_operation",
				tool_pdo_set_Modes_of_operation }, {
				"set_Modes_of_operation_display",
				tool_pdo_set_Modes_of_operation_display }, {
				"set_PRE_Encoder_increments",
				tool_pdo_set_PRE_Encoder_increments },
		{ "set_PRE_Motor_revolutions", tool_pdo_set_PRE_Motor_revolutions }, {
				"set_VNR_Encoder_increments_per_second",
				tool_pdo_set_VNR_Encoder_increments_per_second }, {
				"set_VNR_Motor_revolutions_per_second",
				tool_pdo_set_VNR_Motor_revolutions_per_second }, {
				"set_GR_Motor_revolutions", tool_pdo_set_GR_Motor_revolutions },
		{ "set_GR_Shaft_revolutions", tool_pdo_set_GR_Shaft_revolutions }, {
				"set_FC_Feed", tool_pdo_set_FC_Feed }, {
				"set_FC_Shaft_revolutions", tool_pdo_set_FC_Shaft_revolutions },
		{ "set_PF_Numerator", tool_pdo_set_PF_Numerator }, {
				"set_PF_Feed_constant", tool_pdo_set_PF_Feed_constant }, {
				"set_VEF_Numerator", tool_pdo_set_VEF_Numerator }, {
				"set_VEF_Divisor", tool_pdo_set_VEF_Divisor }, {
				"set_VF1_Numerator", tool_pdo_set_VF1_Numerator }, {
				"set_VF1_Divisor", tool_pdo_set_VF1_Divisor }, {
				"set_VF2_Numerator", tool_pdo_set_VF2_Numerator }, {
				"set_VF2_Divisor", tool_pdo_set_VF2_Divisor }, {
				"set_AF_Numerator", tool_pdo_set_AF_Numerator }, {
				"set_AF_Divisor", tool_pdo_set_AF_Divisor }, { "set_Polarity",
				tool_pdo_set_Polarity }, { "set_Target_position",
				tool_pdo_set_Target_position }, {
				"set_PRL_Min_position_range_limit",
				tool_pdo_set_PRL_Min_position_range_limit }, {
				"set_PRL_Max_position_range_limit",
				tool_pdo_set_PRL_Max_position_range_limit }, {
				"set_SPL_Min_position_limit",
				tool_pdo_set_SPL_Min_position_limit }, {
				"set_SPL_Max_position_limit",
				tool_pdo_set_SPL_Max_position_limit }, {
				"set_Max_profile_velocity", tool_pdo_set_Max_profile_velocity },
		{ "set_Max_motor_speed", tool_pdo_set_Max_motor_speed }, {
				"set_Profile_velocity", tool_pdo_set_Profile_velocity }, {
				"set_End_velocity", tool_pdo_set_End_velocity }, {
				"set_Profile_acceleration", tool_pdo_set_Profile_acceleration },
		{ "set_Profile_deceleration", tool_pdo_set_Profile_deceleration }, {
				"set_Quick_stop_deceleration",
				tool_pdo_set_Quick_stop_deceleration }, {
				"set_Motion_profile_type", tool_pdo_set_Motion_profile_type }, {
				"set_Max_acceleration", tool_pdo_set_Max_acceleration }, {
				"set_Max_deceleration", tool_pdo_set_Max_deceleration }, {
				"set_Home_offset", tool_pdo_set_Home_offset }, {
				"set_Home_method", tool_pdo_set_Home_method }, {
				"set_HS_Speed_during_search_for_switch",
				tool_pdo_set_HS_Speed_during_search_for_switch }, {
				"set_HS_Speed_during_search_for_zero",
				tool_pdo_set_HS_Speed_during_search_for_zero }, {
				"set_Homing_acceleration", tool_pdo_set_Homing_acceleration },
		{ "set_Position_demand_value", tool_pdo_set_Position_demand_value }, {
				"set_Position_actual_value_inc",
				tool_pdo_set_Position_actual_value_inc },
		{ "set_Position_actual_value", tool_pdo_set_Position_actual_value }, {
				"set_Following_error_window",
				tool_pdo_set_Following_error_window }, {
				"set_Following_error_time_out",
				tool_pdo_set_Following_error_time_out }, {
				"set_Position_window", tool_pdo_set_Position_window }, {
				"set_Position_window_time", tool_pdo_set_Position_window_time },
		{ "set_Interpolation_sub_mode_select",
				tool_pdo_set_Interpolation_sub_mode_select }, {
				"set_Interpolation_data_record_0",
				tool_pdo_set_Interpolation_data_record_0 }, {
				"set_ITP_ip_time_units", tool_pdo_set_ITP_ip_time_units }, {
				"set_ITP_ip_time_index", tool_pdo_set_ITP_ip_time_index }, {
				"set_ISD_Synchronize_on_group",
				tool_pdo_set_ISD_Synchronize_on_group }, {
				"set_ISD_ip_sync_every_n_event",
				tool_pdo_set_ISD_ip_sync_every_n_event }, {
				"set_IDC_Actual_buffer_size",
				tool_pdo_set_IDC_Actual_buffer_size }, {
				"set_IDC_Buffer_organization",
				tool_pdo_set_IDC_Buffer_organization }, {
				"set_IDC_Buffer_position", tool_pdo_set_IDC_Buffer_position },
		{ "set_Sensor_selection_code", tool_pdo_set_Sensor_selection_code }, {
				"set_Velocity_window", tool_pdo_set_Velocity_window }, {
				"set_Velocity_window_time", tool_pdo_set_Velocity_window_time },
		{ "set_Velocity_threshold", tool_pdo_set_Velocity_threshold }, {
				"set_Velocity_threshold_time",
				tool_pdo_set_Velocity_threshold_time }, { "set_Target_velocity",
				tool_pdo_set_Target_velocity }, { "set_VCPS_Gain",
				tool_pdo_set_VCPS_Gain }, {
				"set_VCPS_integration_time_constant",
				tool_pdo_set_VCPS_integration_time_constant }, {
				"set_VCPS_Gain_1", tool_pdo_set_VCPS_Gain_1 }, {
				"set_VCPS_integration_time_constant_1",
				tool_pdo_set_VCPS_integration_time_constant_1 }, {
				"set_Target_torque", tool_pdo_set_Target_torque }, {
				"set_Max_torque", tool_pdo_set_Max_torque }, {
				"set_Max_current", tool_pdo_set_Max_current }, {
				"set_Motor_rated_currect", tool_pdo_set_Motor_rated_currect }, {
				"set_Motor_reted_torque", tool_pdo_set_Motor_reted_torque }, {
				"set_Torque_slope", tool_pdo_set_Torque_slope }, {
				"set_Torque_profile_type", tool_pdo_set_Torque_profile_type }, {
				"set_vl_target_velocity", tool_pdo_set_vl_target_velocity }, {
				"set_VV_min_amount", tool_pdo_set_VV_min_amount }, {
				"set_VV_max_amount", tool_pdo_set_VV_max_amount }, {
				"set_VV_min_pos", tool_pdo_set_VV_min_pos }, { "set_VV_max_pos",
				tool_pdo_set_VV_max_pos }, { "set_VV_min_neg",
				tool_pdo_set_VV_min_neg }, { "set_VV_max_neg",
				tool_pdo_set_VV_max_neg }, { "set_VVA_Delta_speed",
				tool_pdo_set_VVA_Delta_speed }, { "set_VVA_Delta_time",
				tool_pdo_set_VVA_Delta_time }, { "set_VVD_Delta_speed",
				tool_pdo_set_VVD_Delta_speed }, { "set_VVD_Delta_time",
				tool_pdo_set_VVD_Delta_time }, { "set_VVQS_Delta_speed",
				tool_pdo_set_VVQS_Delta_speed }, { "set_VVQS_Delta_time",
				tool_pdo_set_VVQS_Delta_time }, { "set_VSPF_numerator",
				tool_pdo_set_VSPF_numerator }, { "set_VSPF_denominator",
				tool_pdo_set_VSPF_denominator }, { "set_VDF_numerator",
				tool_pdo_set_VDF_numerator }, { "set_VDF_denominator",
				tool_pdo_set_VDF_denominator }, { "set_vl_pole_number",
				tool_pdo_set_vl_pole_number }, { "set_vl_velocity_reference",
				tool_pdo_set_vl_velocity_reference },
		{ "set_vl_ramp_function_time", tool_pdo_set_vl_ramp_function_time }, {
				"set_vl_slow_down_time", tool_pdo_set_vl_slow_down_time }, {
				"set_vl_quick_stop_time", tool_pdo_set_vl_quick_stop_time },
		{ "set_vl_nominal_percentage", tool_pdo_set_vl_nominal_percentage }, {
				"set_VVMA_min", tool_pdo_set_VVMA_min }, { "set_VVMA_max",
				tool_pdo_set_VVMA_max }, { "set_VVM_min_pos",
				tool_pdo_set_VVM_min_pos }, { "set_VVM_max_pos",
				tool_pdo_set_VVM_max_pos }, { "set_VVM_min_neg",
				tool_pdo_set_VVM_min_neg }, { "set_VVM_max_neg",
				tool_pdo_set_VVM_max_neg }, { "set_VFMA_min",
				tool_pdo_set_VFMA_min },
		{ "set_VFMA_max", tool_pdo_set_VFMA_max }, { "set_VFM_min_pos",
				tool_pdo_set_VFM_min_pos }, { "set_VFM_max_pos",
				tool_pdo_set_VFM_max_pos }, { "set_VFM_min_neg",
				tool_pdo_set_VFM_min_neg }, { "set_VFM_max_neg",
				tool_pdo_set_VFM_max_neg }, {
				"get_Abort_connection_option_code",
				tool_pdo_get_Abort_connection_option_code }, { "get_Motor_type",
				tool_pdo_get_Motor_type }, { "get_Motor_service_period",
				tool_pdo_get_Motor_service_period }, { "get_Digital_inputs",
				tool_pdo_get_Digital_inputs }, { "get_DO_Physical_outputs",
				tool_pdo_get_DO_Physical_outputs }, { "get_DO_Bitmask",
				tool_pdo_get_DO_Bitmask }, { "get_Controlword",
				tool_pdo_get_Controlword }, { "get_Modes_of_operation",
				tool_pdo_get_Modes_of_operation }, {
				"get_Modes_of_operation_display",
				tool_pdo_get_Modes_of_operation_display }, {
				"get_PRE_Encoder_increments",
				tool_pdo_get_PRE_Encoder_increments },
		{ "get_PRE_Motor_revolutions", tool_pdo_get_PRE_Motor_revolutions }, {
				"get_VNR_Encoder_increments_per_second",
				tool_pdo_get_VNR_Encoder_increments_per_second }, {
				"get_VNR_Motor_revolutions_per_second",
				tool_pdo_get_VNR_Motor_revolutions_per_second }, {
				"get_GR_Motor_revolutions", tool_pdo_get_GR_Motor_revolutions },
		{ "get_GR_Shaft_revolutions", tool_pdo_get_GR_Shaft_revolutions }, {
				"get_FC_Feed", tool_pdo_get_FC_Feed }, {
				"get_FC_Shaft_revolutions", tool_pdo_get_FC_Shaft_revolutions },
		{ "get_PF_Numerator", tool_pdo_get_PF_Numerator }, {
				"get_PF_Feed_constant", tool_pdo_get_PF_Feed_constant }, {
				"get_VEF_Numerator", tool_pdo_get_VEF_Numerator }, {
				"get_VEF_Divisor", tool_pdo_get_VEF_Divisor }, {
				"get_VF1_Numerator", tool_pdo_get_VF1_Numerator }, {
				"get_VF1_Divisor", tool_pdo_get_VF1_Divisor }, {
				"get_VF2_Numerator", tool_pdo_get_VF2_Numerator }, {
				"get_VF2_Divisor", tool_pdo_get_VF2_Divisor }, {
				"get_AF_Numerator", tool_pdo_get_AF_Numerator }, {
				"get_AF_Divisor", tool_pdo_get_AF_Divisor }, { "get_Polarity",
				tool_pdo_get_Polarity }, { "get_Target_position",
				tool_pdo_get_Target_position }, {
				"get_PRL_Min_position_range_limit",
				tool_pdo_get_PRL_Min_position_range_limit }, {
				"get_PRL_Max_position_range_limit",
				tool_pdo_get_PRL_Max_position_range_limit }, {
				"get_SPL_Min_position_limit",
				tool_pdo_get_SPL_Min_position_limit }, {
				"get_SPL_Max_position_limit",
				tool_pdo_get_SPL_Max_position_limit }, {
				"get_Max_profile_velocity", tool_pdo_get_Max_profile_velocity },
		{ "get_Max_motor_speed", tool_pdo_get_Max_motor_speed }, {
				"get_Profile_velocity", tool_pdo_get_Profile_velocity }, {
				"get_End_velocity", tool_pdo_get_End_velocity }, {
				"get_Profile_acceleration", tool_pdo_get_Profile_acceleration },
		{ "get_Profile_deceleration", tool_pdo_get_Profile_deceleration }, {
				"get_Quick_stop_deceleration",
				tool_pdo_get_Quick_stop_deceleration }, {
				"get_Motion_profile_type", tool_pdo_get_Motion_profile_type }, {
				"get_Max_acceleration", tool_pdo_get_Max_acceleration }, {
				"get_Max_deceleration", tool_pdo_get_Max_deceleration }, {
				"get_Home_offset", tool_pdo_get_Home_offset }, {
				"get_Home_method", tool_pdo_get_Home_method }, {
				"get_HS_Speed_during_search_for_switch",
				tool_pdo_get_HS_Speed_during_search_for_switch }, {
				"get_HS_Speed_during_search_for_zero",
				tool_pdo_get_HS_Speed_during_search_for_zero }, {
				"get_Homing_acceleration", tool_pdo_get_Homing_acceleration },
		{ "get_Position_demand_value", tool_pdo_get_Position_demand_value }, {
				"get_Position_actual_value_inc",
				tool_pdo_get_Position_actual_value_inc },
		{ "get_Position_actual_value", tool_pdo_get_Position_actual_value }, {
				"get_Following_error_window",
				tool_pdo_get_Following_error_window }, {
				"get_Following_error_time_out",
				tool_pdo_get_Following_error_time_out }, {
				"get_Position_window", tool_pdo_get_Position_window }, {
				"get_Position_window_time", tool_pdo_get_Position_window_time },
		{ "get_Interpolation_sub_mode_select",
				tool_pdo_get_Interpolation_sub_mode_select }, {
				"get_Interpolation_data_record_0",
				tool_pdo_get_Interpolation_data_record_0 }, {
				"get_ITP_ip_time_units", tool_pdo_get_ITP_ip_time_units }, {
				"get_ITP_ip_time_index", tool_pdo_get_ITP_ip_time_index }, {
				"get_ISD_Synchronize_on_group",
				tool_pdo_get_ISD_Synchronize_on_group }, {
				"get_ISD_ip_sync_every_n_event",
				tool_pdo_get_ISD_ip_sync_every_n_event }, {
				"get_IDC_Actual_buffer_size",
				tool_pdo_get_IDC_Actual_buffer_size }, {
				"get_IDC_Buffer_organization",
				tool_pdo_get_IDC_Buffer_organization }, {
				"get_IDC_Buffer_position", tool_pdo_get_IDC_Buffer_position },
		{ "get_Sensor_selection_code", tool_pdo_get_Sensor_selection_code }, {
				"get_Velocity_window", tool_pdo_get_Velocity_window }, {
				"get_Velocity_window_time", tool_pdo_get_Velocity_window_time },
		{ "get_Velocity_threshold", tool_pdo_get_Velocity_threshold }, {
				"get_Velocity_threshold_time",
				tool_pdo_get_Velocity_threshold_time }, { "get_Target_velocity",
				tool_pdo_get_Target_velocity }, { "get_VCPS_Gain",
				tool_pdo_get_VCPS_Gain }, {
				"get_VCPS_integration_time_constant",
				tool_pdo_get_VCPS_integration_time_constant }, {
				"get_VCPS_Gain_1", tool_pdo_get_VCPS_Gain_1 }, {
				"get_VCPS_integration_time_constant_1",
				tool_pdo_get_VCPS_integration_time_constant_1 }, {
				"get_Target_torque", tool_pdo_get_Target_torque }, {
				"get_Max_torque", tool_pdo_get_Max_torque }, {
				"get_Max_current", tool_pdo_get_Max_current }, {
				"get_Motor_rated_currect", tool_pdo_get_Motor_rated_currect }, {
				"get_Motor_reted_torque", tool_pdo_get_Motor_reted_torque }, {
				"get_Torque_slope", tool_pdo_get_Torque_slope }, {
				"get_Torque_profile_type", tool_pdo_get_Torque_profile_type }, {
				"get_vl_target_velocity", tool_pdo_get_vl_target_velocity }, {
				"get_VV_min_amount", tool_pdo_get_VV_min_amount }, {
				"get_VV_max_amount", tool_pdo_get_VV_max_amount }, {
				"get_VV_min_pos", tool_pdo_get_VV_min_pos }, { "get_VV_max_pos",
				tool_pdo_get_VV_max_pos }, { "get_VV_min_neg",
				tool_pdo_get_VV_min_neg }, { "get_VV_max_neg",
				tool_pdo_get_VV_max_neg }, { "get_VVA_Delta_speed",
				tool_pdo_get_VVA_Delta_speed }, { "get_VVA_Delta_time",
				tool_pdo_get_VVA_Delta_time }, { "get_VVD_Delta_speed",
				tool_pdo_get_VVD_Delta_speed }, { "get_VVD_Delta_time",
				tool_pdo_get_VVD_Delta_time }, { "get_VVQS_Delta_speed",
				tool_pdo_get_VVQS_Delta_speed }, { "get_VVQS_Delta_time",
				tool_pdo_get_VVQS_Delta_time }, { "get_VSPF_numerator",
				tool_pdo_get_VSPF_numerator }, { "get_VSPF_denominator",
				tool_pdo_get_VSPF_denominator }, { "get_VDF_numerator",
				tool_pdo_get_VDF_numerator }, { "get_VDF_denominator",
				tool_pdo_get_VDF_denominator }, { "get_vl_pole_number",
				tool_pdo_get_vl_pole_number }, { "get_vl_velocity_reference",
				tool_pdo_get_vl_velocity_reference },
		{ "get_vl_ramp_function_time", tool_pdo_get_vl_ramp_function_time }, {
				"get_vl_slow_down_time", tool_pdo_get_vl_slow_down_time }, {
				"get_vl_quick_stop_time", tool_pdo_get_vl_quick_stop_time },
		{ "get_vl_nominal_percentage", tool_pdo_get_vl_nominal_percentage }, {
				"get_VVMA_min", tool_pdo_get_VVMA_min }, { "get_VVMA_max",
				tool_pdo_get_VVMA_max }, { "get_VVM_min_pos",
				tool_pdo_get_VVM_min_pos }, { "get_VVM_max_pos",
				tool_pdo_get_VVM_max_pos }, { "get_VVM_min_neg",
				tool_pdo_get_VVM_min_neg }, { "get_VVM_max_neg",
				tool_pdo_get_VVM_max_neg }, { "get_VFMA_min",
				tool_pdo_get_VFMA_min },
		{ "get_VFMA_max", tool_pdo_get_VFMA_max }, { "get_VFM_min_pos",
				tool_pdo_get_VFM_min_pos }, { "get_VFM_max_pos",
				tool_pdo_get_VFM_max_pos }, { "get_VFM_min_neg",
				tool_pdo_get_VFM_min_neg }, { "get_VFM_max_neg",
				tool_pdo_get_VFM_max_neg } };

#define tool_sdo_set(name, type) \
	static inline uint32_t tool_sdo_set_##name(nser_axle *ns_axle, void *v, int *s) \
	{\
		*s = sizeof(type);\
		return set_sdo(name, ns_axle, v, sizeof(type)); \
	}

#define tool_sdo_get(name, type) \
	static inline uint32_t tool_sdo_get_##name(nser_axle *ns_axle, void *v, int *s) \
	{\
		*s = sizeof(type);\
		return get_sdo(name, ns_axle, v, sizeof(type), NULL); \
	}

tool_sdo_set(Abort_connection_option_code, int16_t)
tool_sdo_get(Abort_connection_option_code, int16_t)
tool_sdo_get(Error_code, int16_t)
tool_sdo_get(Motor_type, uint16_t)
tool_sdo_set(Motor_service_period, uint32_t)
tool_sdo_get(Motor_service_period, uint32_t)
tool_sdo_set(Supported_drive_modes, uint32_t)
tool_sdo_get(Supported_drive_modes, uint32_t)
tool_sdo_set(Digital_inputs, uint32_t)
tool_sdo_get(Digital_inputs, uint32_t)
tool_sdo_set(DO_Physical_outputs, uint32_t)
tool_sdo_get(DO_Physical_outputs, uint32_t)
tool_sdo_set(DO_Bitmask, uint32_t)
tool_sdo_get(DO_Bitmask, uint32_t)
tool_sdo_set(Position_notation_index, int8_t)
tool_sdo_get(Position_notation_index, int8_t)
tool_sdo_set(Position_dimension_index, uint8_t)
tool_sdo_get(Position_dimension_index, uint8_t)
tool_sdo_set(Velocity_notation_index, int8_t)
tool_sdo_get(Velocity_notation_index, int8_t)
tool_sdo_set(Velocity_dimension_index, uint8_t)
tool_sdo_get(Velocity_dimension_index, uint8_t)
tool_sdo_set(Acceleration_notation_index, int8_t)
tool_sdo_get(Acceleration_notation_index, int8_t)
tool_sdo_set(Acceleration_dimension_index, uint8_t)
tool_sdo_get(Acceleration_dimension_index, uint8_t)
tool_sdo_set(PRE_Encoder_increments, uint32_t)
tool_sdo_get(PRE_Encoder_increments, uint32_t)
tool_sdo_set(PRE_Motor_revolutions, uint32_t)
tool_sdo_get(PRE_Motor_revolutions, uint32_t)
tool_sdo_set(VNR_Encoder_increments_per_second, uint32_t)
tool_sdo_get(VNR_Encoder_increments_per_second, uint32_t)
tool_sdo_set(VNR_Motor_revolutions_per_second, uint32_t)
tool_sdo_get(VNR_Motor_revolutions_per_second, uint32_t)
tool_sdo_set(GR_Motor_revolutions, uint32_t)
tool_sdo_get(GR_Motor_revolutions, uint32_t)
tool_sdo_set(GR_Shaft_revolutions, uint32_t)
tool_sdo_get(GR_Shaft_revolutions, uint32_t)
tool_sdo_get(FC_Feed, uint32_t)
tool_sdo_set(FC_Feed, uint32_t)
tool_sdo_get(FC_Shaft_revolutions, uint32_t)
tool_sdo_set(FC_Shaft_revolutions, uint32_t)
tool_sdo_get(PF_Numerator, uint32_t)
tool_sdo_set(PF_Numerator, uint32_t)
tool_sdo_get(PF_Feed_constant, uint32_t)
tool_sdo_set(PF_Feed_constant, uint32_t)
tool_sdo_set(VEF_Numerator, uint32_t)
tool_sdo_get(VEF_Numerator, uint32_t)
tool_sdo_set(VEF_Divisor, uint32_t)
tool_sdo_get(VEF_Divisor, uint32_t)
tool_sdo_set(VF1_Numerator, uint32_t)
tool_sdo_get(VF1_Numerator, uint32_t)
tool_sdo_set(VF1_Divisor, uint32_t)
tool_sdo_get(VF1_Divisor, uint32_t)
tool_sdo_set(VF2_Numerator, uint32_t)
tool_sdo_get(VF2_Numerator, uint32_t)
tool_sdo_set(VF2_Divisor, uint32_t)
tool_sdo_get(VF2_Divisor, uint32_t)
tool_sdo_set(AF_Numerator, uint32_t)
tool_sdo_get(AF_Numerator, uint32_t)
tool_sdo_set(AF_Divisor, uint32_t)
tool_sdo_get(AF_Divisor, uint32_t)
tool_sdo_set(Polarity, uint8_t)
tool_sdo_get(Polarity, uint8_t)
tool_sdo_set(Controlword, uint16_t)
tool_sdo_set(Shutdown_option_code, uint16_t)
tool_sdo_set(Disable_operation_option_code, uint16_t)
tool_sdo_set(Quick_stop_option_code, uint16_t)
tool_sdo_set(Halt_option_code, uint16_t)
tool_sdo_set(Fault_reaction_option_code, uint16_t)
tool_sdo_set(Modes_of_operation, int16_t)
tool_sdo_get(Controlword, uint16_t)
tool_sdo_get(Statusword, uint16_t)
tool_sdo_get(Shutdown_option_code, uint16_t)
tool_sdo_get(Disable_operation_option_code, uint16_t)
tool_sdo_get(Quick_stop_option_code, uint16_t)
tool_sdo_get(Halt_option_code, uint16_t)
tool_sdo_get(Fault_reaction_option_code, uint16_t)
tool_sdo_get(Modes_of_operation, int16_t)
tool_sdo_get(Modes_of_operation_display, int16_t)
tool_sdo_set(Target_position, int32_t)
tool_sdo_set(PRL_Min_position_range_limit, int32_t)
// PRL :  Position range limit
tool_sdo_set(PRL_Max_position_range_limit, int32_t)
tool_sdo_set(SPL_Min_position_limit, int32_t)
// SPL : software position limit
tool_sdo_set(SPL_Max_position_limit, int32_t)
tool_sdo_set(Max_profile_velocity, uint32_t)
tool_sdo_set(Max_motor_speed, uint32_t)
tool_sdo_set(Profile_velocity, uint32_t)
tool_sdo_set(End_velocity, uint32_t)
tool_sdo_set(Profile_acceleration, uint32_t)
tool_sdo_set(Profile_deceleration, uint32_t)
tool_sdo_set(Quick_stop_deceleration, uint32_t)
tool_sdo_set(Motion_profile_type, int16_t)
tool_sdo_set(Max_acceleration, uint32_t)
tool_sdo_set(Max_deceleration, uint32_t)
tool_sdo_get(Target_position, int32_t)
tool_sdo_get(PRL_Min_position_range_limit, int32_t)
// PRL :  Position range limit
tool_sdo_get(PRL_Max_position_range_limit, int32_t)
tool_sdo_get(SPL_Min_position_limit, int32_t)
// SPL : software position limit
tool_sdo_get(SPL_Max_position_limit, int32_t)
tool_sdo_get(Max_profile_velocity, uint32_t)
tool_sdo_get(Max_motor_speed, uint32_t)
tool_sdo_get(Profile_velocity, uint32_t)
tool_sdo_get(End_velocity, uint32_t)
tool_sdo_get(Profile_acceleration, uint32_t)
tool_sdo_get(Profile_deceleration, uint32_t)
tool_sdo_get(Quick_stop_deceleration, uint32_t)
tool_sdo_get(Motion_profile_type, int16_t)
tool_sdo_get(Max_acceleration, uint32_t)
tool_sdo_get(Max_deceleration, uint32_t)
tool_sdo_set(Home_offset, int32_t)
tool_sdo_set(Home_method, int8_t)
tool_sdo_set(HS_Speed_during_search_for_switch, uint32_t)
tool_sdo_set(HS_Speed_during_search_for_zero, uint32_t)
tool_sdo_set(Homing_acceleration, uint32_t)
tool_sdo_get(Home_offset, int32_t)
tool_sdo_get(Home_method, int8_t)
tool_sdo_get(HS_Speed_during_search_for_switch, uint32_t)
tool_sdo_get(HS_Speed_during_search_for_zero, uint32_t)
tool_sdo_get(Homing_acceleration, uint32_t)
tool_sdo_set(Following_error_window, uint32_t)
tool_sdo_set(Following_error_time_out, uint16_t)
tool_sdo_set(Position_window, uint32_t)
tool_sdo_set(Position_window_time, uint16_t)
tool_sdo_get(Position_demand_value, int32_t)
tool_sdo_get(Position_actual_value_inc, int32_t)
tool_sdo_get(Position_actual_value, int32_t)
tool_sdo_get(Following_error_window, uint32_t)
tool_sdo_get(Following_error_time_out, uint16_t)
tool_sdo_get(Position_window, uint32_t)
tool_sdo_get(Position_window_time, uint16_t)
tool_sdo_get(Following_error_actual_value, int32_t)
tool_sdo_get(Control_effort, int32_t)
tool_sdo_get(Position_demand_value_inc, int32_t)
tool_sdo_set(Interpolation_sub_mode_select, int16_t)
tool_sdo_set(ITP_ip_time_units, uint8_t)
// ITP : Interpolation time period
tool_sdo_set(ITP_ip_time_index, int8_t)
tool_sdo_set(ISD_Synchronize_on_group, uint8_t)
// ISD : Interpolation sync definition
tool_sdo_set(ISD_ip_sync_every_n_event, uint8_t)
tool_sdo_set(IDC_Actual_buffer_size, uint32_t)
tool_sdo_set(IDC_Buffer_organization, uint32_t)
tool_sdo_set(IDC_Buffer_position, uint32_t)
tool_sdo_set(IDC_Size_of_data_record, uint32_t)
tool_sdo_set(IDC_Buffer_clear, uint32_t)
tool_sdo_get(Interpolation_sub_mode_select, int16_t)
tool_sdo_get(ITP_ip_time_units, uint8_t)
// ITP : Interpolation time period
tool_sdo_get(ITP_ip_time_index, int8_t)
tool_sdo_get(ISD_Synchronize_on_group, uint8_t)
// ISD : Interpolation sync definition
tool_sdo_get(ISD_ip_sync_every_n_event, uint8_t)
tool_sdo_get(IDC_Maxinum_buffer_size, uint32_t)
// IDC : Interpolation data configuration
tool_sdo_get(IDC_Actual_buffer_size, uint32_t)
tool_sdo_get(IDC_Buffer_organization, uint32_t)
tool_sdo_get(IDC_Buffer_position, uint32_t)
tool_sdo_get(IDC_Size_of_data_record, uint32_t)
tool_sdo_get(IDC_Buffer_clear, uint32_t)
tool_sdo_set(Velocity_sensor_actual_value, int32_t)
tool_sdo_set(Sensor_selection_code, int16_t)
tool_sdo_set(Velocity_demand_value, int32_t)
tool_sdo_set(Velocity_actual_value, int32_t)
tool_sdo_set(Velocity_window, uint16_t)
tool_sdo_set(Velocity_window_time, uint16_t)
tool_sdo_set(Velocity_threshold, uint16_t)
tool_sdo_set(Velocity_threshold_time, uint16_t)
tool_sdo_set(Target_velocity, int32_t)
tool_sdo_set(Max_slippage, int32_t)
tool_sdo_set(VCPS_Gain, uint16_t)
// VCPS : Velocity control parameter set
tool_sdo_set(VCPS_integration_time_constant, uint16_t)
tool_sdo_get(Velocity_sensor_actual_value, int32_t)
tool_sdo_get(Sensor_selection_code, int16_t)
tool_sdo_get(Velocity_demand_value, int32_t)
tool_sdo_get(Velocity_actual_value, int32_t)
tool_sdo_get(Velocity_window, uint16_t)
tool_sdo_get(Velocity_window_time, uint16_t)
tool_sdo_get(Velocity_threshold, uint16_t)
tool_sdo_get(Velocity_threshold_time, uint16_t)
tool_sdo_get(Target_velocity, int32_t)
tool_sdo_get(Max_slippage, int32_t)
tool_sdo_get(VCPS_Gain, uint16_t)
// VCPS : Velocity control parameter set
tool_sdo_get(VCPS_integration_time_constant, uint16_t)
tool_sdo_set(Target_torque, int16_t)
tool_sdo_set(Max_torque, uint16_t)
tool_sdo_set(Max_current, uint16_t)
tool_sdo_set(Motor_rated_currect, uint32_t)
tool_sdo_set(Motor_reted_torque, uint32_t)
tool_sdo_set(Torque_slope, uint32_t)
tool_sdo_set(Torque_profile_type, int16_t)
tool_sdo_get(Target_torque, int16_t)
tool_sdo_get(Max_torque, uint16_t)
tool_sdo_get(Max_current, uint16_t)
tool_sdo_get(Torque_demand_value, int16_t)
tool_sdo_get(Motor_rated_currect, uint32_t)
tool_sdo_get(Motor_reted_torque, uint32_t)
tool_sdo_get(Torque_actual_value, int16_t)
tool_sdo_get(Current_actual_value, int16_t)
tool_sdo_get(DC_link_circuit_voltage, uint32_t)
tool_sdo_get(Torque_slope, uint32_t)
tool_sdo_get(Torque_profile_type, int16_t)
tool_sdo_set(vl_target_velocity, int16_t)
tool_sdo_set(vl_velocity_demand, int16_t)
tool_sdo_set(vl_control_effort, int16_t)
tool_sdo_set(vl_manipulated_velocity, int16_t)
tool_sdo_set(VV_min_amount, uint32_t)
//VV : Vl velocity
tool_sdo_set(VV_max_amount, uint32_t)
tool_sdo_set(VV_min_pos, uint32_t)
tool_sdo_set(VV_max_pos, uint32_t)
tool_sdo_set(VV_min_neg, uint32_t)
tool_sdo_set(VV_max_neg, uint32_t)
tool_sdo_set(VVA_Delta_speed, uint32_t)
// VVA : vl velocity acceleration
tool_sdo_set(VVA_Delta_time, uint16_t)
tool_sdo_set(VVD_Delta_speed, uint32_t)
// VVD : vl velocity deceleration
tool_sdo_set(VVD_Delta_time, uint16_t)
tool_sdo_set(VVQS_Delta_speed, uint32_t)
// VVQS : vl velocity quick stop
tool_sdo_set(VVQS_Delta_time, uint16_t)
tool_sdo_set(VSPF_numerator, int16_t)
// VSPF : vl set-point factor
tool_sdo_set(VSPF_denominator, int16_t)
tool_sdo_set(VDF_numerator, int32_t)
// VDF : vl dimension factor
tool_sdo_set(VDF_denominator, int32_t)
tool_sdo_set(vl_pole_number, uint8_t)
tool_sdo_set(vl_velocity_reference, uint32_t)
tool_sdo_set(vl_ramp_function_time, uint32_t)
tool_sdo_set(vl_slow_down_time, uint32_t)
tool_sdo_set(vl_quick_stop_time, uint32_t)
tool_sdo_set(vl_nominal_percentage, int16_t)
tool_sdo_set(vl_percentage_demand, int16_t)
tool_sdo_set(vl_actual_percentage, int16_t)
tool_sdo_set(vl_manipulated_percentage, int16_t)
tool_sdo_set(VVMA_min, uint32_t)
// VVMA : vl velocity motor amount
tool_sdo_set(VVMA_max, uint32_t)
tool_sdo_set(VVM_min_pos, uint32_t)
// VVM	: vl velocity motor
tool_sdo_set(VVM_max_pos, uint32_t)
tool_sdo_set(VVM_min_neg, uint32_t)
tool_sdo_set(VVM_max_neg, uint32_t)
tool_sdo_set(VFMA_min, uint32_t)
// VFMA : vl frequency motor amount
tool_sdo_set(VFMA_max, uint32_t)
tool_sdo_set(VFM_min_pos, uint32_t)
// VFM	: vl frequency motor
tool_sdo_set(VFM_max_pos, uint32_t)
tool_sdo_set(VFM_min_neg, uint32_t)
tool_sdo_set(VFM_max_neg, uint32_t)

static sdo_cmd_t sdo_cmd_list[]__attribute__((unused)) = { {
		"sdo_set_Abort_connection_option_code",
		tool_sdo_set_Abort_connection_option_code }, {
		"sdo_get_Abort_connection_option_code",
		tool_sdo_get_Abort_connection_option_code }, { "sdo_get_Error_code",
		tool_sdo_get_Error_code }, { "sdo_get_Motor_type",
		tool_sdo_get_Motor_type }, { "sdo_set_Motor_service_period",
		tool_sdo_set_Motor_service_period }, { "sdo_get_Motor_service_period",
		tool_sdo_get_Motor_service_period }, { "sdo_set_Supported_drive_modes",
		tool_sdo_set_Supported_drive_modes }, { "sdo_get_Supported_drive_modes",
		tool_sdo_get_Supported_drive_modes }, { "sdo_set_Digital_inputs",
		tool_sdo_set_Digital_inputs }, { "sdo_get_Digital_inputs",
		tool_sdo_get_Digital_inputs }, { "sdo_set_DO_Physical_outputs",
		tool_sdo_set_DO_Physical_outputs }, { "sdo_get_DO_Physical_outputs",
		tool_sdo_get_DO_Physical_outputs }, { "sdo_set_DO_Bitmask",
		tool_sdo_set_DO_Bitmask }, { "sdo_get_DO_Bitmask",
		tool_sdo_get_DO_Bitmask }, { "sdo_set_Position_notation_index",
		tool_sdo_set_Position_notation_index },
		{ "sdo_get_Position_notation_index",
				tool_sdo_get_Position_notation_index }, {
				"sdo_set_Position_dimension_index",
				tool_sdo_set_Position_dimension_index }, {
				"sdo_get_Position_dimension_index",
				tool_sdo_get_Position_dimension_index }, {
				"sdo_set_Velocity_notation_index",
				tool_sdo_set_Velocity_notation_index }, {
				"sdo_get_Velocity_notation_index",
				tool_sdo_get_Velocity_notation_index }, {
				"sdo_set_Velocity_dimension_index",
				tool_sdo_set_Velocity_dimension_index }, {
				"sdo_get_Velocity_dimension_index",
				tool_sdo_get_Velocity_dimension_index }, {
				"sdo_set_Acceleration_notation_index",
				tool_sdo_set_Acceleration_notation_index }, {
				"sdo_get_Acceleration_notation_index",
				tool_sdo_get_Acceleration_notation_index }, {
				"sdo_set_Acceleration_dimension_index",
				tool_sdo_set_Acceleration_dimension_index }, {
				"sdo_get_Acceleration_dimension_index",
				tool_sdo_get_Acceleration_dimension_index }, {
				"sdo_set_PRE_Encoder_increments",
				tool_sdo_set_PRE_Encoder_increments }, {
				"sdo_get_PRE_Encoder_increments",
				tool_sdo_get_PRE_Encoder_increments }, {
				"sdo_set_PRE_Motor_revolutions",
				tool_sdo_set_PRE_Motor_revolutions }, {
				"sdo_get_PRE_Motor_revolutions",
				tool_sdo_get_PRE_Motor_revolutions }, {
				"sdo_set_VNR_Encoder_increments_per_second",
				tool_sdo_set_VNR_Encoder_increments_per_second }, {
				"sdo_get_VNR_Encoder_increments_per_second",
				tool_sdo_get_VNR_Encoder_increments_per_second }, {
				"sdo_set_VNR_Motor_revolutions_per_second",
				tool_sdo_set_VNR_Motor_revolutions_per_second }, {
				"sdo_get_VNR_Motor_revolutions_per_second",
				tool_sdo_get_VNR_Motor_revolutions_per_second }, {
				"sdo_set_GR_Motor_revolutions",
				tool_sdo_set_GR_Motor_revolutions }, {
				"sdo_get_GR_Motor_revolutions",
				tool_sdo_get_GR_Motor_revolutions }, {
				"sdo_set_GR_Shaft_revolutions",
				tool_sdo_set_GR_Shaft_revolutions }, {
				"sdo_get_GR_Shaft_revolutions",
				tool_sdo_get_GR_Shaft_revolutions }, { "sdo_get_FC_Feed",
				tool_sdo_get_FC_Feed }, { "sdo_set_FC_Feed",
				tool_sdo_set_FC_Feed }, { "sdo_get_FC_Shaft_revolutions",
				tool_sdo_get_FC_Shaft_revolutions }, {
				"sdo_set_FC_Shaft_revolutions",
				tool_sdo_set_FC_Shaft_revolutions }, { "sdo_get_PF_Numerator",
				tool_sdo_get_PF_Numerator }, { "sdo_set_PF_Numerator",
				tool_sdo_set_PF_Numerator }, { "sdo_get_PF_Feed_constant",
				tool_sdo_get_PF_Feed_constant }, { "sdo_set_PF_Feed_constant",
				tool_sdo_set_PF_Feed_constant }, { "sdo_set_VEF_Numerator",
				tool_sdo_set_VEF_Numerator }, { "sdo_get_VEF_Numerator",
				tool_sdo_get_VEF_Numerator }, { "sdo_set_VEF_Divisor",
				tool_sdo_set_VEF_Divisor }, { "sdo_get_VEF_Divisor",
				tool_sdo_get_VEF_Divisor }, { "sdo_set_VF1_Numerator",
				tool_sdo_set_VF1_Numerator }, { "sdo_get_VF1_Numerator",
				tool_sdo_get_VF1_Numerator }, { "sdo_set_VF1_Divisor",
				tool_sdo_set_VF1_Divisor }, { "sdo_get_VF1_Divisor",
				tool_sdo_get_VF1_Divisor }, { "sdo_set_VF2_Numerator",
				tool_sdo_set_VF2_Numerator }, { "sdo_get_VF2_Numerator",
				tool_sdo_get_VF2_Numerator }, { "sdo_set_VF2_Divisor",
				tool_sdo_set_VF2_Divisor }, { "sdo_get_VF2_Divisor",
				tool_sdo_get_VF2_Divisor }, { "sdo_set_AF_Numerator",
				tool_sdo_set_AF_Numerator }, { "sdo_get_AF_Numerator",
				tool_sdo_get_AF_Numerator }, { "sdo_set_AF_Divisor",
				tool_sdo_set_AF_Divisor }, { "sdo_set_AF_Numerator",
				tool_sdo_set_AF_Numerator }, { "sdo_get_AF_Numerator",
				tool_sdo_get_AF_Numerator }, { "sdo_set_AF_Divisor",
				tool_sdo_set_AF_Divisor }, { "sdo_get_AF_Divisor",
				tool_sdo_get_AF_Divisor }, { "sdo_set_Polarity",
				tool_sdo_set_Polarity }, { "sdo_get_Polarity",
				tool_sdo_get_Polarity }, { "sdo_set_Controlword",
				tool_sdo_set_Controlword }, { "sdo_set_Shutdown_option_code",
				tool_sdo_set_Shutdown_option_code }, {
				"sdo_set_Disable_operation_option_code",
				tool_sdo_set_Disable_operation_option_code }, {
				"sdo_set_Quick_stop_option_code",
				tool_sdo_set_Quick_stop_option_code }, {
				"sdo_set_Halt_option_code", tool_sdo_set_Halt_option_code }, {
				"sdo_set_Fault_reaction_option_code",
				tool_sdo_set_Fault_reaction_option_code }, {
				"sdo_set_Modes_of_operation", tool_sdo_set_Modes_of_operation },
		{ "sdo_get_Controlword", tool_sdo_get_Controlword }, {
				"sdo_get_Statusword", tool_sdo_get_Statusword }, {
				"sdo_get_Shutdown_option_code",
				tool_sdo_get_Shutdown_option_code }, {
				"sdo_get_Disable_operation_option_code",
				tool_sdo_get_Disable_operation_option_code }, {
				"sdo_get_Quick_stop_option_code",
				tool_sdo_get_Quick_stop_option_code }, {
				"sdo_get_Halt_option_code", tool_sdo_get_Halt_option_code }, {
				"sdo_get_Fault_reaction_option_code",
				tool_sdo_get_Fault_reaction_option_code }, {
				"sdo_get_Modes_of_operation", tool_sdo_get_Modes_of_operation },
		{ "sdo_get_Modes_of_operation_display",
				tool_sdo_get_Modes_of_operation_display }, {
				"sdo_set_Target_position", tool_sdo_set_Target_position }, {
				"sdo_set_PRL_Min_position_range_limit",
				tool_sdo_set_PRL_Min_position_range_limit }, {
				"sdo_set_PRL_Max_position_range_limit",
				tool_sdo_set_PRL_Max_position_range_limit }, {
				"sdo_set_SPL_Min_position_limit",
				tool_sdo_set_SPL_Min_position_limit }, {
				"sdo_set_SPL_Max_position_limit",
				tool_sdo_set_SPL_Max_position_limit }, {
				"sdo_set_Max_profile_velocity",
				tool_sdo_set_Max_profile_velocity }, {
				"sdo_set_Max_motor_speed", tool_sdo_set_Max_motor_speed }, {
				"sdo_set_Profile_velocity", tool_sdo_set_Profile_velocity }, {
				"sdo_set_End_velocity", tool_sdo_set_End_velocity }, {
				"sdo_set_Profile_acceleration",
				tool_sdo_set_Profile_acceleration }, {
				"sdo_set_Profile_deceleration",
				tool_sdo_set_Profile_deceleration }, {
				"sdo_set_Quick_stop_deceleration",
				tool_sdo_set_Quick_stop_deceleration },
		{ "sdo_set_Motion_profile_type", tool_sdo_set_Motion_profile_type }, {
				"sdo_set_Max_acceleration", tool_sdo_set_Max_acceleration }, {
				"sdo_set_Max_deceleration", tool_sdo_set_Max_deceleration }, {
				"sdo_get_Target_position", tool_sdo_get_Target_position }, {
				"sdo_get_PRL_Min_position_range_limit",
				tool_sdo_get_PRL_Min_position_range_limit }, {
				"sdo_get_PRL_Max_position_range_limit",
				tool_sdo_get_PRL_Max_position_range_limit }, {
				"sdo_get_SPL_Min_position_limit",
				tool_sdo_get_SPL_Min_position_limit }, {
				"sdo_get_SPL_Max_position_limit",
				tool_sdo_get_SPL_Max_position_limit }, {
				"sdo_get_Max_profile_velocity",
				tool_sdo_get_Max_profile_velocity }, {
				"sdo_get_Max_motor_speed", tool_sdo_get_Max_motor_speed }, {
				"sdo_get_Profile_velocity", tool_sdo_get_Profile_velocity }, {
				"sdo_get_End_velocity", tool_sdo_get_End_velocity }, {
				"sdo_get_Profile_acceleration",
				tool_sdo_get_Profile_acceleration }, {
				"sdo_get_Profile_deceleration",
				tool_sdo_get_Profile_deceleration }, {
				"sdo_get_Quick_stop_deceleration",
				tool_sdo_get_Quick_stop_deceleration },
		{ "sdo_get_Motion_profile_type", tool_sdo_get_Motion_profile_type }, {
				"sdo_get_Max_acceleration", tool_sdo_get_Max_acceleration }, {
				"sdo_get_Max_deceleration", tool_sdo_get_Max_deceleration }, {
				"sdo_set_Home_offset", tool_sdo_set_Home_offset }, {
				"sdo_set_Home_method", tool_sdo_set_Home_method }, {
				"sdo_set_HS_Speed_during_search_for_switch",
				tool_sdo_set_HS_Speed_during_search_for_switch }, {
				"sdo_set_HS_Speed_during_search_for_zero",
				tool_sdo_set_HS_Speed_during_search_for_zero },
		{ "sdo_set_Homing_acceleration", tool_sdo_set_Homing_acceleration }, {
				"sdo_get_Home_offset", tool_sdo_get_Home_offset }, {
				"sdo_get_Home_method", tool_sdo_get_Home_method }, {
				"sdo_get_HS_Speed_during_search_for_switch",
				tool_sdo_get_HS_Speed_during_search_for_switch }, {
				"sdo_get_HS_Speed_during_search_for_zero",
				tool_sdo_get_HS_Speed_during_search_for_zero },
		{ "sdo_get_Homing_acceleration", tool_sdo_get_Homing_acceleration }, {
				"sdo_set_Following_error_window",
				tool_sdo_set_Following_error_window }, {
				"sdo_set_Following_error_time_out",
				tool_sdo_set_Following_error_time_out }, {
				"sdo_set_Position_window", tool_sdo_set_Position_window }, {
				"sdo_set_Position_window_time",
				tool_sdo_set_Position_window_time }, {
				"sdo_get_Position_demand_value",
				tool_sdo_get_Position_demand_value }, {
				"sdo_get_Position_actual_value_inc",
				tool_sdo_get_Position_actual_value_inc }, {
				"sdo_get_Position_actual_value",
				tool_sdo_get_Position_actual_value }, {
				"sdo_get_Following_error_window",
				tool_sdo_get_Following_error_window }, {
				"sdo_get_Following_error_time_out",
				tool_sdo_get_Following_error_time_out }, {
				"sdo_get_Position_window", tool_sdo_get_Position_window }, {
				"sdo_get_Position_window_time",
				tool_sdo_get_Position_window_time }, {
				"sdo_get_Following_error_actual_value",
				tool_sdo_get_Following_error_actual_value }, {
				"sdo_get_Control_effort", tool_sdo_get_Control_effort }, {
				"sdo_get_Position_demand_value_inc",
				tool_sdo_get_Position_demand_value_inc }, {
				"sdo_set_Interpolation_sub_mode_select",
				tool_sdo_set_Interpolation_sub_mode_select }, {
				"sdo_set_ITP_ip_time_units", tool_sdo_set_ITP_ip_time_units }, {
				"sdo_set_ITP_ip_time_index", tool_sdo_set_ITP_ip_time_index }, {
				"sdo_set_ISD_Synchronize_on_group",
				tool_sdo_set_ISD_Synchronize_on_group }, {
				"sdo_set_ISD_ip_sync_every_n_event",
				tool_sdo_set_ISD_ip_sync_every_n_event }, {
				"sdo_set_IDC_Actual_buffer_size",
				tool_sdo_set_IDC_Actual_buffer_size }, {
				"sdo_set_IDC_Buffer_organization",
				tool_sdo_set_IDC_Buffer_organization },
		{ "sdo_set_IDC_Buffer_position", tool_sdo_set_IDC_Buffer_position }, {
				"sdo_set_IDC_Size_of_data_record",
				tool_sdo_set_IDC_Size_of_data_record }, {
				"sdo_set_IDC_Buffer_clear", tool_sdo_set_IDC_Buffer_clear }, {
				"sdo_get_Interpolation_sub_mode_select",
				tool_sdo_get_Interpolation_sub_mode_select }, {
				"sdo_get_ITP_ip_time_units", tool_sdo_get_ITP_ip_time_units }, {
				"sdo_get_ITP_ip_time_index", tool_sdo_get_ITP_ip_time_index }, {
				"sdo_get_ISD_Synchronize_on_group",
				tool_sdo_get_ISD_Synchronize_on_group }, {
				"sdo_get_ISD_ip_sync_every_n_event",
				tool_sdo_get_ISD_ip_sync_every_n_event }, {
				"sdo_get_IDC_Maxinum_buffer_size",
				tool_sdo_get_IDC_Maxinum_buffer_size }, {
				"sdo_get_IDC_Actual_buffer_size",
				tool_sdo_get_IDC_Actual_buffer_size }, {
				"sdo_get_IDC_Buffer_organization",
				tool_sdo_get_IDC_Buffer_organization },
		{ "sdo_get_IDC_Buffer_position", tool_sdo_get_IDC_Buffer_position }, {
				"sdo_get_IDC_Size_of_data_record",
				tool_sdo_get_IDC_Size_of_data_record }, {
				"sdo_get_IDC_Buffer_clear", tool_sdo_get_IDC_Buffer_clear }, {
				"sdo_set_Velocity_sensor_actual_value",
				tool_sdo_set_Velocity_sensor_actual_value }, {
				"sdo_set_Sensor_selection_code",
				tool_sdo_set_Sensor_selection_code }, {
				"sdo_set_Velocity_demand_value",
				tool_sdo_set_Velocity_demand_value }, {
				"sdo_set_Velocity_actual_value",
				tool_sdo_set_Velocity_actual_value }, {
				"sdo_set_Velocity_window", tool_sdo_set_Velocity_window }, {
				"sdo_set_Velocity_window_time",
				tool_sdo_set_Velocity_window_time }, {
				"sdo_set_Velocity_threshold", tool_sdo_set_Velocity_threshold },
		{ "sdo_set_Velocity_threshold_time",
				tool_sdo_set_Velocity_threshold_time }, {
				"sdo_set_Target_velocity", tool_sdo_set_Target_velocity }, {
				"sdo_set_Max_slippage", tool_sdo_set_Max_slippage }, {
				"sdo_set_VCPS_Gain", tool_sdo_set_VCPS_Gain }, {
				"sdo_set_VCPS_integration_time_constant",
				tool_sdo_set_VCPS_integration_time_constant }, {
				"sdo_get_Velocity_sensor_actual_value",
				tool_sdo_get_Velocity_sensor_actual_value }, {
				"sdo_get_Sensor_selection_code",
				tool_sdo_get_Sensor_selection_code }, {
				"sdo_get_Velocity_demand_value",
				tool_sdo_get_Velocity_demand_value }, {
				"sdo_get_Velocity_actual_value",
				tool_sdo_get_Velocity_actual_value }, {
				"sdo_get_Velocity_window", tool_sdo_get_Velocity_window }, {
				"sdo_get_Velocity_window_time",
				tool_sdo_get_Velocity_window_time }, {
				"sdo_get_Velocity_threshold", tool_sdo_get_Velocity_threshold },
		{ "sdo_get_Velocity_threshold_time",
				tool_sdo_get_Velocity_threshold_time }, {
				"sdo_get_Target_velocity", tool_sdo_get_Target_velocity }, {
				"sdo_get_Max_slippage", tool_sdo_get_Max_slippage }, {
				"sdo_get_VCPS_Gain", tool_sdo_get_VCPS_Gain }, {
				"sdo_get_VCPS_integration_time_constant",
				tool_sdo_get_VCPS_integration_time_constant }, {
				"sdo_set_Target_torque", tool_sdo_set_Target_torque }, {
				"sdo_set_Max_torque", tool_sdo_set_Max_torque }, {
				"sdo_set_Max_current", tool_sdo_set_Max_current },
		{ "sdo_set_Motor_rated_currect", tool_sdo_set_Motor_rated_currect }, {
				"sdo_set_Motor_reted_torque", tool_sdo_set_Motor_reted_torque },
		{ "sdo_set_Torque_slope", tool_sdo_set_Torque_slope },
		{ "sdo_set_Torque_profile_type", tool_sdo_set_Torque_profile_type }, {
				"sdo_get_Target_torque", tool_sdo_get_Target_torque }, {
				"sdo_get_Max_torque", tool_sdo_get_Max_torque }, {
				"sdo_get_Max_current", tool_sdo_get_Max_current },
		{ "sdo_get_Torque_demand_value", tool_sdo_get_Torque_demand_value },
		{ "sdo_get_Motor_rated_currect", tool_sdo_get_Motor_rated_currect }, {
				"sdo_get_Motor_reted_torque", tool_sdo_get_Motor_reted_torque },
		{ "sdo_get_Torque_actual_value", tool_sdo_get_Torque_actual_value }, {
				"sdo_get_Current_actual_value",
				tool_sdo_get_Current_actual_value }, {
				"sdo_get_DC_link_circuit_voltage",
				tool_sdo_get_DC_link_circuit_voltage }, {
				"sdo_get_Torque_slope", tool_sdo_get_Torque_slope },
		{ "sdo_get_Torque_profile_type", tool_sdo_get_Torque_profile_type }, {
				"sdo_set_vl_target_velocity", tool_sdo_set_vl_target_velocity },
		{ "sdo_set_vl_velocity_demand", tool_sdo_set_vl_velocity_demand }, {
				"sdo_set_vl_control_effort", tool_sdo_set_vl_control_effort }, {
				"sdo_set_vl_manipulated_velocity",
				tool_sdo_set_vl_manipulated_velocity }, {
				"sdo_set_VV_min_amount", tool_sdo_set_VV_min_amount }, {
				"sdo_set_VV_max_amount", tool_sdo_set_VV_max_amount }, {
				"sdo_set_VV_min_pos", tool_sdo_set_VV_min_pos }, {
				"sdo_set_VV_max_pos", tool_sdo_set_VV_max_pos }, {
				"sdo_set_VV_min_neg", tool_sdo_set_VV_min_neg }, {
				"sdo_set_VV_max_neg", tool_sdo_set_VV_max_neg }, {
				"sdo_set_VVA_Delta_speed", tool_sdo_set_VVA_Delta_speed }, {
				"sdo_set_VVA_Delta_time", tool_sdo_set_VVA_Delta_time }, {
				"sdo_set_VVD_Delta_speed", tool_sdo_set_VVD_Delta_speed }, {
				"sdo_set_VVD_Delta_time", tool_sdo_set_VVD_Delta_time }, {
				"sdo_set_VVQS_Delta_speed", tool_sdo_set_VVQS_Delta_speed }, {
				"sdo_set_VVQS_Delta_time", tool_sdo_set_VVQS_Delta_time }, {
				"sdo_set_VSPF_numerator", tool_sdo_set_VSPF_numerator }, {
				"sdo_set_VSPF_denominator", tool_sdo_set_VSPF_denominator }, {
				"sdo_set_VDF_numerator", tool_sdo_set_VDF_numerator }, {
				"sdo_set_VDF_denominator", tool_sdo_set_VDF_denominator }, {
				"sdo_set_vl_pole_number", tool_sdo_set_vl_pole_number }, {
				"sdo_set_vl_velocity_reference",
				tool_sdo_set_vl_velocity_reference }, {
				"sdo_set_vl_ramp_function_time",
				tool_sdo_set_vl_ramp_function_time }, {
				"sdo_set_vl_slow_down_time", tool_sdo_set_vl_slow_down_time }, {
				"sdo_set_vl_quick_stop_time", tool_sdo_set_vl_quick_stop_time },
		{ "sdo_set_vl_nominal_percentage", tool_sdo_set_vl_nominal_percentage },
		{ "sdo_set_vl_percentage_demand", tool_sdo_set_vl_percentage_demand }, {
				"sdo_set_vl_actual_percentage",
				tool_sdo_set_vl_actual_percentage }, {
				"sdo_set_vl_manipulated_percentage",
				tool_sdo_set_vl_manipulated_percentage }, { "sdo_set_VVMA_min",
				tool_sdo_set_VVMA_min }, { "sdo_set_VVMA_max",
				tool_sdo_set_VVMA_max }, { "sdo_set_VVM_min_pos",
				tool_sdo_set_VVM_min_pos }, { "sdo_set_VVM_max_pos",
				tool_sdo_set_VVM_max_pos }, { "sdo_set_VVM_min_neg",
				tool_sdo_set_VVM_min_neg }, { "sdo_set_VVM_max_neg",
				tool_sdo_set_VVM_max_neg }, { "sdo_set_VFMA_min",
				tool_sdo_set_VFMA_min }, { "sdo_set_VFMA_max",
				tool_sdo_set_VFMA_max }, { "sdo_set_VFM_min_pos",
				tool_sdo_set_VFM_min_pos }, { "sdo_set_VFM_max_pos",
				tool_sdo_set_VFM_max_pos }, { "sdo_set_VFM_min_neg",
				tool_sdo_set_VFM_min_neg }, { "sdo_set_VFM_max_neg",
				tool_sdo_set_VFM_max_neg } };

nser_global_data *nser_app_config_init(char *xmlfile);
int nser_app_load_xml(nser_global_data *ns_data, char *xmlfile);
/** The cycle task callback function ***/
int nser_tool_start_task(nser_global_data *ns_data, int isAutoStart);
tool_data *nser_config_tool_init(int max_domain_size);
int nser_config_tool_finish(nser_global_data *ns_data, tool_data *t_data);

#endif /* TOOL_H_ */
