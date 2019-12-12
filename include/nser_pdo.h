// SPDX-License-Identifier: (GPL-2.0 OR MIT)
/*
 * Copyright 2018-2020 NXP
 */

#ifndef NSER_PDO_H_
#define NSER_PDO_H_
#include "config.h"
#include "nservo.h"
struct _nser_axle;

//******************************
//*
//* PDO's index on pdos_offset array on struct nser_axle
//*
//*
//*******************************

// DO : Digital outpus
// PRE : Position encoder resolution
// VNR : Velocity encoder resolution
// GR : Gear ratio
// FC : Feed constant
// PF : Position factor
// VEF : Velocity encoder resolition
// VF1 : Velocity factor 1
// VF2 : Velocity factor 2
// AF : acceleration factor
// PRL :  Position range limit
// SPL : software position limit
// ITP : Interpolation time period
// ISD : Interpolation sync definition
// IDC : Interpolation data configuration
// VCPS : Velocity control parameter set
// VV : Vl velocity
// VVA : vl velocity acceleration
// VVD : vl velocity deceleration
// VVQS : vl velocity quick stop
// VSPF : vl set-point factor
// VDF : vl dimension factor
// VVMA : vl velocity motor amount
// VVM	: vl velocity motor
// VFMA : vl frequency motor amount
// VFM	: vl frequency motor

#define    n_Abort_connection_option_code  0
#define    n_Error_code  1
#define    n_Motor_type  2
#define    n_Motor_calibration_date  3
#define    n_Motor_service_period  4
#define    n_Supported_drive_modes  5
#define    n_Digital_inputs  6
#define    n_DO_Physical_outputs  7
#define    n_DO_Bitmask  8
#define    n_Controlword  9
#define    n_Statusword  10
#define    n_Modes_of_operation  11
#define    n_Modes_of_operation_display  12
#define    n_PRE_Encoder_increments  13
#define    n_PRE_Motor_revolutions  14
#define    n_VNR_Encoder_increments_per_second  15
#define    n_VNR_Motor_revolutions_per_second  16
#define    n_GR_Motor_revolutions  17
#define    n_GR_Shaft_revolutions  18
#define    n_FC_Feed  19
#define    n_FC_Shaft_revolutions  20
#define    n_PF_Numerator  21
#define    n_PF_Feed_constant  22
#define    n_VEF_Numerator  23
#define    n_VEF_Divisor  24
#define    n_VF1_Numerator  25
#define    n_VF1_Divisor  26
#define    n_VF2_Numerator  27
#define    n_VF2_Divisor  28
#define    n_AF_Numerator  29
#define    n_AF_Divisor  30
#define    n_Polarity  31
#define    n_Target_position  32
#define    n_PRL_Min_position_range_limit  33
#define    n_PRL_Max_position_range_limit  34
#define    n_SPL_Min_position_limit  35
#define    n_SPL_Max_position_limit  36
#define    n_Max_profile_velocity  37
#define    n_Max_motor_speed  38
#define    n_Profile_velocity  39
#define    n_End_velocity  40
#define    n_Profile_acceleration  41
#define    n_Profile_deceleration  42
#define    n_Quick_stop_deceleration  43
#define    n_Motion_profile_type  44
#define    n_Max_acceleration  45
#define    n_Max_deceleration  46
#define    n_Home_offset  47
#define    n_Home_method  48
#define    n_HS_Speed_during_search_for_switch  49
#define    n_HS_Speed_during_search_for_zero  50
#define    n_Homing_acceleration  51
#define    n_Position_demand_value  52
#define    n_Position_actual_value_inc  53
#define    n_Position_actual_value  54
#define    n_Following_error_window  55
#define    n_Following_error_time_out  56
#define    n_Position_window  57
#define    n_Position_window_time  58
#define    n_Following_error_actual_value  59
#define    n_Control_effort  60
#define    n_Position_demand_value_inc  61
#define    n_Interpolation_sub_mode_select  62
#define    n_Interpolation_data_record_0  63
#define    n_ITP_ip_time_units  64
#define    n_ITP_ip_time_index  65
#define    n_ISD_Synchronize_on_group  66
#define    n_ISD_ip_sync_every_n_event  67
#define    n_IDC_Maxinum_buffer_size  68
#define    n_IDC_Actual_buffer_size  69
#define    n_IDC_Buffer_organization  70
#define    n_IDC_Buffer_position  71
#define    n_IDC_Size_of_data_record  72
#define    n_IDC_Buffer_clear  73
#define    n_Velocity_sensor_actual_value  74
#define    n_Sensor_selection_code  75
#define    n_Velocity_demand_value  76
#define    n_Velocity_actual_value  77
#define    n_Velocity_window  78
#define    n_Velocity_window_time  79
#define    n_Velocity_threshold  80
#define    n_Velocity_threshold_time  81
#define    n_Target_velocity  82
#define    n_Max_slippage  83
#define    n_VCPS_Gain  84
#define    n_VCPS_integration_time_constant  85
#define    n_VCPS_Gain_1  86
#define    n_VCPS_integration_time_constant_1  87
#define    n_Target_torque  88
#define    n_Max_torque  89
#define    n_Max_current  90
#define    n_Torque_demand_value  91
#define    n_Motor_rated_currect  92
#define    n_Motor_reted_torque  93
#define    n_Torque_actual_value  94
#define    n_Current_actual_value  95
#define    n_DC_link_circuit_voltage  96
#define    n_Torque_slope  97
#define    n_Torque_profile_type  98
#define    n_vl_target_velocity  99
#define    n_vl_velocity_demand  100
#define    n_vl_control_effort  101
#define    n_vl_manipulated_velocity  102
#define    n_VV_min_amount  103
#define    n_VV_max_amount  104
#define    n_VV_min_pos  105
#define    n_VV_max_pos  106
#define    n_VV_min_neg  107
#define    n_VV_max_neg  108
#define    n_VVA_Delta_speed  109
#define    n_VVA_Delta_time  110
#define    n_VVD_Delta_speed  111
#define    n_VVD_Delta_time  112
#define    n_VVQS_Delta_speed  113
#define    n_VVQS_Delta_time  114
#define    n_VSPF_numerator  115
#define    n_VSPF_denominator  116
#define    n_VDF_numerator  117
#define    n_VDF_denominator  118
#define    n_vl_pole_number  119
#define    n_vl_velocity_reference  120
#define    n_vl_ramp_function_time  121
#define    n_vl_slow_down_time  122
#define    n_vl_quick_stop_time  123
#define    n_vl_nominal_percentage  124
#define    n_vl_percentage_demand  125
#define    n_vl_actual_percentage  126
#define    n_vl_manipulated_percentage  127
#define    n_VVMA_min  128
#define    n_VVMA_max  129
#define    n_VVM_min_pos  130
#define    n_VVM_max_pos  131
#define    n_VVM_min_neg  132
#define    n_VVM_max_neg  133
#define    n_VFMA_min  134
#define    n_VFMA_max  135
#define    n_VFM_min_pos  136
#define    n_VFM_max_pos  137
#define    n_VFM_min_neg  138
#define    n_VFM_max_neg  139
//#define	   PDO_ENTRY_NUM_PER_AXLE 140

#define pdo_get(name, U, u) \
	static inline u nser_pdo_get_##name(nser_axle *ns_axle) \
	{\
	return EC_READ_##U(ns_axle->domain_dp + ns_axle->pdos_offset[n_##name]); \
	}

#define pdo_set(name, U, u ) \
	static inline void nser_pdo_set_##name(nser_axle *ns_axle, u v) \
	{\
		EC_WRITE_##U(ns_axle->domain_dp + ns_axle->pdos_offset[n_##name], v); \
	}

/*Only read*/
pdo_get(Error_code, U16, uint16_t)
pdo_get(Statusword, U16, uint16_t)
pdo_get(Supported_drive_modes, U32, uint32_t)
pdo_get(Following_error_actual_value, S32, int32_t)
pdo_get(Control_effort, S32, int32_t)
pdo_get(Position_demand_value_inc, S32, int32_t)
pdo_get(IDC_Maxinum_buffer_size, U32, uint32_t)
pdo_get(Velocity_sensor_actual_value, S32, int32_t)
pdo_get(Velocity_demand_value, S32, int32_t)
pdo_get(Velocity_actual_value, S32, int32_t)
pdo_get(Max_slippage, S32, int32_t)
pdo_get(Torque_demand_value, S16, int16_t)
pdo_get(Torque_actual_value, S16, int16_t)
pdo_get(Current_actual_value, S16, int16_t)
pdo_get(DC_link_circuit_voltage, U32, uint32_t)
pdo_get(vl_velocity_demand, S16, int16_t)
pdo_get(vl_control_effort, S16, int16_t)
pdo_get(vl_manipulated_velocity, S16, int16_t)
pdo_get(vl_percentage_demand, S16, int16_t)
pdo_get(vl_actual_percentage, S16, int16_t)
pdo_get(vl_manipulated_percentage, S16, int16_t)

/*Only write*/
pdo_set(IDC_Size_of_data_record, U8, uint8_t)
pdo_set(IDC_Buffer_clear, U8, uint8_t)

/*RW*/
pdo_set(Abort_connection_option_code, S16, int16_t)
pdo_set(Motor_type, U16, uint16_t)
pdo_set(Motor_service_period, U32, uint32_t)
pdo_set(Digital_inputs, U32, uint32_t)
pdo_set(DO_Physical_outputs, U32, uint32_t)
pdo_set(DO_Bitmask, U32, uint32_t)
pdo_set(Controlword, U16, uint16_t)
pdo_set(Modes_of_operation, S8, int8_t)
pdo_set(Modes_of_operation_display, S8, int8_t)
pdo_set(PRE_Encoder_increments, U32, uint32_t)
pdo_set(PRE_Motor_revolutions, U32, uint32_t)
pdo_set(VNR_Encoder_increments_per_second, U32, uint32_t)
pdo_set(VNR_Motor_revolutions_per_second, U32, uint32_t)
pdo_set(GR_Motor_revolutions, U32, uint32_t)
pdo_set(GR_Shaft_revolutions, U32, uint32_t)
pdo_set(FC_Feed, U32, uint32_t)
pdo_set(FC_Shaft_revolutions, U32, uint32_t)
pdo_set(PF_Numerator, U32, uint32_t)
pdo_set(PF_Feed_constant, U32, uint32_t)
pdo_set(VEF_Numerator, U32, uint32_t)
pdo_set(VEF_Divisor, U32, uint32_t)
pdo_set(VF1_Numerator, U32, uint32_t)
pdo_set(VF1_Divisor, U32, uint32_t)
pdo_set(VF2_Numerator, U32, uint32_t)
pdo_set(VF2_Divisor, U32, uint32_t)
pdo_set(AF_Numerator, U32, uint32_t)
pdo_set(AF_Divisor, U32, uint32_t)
pdo_set(Polarity, U8, uint8_t)
pdo_set(Target_position, S32, int32_t)
pdo_set(PRL_Min_position_range_limit, S32, int32_t)
pdo_set(PRL_Max_position_range_limit, S32, int32_t)
pdo_set(SPL_Min_position_limit, S32, int32_t)
pdo_set(SPL_Max_position_limit, S32, int32_t)
pdo_set(Max_profile_velocity, U32, uint32_t)
pdo_set(Max_motor_speed, U32, uint32_t)
pdo_set(Profile_velocity, U32, uint32_t)
pdo_set(End_velocity, U32, uint32_t)
pdo_set(Profile_acceleration, U32, uint32_t)
pdo_set(Profile_deceleration, U32, uint32_t)
pdo_set(Quick_stop_deceleration, U32, uint32_t)
pdo_set(Motion_profile_type, S16, int16_t)
pdo_set(Max_acceleration, U32, uint32_t)
pdo_set(Max_deceleration, U32, uint32_t)
pdo_set(Home_offset, U32, uint32_t)
pdo_set(Home_method, S8, int8_t)
pdo_set(HS_Speed_during_search_for_switch, U32, uint32_t)
pdo_set(HS_Speed_during_search_for_zero, U32, uint32_t)
pdo_set(Homing_acceleration, U32, uint32_t)
pdo_set(Position_demand_value, S32, int32_t)
pdo_set(Position_actual_value_inc, S32, int32_t)
pdo_set(Position_actual_value, S32, int32_t)
pdo_set(Following_error_window, U32, uint32_t)
pdo_set(Following_error_time_out, U16, uint16_t)
pdo_set(Position_window, U32, uint32_t)
pdo_set(Position_window_time, U16, uint16_t)
pdo_set(Interpolation_sub_mode_select, S16, int16_t)
pdo_set(Interpolation_data_record_0, S16, int16_t)
pdo_set(ITP_ip_time_units, U8, uint8_t)
pdo_set(ITP_ip_time_index, S8, int8_t)
pdo_set(ISD_Synchronize_on_group, U8, uint8_t)
pdo_set(ISD_ip_sync_every_n_event, U8, uint8_t)
pdo_set(IDC_Actual_buffer_size, U32, uint32_t)
pdo_set(IDC_Buffer_organization, U8, uint8_t)
pdo_set(IDC_Buffer_position, U16, uint16_t)
pdo_set(Sensor_selection_code, S16, int16_t)
pdo_set(Velocity_window, U16, uint16_t)
pdo_set(Velocity_window_time, U16, uint16_t)
pdo_set(Velocity_threshold, U16, uint16_t)
pdo_set(Velocity_threshold_time, U16, uint16_t)
pdo_set(Target_velocity, S32, int32_t)
pdo_set(VCPS_Gain, U16, uint16_t)
pdo_set(VCPS_integration_time_constant, U16, uint16_t)
pdo_set(VCPS_Gain_1, U16, uint16_t)
pdo_set(VCPS_integration_time_constant_1, U16, uint16_t)
pdo_set(Target_torque, S16, int16_t)
pdo_set(Max_torque, U16, uint16_t)
pdo_set(Max_current, U16, uint16_t)
pdo_set(Motor_rated_currect, U32, uint32_t)
pdo_set(Motor_reted_torque, U32, uint32_t)
pdo_set(Torque_slope, U32, uint32_t)
pdo_set(Torque_profile_type, S16, int16_t)
pdo_set(vl_target_velocity, S16, int16_t)
pdo_set(VV_min_amount, U32, uint32_t)
pdo_set(VV_max_amount, U32, uint32_t)
pdo_set(VV_min_pos, U32, uint32_t)
pdo_set(VV_max_pos, U32, uint32_t)
pdo_set(VV_min_neg, U32, uint32_t)
pdo_set(VV_max_neg, U32, uint32_t)
pdo_set(VVA_Delta_speed, U32, uint32_t)
pdo_set(VVA_Delta_time, U16, uint16_t)
pdo_set(VVD_Delta_speed, U32, uint32_t)
pdo_set(VVD_Delta_time, U16, uint16_t)
pdo_set(VVQS_Delta_speed, U32, uint32_t)
pdo_set(VVQS_Delta_time, U16, uint16_t)
pdo_set(VSPF_numerator, U16, uint16_t)
pdo_set(VSPF_denominator, U16, uint16_t)
pdo_set(VDF_numerator, S32, int32_t)
pdo_set(VDF_denominator, S32, int32_t)
pdo_set(vl_pole_number, U8, uint8_t)
pdo_set(vl_velocity_reference, U32, uint32_t)
pdo_set(vl_ramp_function_time, U32, uint32_t)
pdo_set(vl_slow_down_time, U32, uint32_t)
pdo_set(vl_quick_stop_time, U32, uint32_t)
pdo_set(vl_nominal_percentage, S16, int16_t)
pdo_set(VVMA_min, U32, uint32_t)
pdo_set(VVMA_max, U32, uint32_t)
pdo_set(VVM_min_pos, U32, uint32_t)
pdo_set(VVM_max_pos, U32, uint32_t)
pdo_set(VVM_min_neg, U32, uint32_t)
pdo_set(VVM_max_neg, U32, uint32_t)
pdo_set(VFMA_min, U32, uint32_t)
pdo_set(VFMA_max, U32, uint32_t)
pdo_set(VFM_min_pos, U32, uint32_t)
pdo_set(VFM_max_pos, U32, uint32_t)
pdo_set(VFM_min_neg, U32, uint32_t)
pdo_set(VFM_max_neg, U32, uint32_t)
pdo_get(Abort_connection_option_code, S16, int16_t)
pdo_get(Motor_type, U16, uint16_t)
pdo_get(Motor_service_period, U32, uint32_t)
pdo_get(Digital_inputs, U32, uint32_t)
pdo_get(DO_Physical_outputs, U32, uint32_t)
pdo_get(DO_Bitmask, U32, uint32_t)
pdo_get(Controlword, U16, uint16_t)
pdo_get(Modes_of_operation, S8, int8_t)
pdo_get(Modes_of_operation_display, S8, int8_t)
pdo_get(PRE_Encoder_increments, U32, uint32_t)
pdo_get(PRE_Motor_revolutions, U32, uint32_t)
pdo_get(VNR_Encoder_increments_per_second, U32, uint32_t)
pdo_get(VNR_Motor_revolutions_per_second, U32, uint32_t)
pdo_get(GR_Motor_revolutions, U32, uint32_t)
pdo_get(GR_Shaft_revolutions, U32, uint32_t)
pdo_get(FC_Feed, U32, uint32_t)
pdo_get(FC_Shaft_revolutions, U32, uint32_t)
pdo_get(PF_Numerator, U32, uint32_t)
pdo_get(PF_Feed_constant, U32, uint32_t)
pdo_get(VEF_Numerator, U32, uint32_t)
pdo_get(VEF_Divisor, U32, uint32_t)
pdo_get(VF1_Numerator, U32, uint32_t)
pdo_get(VF1_Divisor, U32, uint32_t)
pdo_get(VF2_Numerator, U32, uint32_t)
pdo_get(VF2_Divisor, U32, uint32_t)
pdo_get(AF_Numerator, U32, uint32_t)
pdo_get(AF_Divisor, U32, uint32_t)
pdo_get(Polarity, U8, uint8_t)
pdo_get(Target_position, S32, int32_t)
pdo_get(PRL_Min_position_range_limit, S32, int32_t)
pdo_get(PRL_Max_position_range_limit, S32, int32_t)
pdo_get(SPL_Min_position_limit, S32, int32_t)
pdo_get(SPL_Max_position_limit, S32, int32_t)
pdo_get(Max_profile_velocity, U32, uint32_t)
pdo_get(Max_motor_speed, U32, uint32_t)
pdo_get(Profile_velocity, U32, uint32_t)
pdo_get(End_velocity, U32, uint32_t)
pdo_get(Profile_acceleration, U32, uint32_t)
pdo_get(Profile_deceleration, U32, uint32_t)
pdo_get(Quick_stop_deceleration, U32, uint32_t)
pdo_get(Motion_profile_type, S16, int16_t)
pdo_get(Max_acceleration, U32, uint32_t)
pdo_get(Max_deceleration, U32, uint32_t)
pdo_get(Home_offset, U32, uint32_t)
pdo_get(Home_method, S8, int8_t)
pdo_get(HS_Speed_during_search_for_switch, U32, uint32_t)
pdo_get(HS_Speed_during_search_for_zero, U32, uint32_t)
pdo_get(Homing_acceleration, U32, uint32_t)
pdo_get(Position_demand_value, S32, int32_t)
pdo_get(Position_actual_value_inc, S32, int32_t)
pdo_get(Position_actual_value, S32, int32_t)
pdo_get(Following_error_window, U32, uint32_t)
pdo_get(Following_error_time_out, U16, uint16_t)
pdo_get(Position_window, U32, uint32_t)
pdo_get(Position_window_time, U16, uint16_t)
pdo_get(Interpolation_sub_mode_select, S16, int16_t)
pdo_get(Interpolation_data_record_0, S16, int16_t)
pdo_get(ITP_ip_time_units, U8, uint8_t)
pdo_get(ITP_ip_time_index, S8, int8_t)
pdo_get(ISD_Synchronize_on_group, U8, uint8_t)
pdo_get(ISD_ip_sync_every_n_event, U8, uint8_t)
pdo_get(IDC_Actual_buffer_size, U32, uint32_t)
pdo_get(IDC_Buffer_organization, U8, uint8_t)
pdo_get(IDC_Buffer_position, U16, uint16_t)
pdo_get(Sensor_selection_code, S16, int16_t)
pdo_get(Velocity_window, U16, uint16_t)
pdo_get(Velocity_window_time, U16, uint16_t)
pdo_get(Velocity_threshold, U16, uint16_t)
pdo_get(Velocity_threshold_time, U16, uint16_t)
pdo_get(Target_velocity, S32, int32_t)
pdo_get(VCPS_Gain, U16, uint16_t)
pdo_get(VCPS_integration_time_constant, U16, uint16_t)
pdo_get(VCPS_Gain_1, U16, uint16_t)
pdo_get(VCPS_integration_time_constant_1, U16, uint16_t)
pdo_get(Target_torque, S16, int16_t)
pdo_get(Max_torque, U16, uint16_t)
pdo_get(Max_current, U16, uint16_t)
pdo_get(Motor_rated_currect, U32, uint32_t)
pdo_get(Motor_reted_torque, U32, uint32_t)
pdo_get(Torque_slope, U32, uint32_t)
pdo_get(Torque_profile_type, S16, int16_t)
pdo_get(vl_target_velocity, S16, int16_t)
pdo_get(VV_min_amount, U32, uint32_t)
pdo_get(VV_max_amount, U32, uint32_t)
pdo_get(VV_min_pos, U32, uint32_t)
pdo_get(VV_max_pos, U32, uint32_t)
pdo_get(VV_min_neg, U32, uint32_t)
pdo_get(VV_max_neg, U32, uint32_t)
pdo_get(VVA_Delta_speed, U32, uint32_t)
pdo_get(VVA_Delta_time, U16, uint16_t)
pdo_get(VVD_Delta_speed, U32, uint32_t)
pdo_get(VVD_Delta_time, U16, uint16_t)
pdo_get(VVQS_Delta_speed, U32, uint32_t)
pdo_get(VVQS_Delta_time, U16, uint16_t)
pdo_get(VSPF_numerator, U16, uint16_t)
pdo_get(VSPF_denominator, U16, uint16_t)
pdo_get(VDF_numerator, S32, int32_t)
pdo_get(VDF_denominator, S32, int32_t)
pdo_get(vl_pole_number, U8, uint8_t)
pdo_get(vl_velocity_reference, U32, uint32_t)
pdo_get(vl_ramp_function_time, U32, uint32_t)
pdo_get(vl_slow_down_time, U32, uint32_t)
pdo_get(vl_quick_stop_time, U32, uint32_t)
pdo_get(vl_nominal_percentage, S16, int16_t)
pdo_get(VVMA_min, U32, uint32_t)
pdo_get(VVMA_max, U32, uint32_t)
pdo_get(VVM_min_pos, U32, uint32_t)
pdo_get(VVM_max_pos, U32, uint32_t)
pdo_get(VVM_min_neg, U32, uint32_t)
pdo_get(VVM_max_neg, U32, uint32_t)
pdo_get(VFMA_min, U32, uint32_t)
pdo_get(VFMA_max, U32, uint32_t)
pdo_get(VFM_min_pos, U32, uint32_t)
pdo_get(VFM_max_pos, U32, uint32_t)
pdo_get(VFM_min_neg, U32, uint32_t)
pdo_get(VFM_max_neg, U32, uint32_t)

struct pdo_entry_id {
	uint16_t index;
	uint8_t subindex;
	char *entry_name;
	int offset;
};

static struct pdo_entry_id pdo_entry_is_table[] __attribute__((unused)) = { {
		0x6070, 0, "Abort_connection_option_code",
		n_Abort_connection_option_code }, { 0x603f, 0, "Error_code",
		n_Error_code }, { 0x6402, 0, "Motor_type", n_Motor_type }, { 0x6406, 0,
		"Motor_calibration_date", n_Motor_calibration_date }, { 0x6407, 0,
		"Motor_service_period", n_Motor_service_period }, { 0x6502, 0,
		"Supported_drive_modes", n_Supported_drive_modes }, { 0x60fd, 0,
		"Digital_inputs", n_Digital_inputs }, { 0x60fe, 1, "Physical_outputs",
		n_DO_Physical_outputs }, { 0x60fe, 2, "Bitmask", n_DO_Bitmask }, {
		0x6040, 0, "Controlword", n_Controlword }, { 0x6041, 0, "Statusword",
		n_Statusword },
		{ 0x6060, 0, "Modes_of_operation", n_Modes_of_operation }, { 0x6061, 0,
				"Modes_of_operation_display", n_Modes_of_operation_display }, {
				0x608f, 1, "Encoder_increments", n_PRE_Encoder_increments }, {
				0x608f, 2, "Motor_revolutions", n_PRE_Motor_revolutions }, {
				0x6090, 1, "Encoder_increments_per_second",
				n_VNR_Encoder_increments_per_second }, { 0x6090, 2,
				"Motor_revolutions_per_second",
				n_VNR_Motor_revolutions_per_second }, { 0x6091, 1,
				"GR_Motor_revolutions", n_GR_Motor_revolutions }, { 0x6091, 2,
				"GR_Shaft_revolutions", n_GR_Shaft_revolutions }, { 0x6092, 1,
				"FC_Feed", n_FC_Feed }, { 0x6092, 2, "FC_Shaft_revolutions",
				n_FC_Shaft_revolutions }, { 0x6093, 1, "PF_Numerator",
				n_PF_Numerator }, { 0x6093, 2, "PF_Feed_constant",
				n_PF_Feed_constant }, { 0x6094, 1, "VEF_Numerator",
				n_VEF_Numerator }, { 0x6094, 2, "VEF_Divisor", n_VEF_Divisor },
		{ 0x6095, 1, "VF1_Numerator", n_VF1_Numerator }, { 0x6095, 2,
				"VF1_Divisor", n_VF1_Divisor }, { 0x6096, 1, "VF2_Numerator",
				n_VF2_Numerator }, { 0x6096, 2, "VF2_Divisor", n_VF2_Divisor },
		{ 0x6097, 1, "AF_Numerator", n_AF_Numerator }, { 0x6097, 2,
				"AF_Divisor", n_AF_Divisor }, { 0x607e, 0, "Polarity",
				n_Polarity },
		{ 0x607a, 0, "Target_position", n_Target_position }, { 0x607b, 1,
				"Min_position_range_limit", n_PRL_Min_position_range_limit }, {
				0x607b, 2, "Max_position_range_limit",
				n_PRL_Max_position_range_limit }, { 0x607d, 1,
				"Min_position_limit", n_SPL_Min_position_limit }, { 0x607d, 2,
				"Max_position_limit", n_SPL_Max_position_limit }, { 0x607f, 0,
				"Max_profile_velocity", n_Max_profile_velocity }, { 0x6080, 0,
				"Max_motor_speed", n_Max_motor_speed }, { 0x6081, 0,
				"Profile_velocity", n_Profile_velocity }, { 0x6082, 0,
				"End_velocity", n_End_velocity }, { 0x6083, 0,
				"Profile_acceleration", n_Profile_acceleration }, { 0x6084, 0,
				"Profile_deceleration", n_Profile_deceleration }, { 0x6085, 0,
				"Quick_stop_deceleration", n_Quick_stop_deceleration }, {
				0x6086, 0, "Motion_profile_type", n_Motion_profile_type }, {
				0x60C5, 0, "Max_acceleration", n_Max_acceleration }, { 0x60C6,
				0, "Max_deceleration", n_Max_deceleration }, { 0x607C, 0,
				"Home_offset", n_Home_offset }, { 0x6098, 0, "Home_method",
				n_Home_method }, { 0x6099, 1, "Speed_during_search_for_switch",
				n_HS_Speed_during_search_for_switch }, { 0x6099, 2,
				"Speed_during_search_for_zero",
				n_HS_Speed_during_search_for_zero }, { 0x609A, 0,
				"Homing_acceleration", n_Homing_acceleration }, { 0x6062, 0,
				"Position_demand_value", n_Position_demand_value }, { 0x6063, 0,
				"Position_actual_value_inc", n_Position_actual_value_inc }, {
				0x6064, 0, "Position_actual_value", n_Position_actual_value }, {
				0x6065, 0, "Following_error_window", n_Following_error_window },
		{ 0x6066, 0, "Following_error_time_out", n_Following_error_time_out }, {
				0x6067, 0, "Position_window", n_Position_window }, { 0x6068, 0,
				"Position_window_time", n_Position_window_time },
		{ 0x60F4, 0, "Following_error_actual_value",
				n_Following_error_actual_value }, { 0x60FA, 0, "Control_effort",
				n_Control_effort }, { 0x60FC, 0, "Position_demand_value_inc",
				n_Position_demand_value_inc },
		{ 0x60C0, 0, "Interpolation_sub_mode_select",
				n_Interpolation_sub_mode_select }, { 0x60C1, 1,
				"Interpolation_data_record_0", n_Interpolation_data_record_0 },
		{ 0x60C2, 1, "ip_time_units", n_ITP_ip_time_units }, { 0x60C2, 2,
				"ip_time_index", n_ITP_ip_time_index }, { 0x60C3, 1,
				"Synchronize_on_group", n_ISD_Synchronize_on_group }, { 0x60C3,
				2, "ip_sync_every_n_event", n_ISD_ip_sync_every_n_event }, {
				0x60C4, 1, "Maxinum_buffer_size", n_IDC_Maxinum_buffer_size }, {
				0x60C4, 2, "Actual_buffer_size", n_IDC_Actual_buffer_size }, {
				0x60C4, 3, "Buffer_organization", n_IDC_Buffer_organization }, {
				0x60C4, 4, "Buffer_position", n_IDC_Buffer_position }, { 0x60C4,
				5, "Size_of_data_record", n_IDC_Size_of_data_record }, { 0x60C4,
				6, "Buffer_clear", n_IDC_Buffer_clear },
		{ 0x6069, 0, "Velocity_sensor_actual_value",
				n_Velocity_sensor_actual_value }, { 0x606A, 0,
				"Sensor_selection_code", n_Sensor_selection_code }, { 0x606B, 0,
				"Velocity_demand_value", n_Velocity_demand_value }, { 0x606C, 0,
				"Velocity_actual_value", n_Velocity_actual_value }, { 0x606D, 0,
				"Velocity_window", n_Velocity_window }, { 0x606E, 0,
				"Velocity_window_time", n_Velocity_window_time }, { 0x606F, 0,
				"Velocity_threshold", n_Velocity_threshold }, { 0x6070, 0,
				"Velocity_threshold_time", n_Velocity_threshold_time }, {
				0x60FF, 0, "Target_velocity", n_Target_velocity }, { 0x60F8, 0,
				"Max_slippage", n_Max_slippage }, { 0x60F9, 1, "Gain",
				n_VCPS_Gain }, { 0x60F9, 2, "integration_time_constant",
				n_VCPS_integration_time_constant }, { 0x60F9, 3, "Gain_1",
				n_VCPS_Gain_1 }, { 0x60F9, 4, "Integration_time_constant_1",
				n_VCPS_integration_time_constant_1 }, { 0x6071, 0,
				"Target_torque", n_Target_torque }, { 0x6072, 0, "Max_torque",
				n_Max_torque }, { 0x6073, 0, "Max_current", n_Max_current }, {
				0x6074, 0, "Torque_demand_value", n_Torque_demand_value }, {
				0x6075, 0, "Motor_rated_currect", n_Motor_rated_currect }, {
				0x6076, 0, "Motor_reted_torque", n_Motor_reted_torque }, {
				0x6077, 0, "Torque_actual_value", n_Torque_actual_value }, {
				0x6078, 0, "Current_actual_value", n_Current_actual_value },
		{ 0x6079, 0, "DC_link_circuit_voltage", n_DC_link_circuit_voltage }, {
				0x6087, 0, "Torque_slope", n_Torque_slope }, { 0x6088, 0,
				"Torque_profile_type", n_Torque_profile_type }, { 0x6042, 0,
				"vl_target_velocity", n_vl_target_velocity }, { 0x6043, 0,
				"vl_velocity_demand", n_vl_velocity_demand }, { 0x6044, 0,
				"vl_control_effort", n_vl_control_effort }, { 0x6045, 0,
				"vl_manipulated_velocity", n_vl_manipulated_velocity }, {
				0x6046, 1, "VV_min_amount", n_VV_min_amount }, { 0x6046, 2,
				"VV_max_amount", n_VV_max_amount }, { 0x6047, 1, "VV_min_pos",
				n_VV_min_pos }, { 0x6047, 2, "VV_max_pos", n_VV_max_pos }, {
				0x6047, 3, "VV_min_neg", n_VV_min_neg }, { 0x6047, 4,
				"VV_max_neg", n_VV_max_neg }, { 0x6048, 1, "VVA_Delta_speed",
				n_VVA_Delta_speed }, { 0x6048, 2, "VVA_Delta_time",
				n_VVA_Delta_time }, { 0x6049, 1, "VVD_Delta_speed",
				n_VVD_Delta_speed }, { 0x6049, 2, "VVD_Delta_time",
				n_VVD_Delta_time }, { 0x604A, 1, "VVQS_Delta_speed",
				n_VVQS_Delta_speed }, { 0x604A, 2, "VVQS_Delta_time",
				n_VVQS_Delta_time }, { 0x604B, 1, "VSPF_numerator",
				n_VSPF_numerator }, { 0x604B, 2, "VSPF_denominator",
				n_VSPF_denominator }, { 0x604C, 1, "VDF_numerator",
				n_VDF_numerator }, { 0x604C, 2, "VDF_denominator",
				n_VDF_denominator }, { 0x604D, 0, "vl_pole_number",
				n_vl_pole_number }, { 0x604E, 0, "vl_velocity_reference",
				n_vl_velocity_reference }, { 0x604F, 0, "vl_ramp_function_time",
				n_vl_ramp_function_time }, { 0x6050, 0, "vl_slow_down_time",
				n_vl_slow_down_time }, { 0x6051, 0, "vl_quick_stop_time",
				n_vl_quick_stop_time }, { 0x6052, 0, "vl_nominal_percentage",
				n_vl_nominal_percentage }, { 0x6053, 0, "vl_percentage_demand",
				n_vl_percentage_demand }, { 0x6054, 0, "vl_actual_percentage",
				n_vl_actual_percentage }, { 0x6055, 0,
				"vl_manipulated_percentage", n_vl_manipulated_percentage }, {
				0x6056, 1, "VVMA_min", n_VVMA_min }, { 0x6056, 2, "VVMA_max",
				n_VVMA_max }, { 0x6057, 1, "VVM_min_pos", n_VVM_min_pos }, {
				0x6057, 2, "VVM_max_pos", n_VVM_max_pos }, { 0x6057, 3,
				"VVM_min_neg", n_VVM_min_neg }, { 0x6057, 4, "VVM_max_neg",
				n_VVM_max_neg }, { 0x6058, 1, "VFMA_min", n_VFMA_min }, {
				0x6058, 2, "VFMA_max", n_VFMA_max }, { 0x6059, 1, "VFM_min_pos",
				n_VFM_min_pos }, { 0x6059, 2, "VFM_max_pos", n_VFM_max_pos }, {
				0x6059, 3, "VFM_min_neg", n_VFM_min_neg }, { 0x6059, 4,
				"VFM_max_neg", n_VFM_max_neg } };
#endif /* NSER_PDO_H_ */
