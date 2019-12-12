// SPDX-License-Identifier: (GPL-2.0 OR MIT)
/*
 * Copyright 2018-2020 NXP
 */

#ifndef DS402_H_
#define DS402_H_
#include <stdint.h>
//******************************
//*
//* DS402 object dictionary
//*
//*
//*******************************

/****** Common entries *******/
#define Abort_connection_option_code	0x6007, 0x0
#define Error_code					 	0x603F, 0x0
#define Motor_type						0x6402, 0x0
#define Motor_catalog_number			0x6403, 0x0
#define Motor_manufacturer				0x6404, 0x0
#define http_motor_catalog_address		0x6405, 0x0
#define Motor_calibration_date			0x6406, 0x0
#define Motor_service_period			0x6407, 0x0
#define Motor_data						0x6410, 0x0
#define Supported_drive_modes			0x6502, 0x0
#define Drive_catalog_number			0x6503, 0x0
#define Drive_manufacturer				0x6504, 0x0
#define http_drive_catalog_address		0x6505, 0x0
#define drive_data						0x6510, 0x0
#define Digital_inputs					0x60FD, 0x0
#define DO_Physical_outputs				0x60FE, 0x01
#define DO_Bitmask						0x60FE, 0x02

/******* Device control ********/
#define Controlword      				0x6040, 0x0
#define Statusword       				0x6041, 0x0
#define Shutdown_option_code			0x605B, 0x0
#define Disable_operation_option_code	0x605C, 0x0
#define Quick_stop_option_code			0x605A, 0x0
#define Halt_option_code				0x605D, 0x0
#define Fault_reaction_option_code		0x605E, 0x0
#define Modes_of_operation   			0x6060, 0x0
#define Modes_of_operation_display  	0x6061, 0x0

/****Factor group*****/
#define Position_notation_index			0x6089, 0x0
#define Position_dimension_index		0x608A, 0x0
#define Velocity_notation_index			0x608B, 0x0
#define Velocity_dimension_index		0x608C, 0x0
#define Acceleration_notation_index		0x608D, 0x0
#define Acceleration_dimension_index	0x608E, 0x0

#define PRE_Encoder_increments			0x608F, 0x01   // PRE : Position encoder resolution
#define PRE_Motor_revolutions			0x608F, 0x02

#define VNR_Encoder_increments_per_second	0x6090, 0x01	    // VNR : Velocity encoder resolution
#define VNR_Motor_revolutions_per_second	0x6090, 0x02

#define GR_Motor_revolutions  				0x6091, 0x01 				  // GR : Gear ratio
#define GR_Shaft_revolutions				0x6091, 0x02

#define FC_Feed							   	0x6092, 0x01  // FC : Feed constant
#define FC_Shaft_revolutions				0x6092, 0x02

#define PF_Numerator						0x6093, 0x01   // PF : Position factor
#define PF_Feed_constant					0x6093, 0x02

#define VEF_Numerator						0x6094, 0x01	  // VEF : Velocity encoder resolition
#define VEF_Divisor							0x6094, 0x02

#define VF1_Numerator						0x6095, 0x01    // VF1 : Velocity factor 1
#define VF1_Divisor							0x6095, 0x02

#define VF2_Numerator						0x6096, 0x01    // VF2 : Velocity factor 2
#define VF2_Divisor							0x6096, 0x02

#define AF_Numerator						0x6097, 0x01    // AF : acceleration factor
#define AF_Divisor							0x6097, 0x02

#define Polarity							0x607E, 0x00

/***** Profile posion mode ******/
#define Target_position						0x607A, 0x00

#define PRL_Min_position_range_limit      	0x607B, 0x01// PRL :  Position range limit
#define PRL_Max_position_range_limit		0x607B, 0x02

#define SPL_Min_position_limit			  	0x607D, 0x01// SPL : software position limit
#define SPL_Max_position_limit				0x607D, 0x02

#define Max_profile_velocity				0x607F, 0x00
#define Max_motor_speed						0x6080, 0x00
#define Profile_velocity					0x6081, 0x00
#define End_velocity						0x6082, 0x00
#define Profile_acceleration				0x6083, 0x00
#define Profile_deceleration				0x6084, 0x00
#define Quick_stop_deceleration				0x6085, 0x00
#define Motion_profile_type					0x6086, 0x00
#define Max_acceleration					0x60C5, 0x00
#define Max_deceleration					0x60C6, 0x00

/**** Homing mode ******/
#define Home_offset							0x607C, 0x00
#define Home_method							0x6098, 0x00
#define HS_Speed_during_search_for_switch	0x6099, 0x01
#define HS_Speed_during_search_for_zero		0x6099, 0x02
#define Homing_acceleration					0x609A, 0x00

/****Position control function*******/
#define Position_demand_value				0x6062, 0x00
#define Position_actual_value_inc			0x6063, 0x00
#define Position_actual_value				0x6064, 0x00
#define Following_error_window				0x6065, 0x00
#define Following_error_time_out 			0x6066, 0x00
#define Position_window						0x6067, 0x00
#define Position_window_time				0x6068, 0x00
#define Following_error_actual_value		0x60F4, 0x00
#define Control_effort						0x60FA, 0x00
#define Position_control_parameter_set		0x60FB, 0x00
#define Position_demand_value_inc			0x60FC, 0x00

/*****Interpolate position mode ******/
#define Interpolation_sub_mode_select		0x60C0, 0x00
#define Interpolation_data_record_0			0x60C1, 0x01
#define ITP_ip_time_units					0x60C2, 0x01	// ITP : Interpolation time period
#define ITP_ip_time_index					0x60C2, 0x02
#define ISD_Synchronize_on_group		 	0x60C3, 0x01 	// ISD : Interpolation sync definition
#define ISD_ip_sync_every_n_event			0x60C3, 0x02
#define IDC_Maxinum_buffer_size			 	0x60C4, 0x01    // IDC : Interpolation data configuration
#define IDC_Actual_buffer_size				0x60C4, 0x02
#define IDC_Buffer_organization				0x60C4, 0x03
#define IDC_Buffer_position					0x60C4, 0x04
#define IDC_Size_of_data_record				0x60C4, 0x05
#define IDC_Buffer_clear					0x60C4, 0x06

/******* Profile velocity mode*******/
#define Velocity_sensor_actual_value 		0x6069, 0x00
#define Sensor_selection_code				0x606A, 0x00
#define Velocity_demand_value				0x606B, 0x00
#define Velocity_actual_value 				0x606C, 0x00
#define Velocity_window						0x606D, 0x00
#define Velocity_window_time 				0x606E, 0x00
#define Velocity_threshold					0x606F, 0x00
#define Velocity_threshold_time				0x6070, 0x00
#define Target_velocity						0x60FF, 0x00
#define Max_slippage						0x60F8, 0x00
#define VCPS_Gain							0x60F9, 0x01  	// VCPS : Velocity control parameter set
#define VCPS_integration_time_constant		0x60F9, 0x02

/***** Profile torque mode*******/
#define Target_torque						0x6071, 0x0
#define Max_torque							0x6072, 0x0
#define Max_current							0x6073, 0x0
#define Torque_demand_value					0x6074, 0x0
#define Motor_rated_currect					0x6075, 0x0
#define Motor_reted_torque					0x6076, 0x0
#define Torque_actual_value					0x6077, 0x0
#define Current_actual_value				0x6078, 0x0
#define DC_link_circuit_voltage 			0x6079, 0x0
#define Torque_slope						0x6087, 0x0
#define Torque_profile_type					0x6088, 0x00
#define Power_stage_parameters 				0x60F7, 0x01
#define Torque_control_parameters 			0x60F6, 0x01

/******Velocity mode*******/
#define vl_target_velocity					0x6042, 0x0
#define vl_velocity_demand					0x6043, 0x0
#define vl_control_effort					0x6044, 0x0
#define vl_manipulated_velocity				0x6045, 0x0
#define VV_min_amount						0x6046, 0x01 		//VV : Vl velocity
#define VV_max_amount						0x6046, 0x02
#define VV_min_pos							0x6047, 0x01
#define VV_max_pos							0x6047, 0x02
#define VV_min_neg							0x6047, 0x03
#define VV_max_neg							0x6047, 0x04
#define VVA_Delta_speed					 	0x6048, 0x01	// VVA : vl velocity acceleration
#define VVA_Delta_time						0x6048, 0x02
#define VVD_Delta_speed					 	0x6049, 0x01	// VVD : vl velocity deceleration
#define VVD_Delta_time					 	0x6049, 0x02
#define VVQS_Delta_speed					0x604A, 0x01 		// VVQS : vl velocity quick stop
#define VVQS_Delta_time						0x604A, 0x02
#define VSPF_numerator					 	0x604B, 0x01	// VSPF : vl set-point factor
#define VSPF_denominator					0x604B, 0x02
#define VDF_numerator						0x604C, 0x01 		// VDF : vl dimension factor
#define VDF_denominator						0x604C, 0x02
#define vl_pole_number					 	0x604D, 0x0
#define vl_velocity_reference				0x604E, 0x0
#define vl_ramp_function_time				0x604F, 0x0
#define vl_slow_down_time					0x6050, 0x0
#define vl_quick_stop_time					0x6051, 0x0
#define vl_nominal_percentage				0x6052, 0x0
#define vl_percentage_demand				0x6053, 0x0
#define vl_actual_percentage				0x6054, 0x0
#define vl_manipulated_percentage			0x6055, 0x0
#define VVMA_min							0x6056, 0x01 		// VVMA : vl velocity motor amount
#define VVMA_max							0x6056, 0x02
#define VVM_min_pos						 	0x6057, 0x01	// VVM	: vl velocity motor
#define VVM_max_pos							0x6057, 0x02
#define VVM_min_neg							0x6057, 0x03
#define VVM_max_neg							0x6057, 0x04
#define VFMA_min							0x6058, 0x01 		// VFMA : vl frequency motor amount
#define VFMA_max							0x6058, 0x02
#define VFM_min_pos						 	0x6059,	0x01    // VFM	: vl frequency motor
#define VFM_max_pos							0x6059, 0x02
#define VFM_min_neg							0x6059, 0x03
#define VFM_max_neg							0x6059, 0x04

#define LEN_STR(a) (sizeof(a) /sizeof(a[0]))
// Object 1000h: Device type
#define get_device_profile_number(v) ((v) & 0xFFFF)
static char *device_type_str[] __attribute__((unused)) = {
		"Frequency converter", "Servo drive", "Stepper motor",
		"Multiple device module", "None" };
typedef enum {
	dt_Frequency_converter = 0,
	dt_Servo_drive,
	dt_Stepper_motor,
	dt_Multiple_device_module,
	dt_None
} device_type;

device_type get_device_type(uint32_t v);

#define get_device_type_str(v) device_type_str[(v)]

//Object 60FDh: Digital inputs
#define is_set_negative_limit_switch(v) (v) & 0x01 ? 1 : 0
#define is_set_positive_limit_switch(v) (v) & 0x02 ? 1 : 0
#define is_set_home_switch(v) 			(v) & 0x04 ? 1 : 0
#define is_set_interlock(v) 			(v) & 0x08 ? 1 : 0

//Object 6007h: Abort connection option code
static char *abort_connection_option_code_str[] __attribute__((unused))
		= { "no_action", "malfunction", "disable_voltage", "quick_stop",
				"reserved" };
typedef enum {
	acoc_no_action = 0,
	acoc_malfunction,
	acoc_disable_voltage,
	acoc_quick_stop,
	acoc_None
} abort_connection_option_code_type;

#define  get_abort_connection_option_code_type(v) (v) < LEN_STR(abort_connection_option_code_str) ? v : LEN_STR(abort_connection_option_code_str) - 1
#define get_device_type_str(v) device_type_str[(v)]

//Object 6402h: Motor type
static char *motor_type_str[] __attribute__((unused)) = { "Non-standard motor",
		"Phase modulated DC motor", "Frequency controlled DC motor",
		"PM synchronous motor", "FC synchronous motor",
		"Switched reluctance motor", "Wound rotor induction motor",
		"Squirrel cage induction motor", "Stepper motor",
		"Micro-step stepper motor", "Sinusoidal PM BL motor",
		"Trapezoidal PM BL motor", "reserved", "manufacturer specific",
		"reserved" };
#define get_motor_type_str(v) motor_type_str[(v) < LEN_STR(motor_type_str) ? v : LEN_STR(motor_type_str) - 1]

//Object 6502h: Supported drive modes

//typedef enum
//{
//	pp = 0,
//	vl = 1,
//	pv = 2,
//	tq = 3,
//	hm = 5,
//	ip = 6,
//	csp = 7,
//	csv = 8,
//	cst = 9
//
//}operation_mode_support_type;

static char *operation_mode_support_str[] __attribute__((unused)) = {
		"pp - Profile Position Mode", "vl   Velocity Mode",
		"pv - Profile velocity Mode", "tq - Profile Torque Mode", "reserved",
		"hm - Homing Mode", "ip - Interpolated Position Mode",
		"csp - Cyclic sync Position Mode", "csv - Cyclic sync Velocity Mode",
		"cst - Cyclic sync Torque Mode" };
#define is_support(type, v) (v) & (0x01 << (type)) ? 1 : 0
#define is_suppot_pp(v)  (v) & 0x01 ? 1 : 0
#define is_suppot_vl(v)  (v) & 0x02 ? 1 : 0
#define is_suppot_pv(v)  (v) & 0x04 ? 1 : 0
#define is_suppot_tq(v)  (v) & 0x08 ? 1 : 0
#define is_suppot_hm(v)  (v) & 0x20 ? 1 : 0
#define is_suppot_ip(v)  (v) & 0x40 ? 1 : 0
#define is_suppot_csp(v)  (v) & 0x80 ? 1 : 0
#define is_suppot_csv(v)  (v) & 0x100 ? 1 : 0
#define is_suppot_cst(v)  (v) & 0x200 ? 1 : 0

#define get_operation_mode_support_str(type,v)  (is_support((type), (v)) ? operation_mode_support_str[type] : NULL)

//Object 6041h: Statusword
#define is_status_ready_to_switch_on(statusword) 		((statusword) & (0x01 << 0))
#define is_status_switched_on(statusword)				((statusword) & (0x01 << 1))
#define is_status_operation_enabled(statusword)			((statusword) & (0x01 << 2))
#define is_status_fault(statusword)			((statusword) & (0x01 << 3))
#define is_status_voltage_enabled(statusword)			((statusword) & (0x01 << 4))
#define is_status_quick_stop(statusword)				((statusword) & (0x01 << 5))
#define is_status_switch_on_disabled(statusword)		((statusword) & (0x01 << 6))
#define is_status_warning(statusword)					((statusword) & (0x01 << 7))
#define is_status_manufacturer_specific(statusword)		((statusword) & (0x01 << 8))
#define is_status_remote(statusword)					((statusword) & (0x01 << 9))
#define is_status_target_reached(statusword)			((statusword) & (0x01 << 10))
#define is_status_internal_limit_active(statusword)		((statusword) & (0x01 << 11))
#define is_status_set_point_acknowledge(statusword)		((statusword) & (0x01 << 12))
#define is_status_following_error(statusword)			((statusword) & (0x01 << 13))
#define is_status_speed(statusword)						((statusword) & (0x01 << 12))
#define is_status_max_slippage_error(statusword)		((statusword) & (0x01 << 13))
#define is_status_homing_attained(statusword)			((statusword) & (0x01 << 12))
#define is_status_homing_error(statusword)				((statusword) & (0x01 << 13))
#define is_status_ip_mode_active(statusword)			((statusword) & (0x01 << 12))

#define contrlword_shutdown(c) 		(((c) | 0x6) & ~0x81)
#define contrlword_switch_on(c) 		(((c) | 0x7) & ~0x88)
#define contrlword_disable_voltage(c) 		((c)  & ~0x82)
#define contrlword_quick_stop(c) 		(((c) | 0x2) & ~0x84)
#define contrlword_disable_operation(c) 	(((c) | 0x7) & ~0x88)
#define contrlword_enable_operation(c) 	(((c) | 0xF) & ~0x80)
#define contrlword_fault_reset(c)		((c) | 0x80)
#define contrlword_new_set_point(c)		((c) | 0x10)
#define contrlword_new_set_point_imm(c)		((c) | 0x30)

typedef enum {

	no_ready_to_switch_on = 0,
	/* Low level power(e.g. +/- 15V, 5V) has been applied to the drive.
	 *  The drive is being initialized or is running self test.
	 *  A brake, if present, has to be applied in this state.
	 *  The drive function is disable.
	 * */

	switch_on_disable,
	/* Drive initialization is complete.
	 * The drive parameters have been set up.
	 * Drive parameters may be changed.
	 * High voltage may not be applied to the dirve.
	 * The drive function is disabled.
	 * */

	ready_to_switch_on,
	/* High voltage may be applied to the drive.
	 * The drive parameters may be changed.
	 * The drive function is disabled.
	 * */

	switched_on,
	/* High voltage has been applied to the drive.
	 * The power amplifier is ready.
	 * The dirve parameters may be change.
	 * The drive functuion is disable.
	 * */
	operation_enable,
	/* No faults have been detected.
	 * The dirve  function is enabled and power is apllied to the motor.
	 * The dirve function is enable.
	 * */

	quick_stop_active,
	/* The drive paramters may be changed.
	 * The quick stop function is being executed.
	 * The drive function is enabled and power is applied to the motor.
	 * */

	fault_reaction_active,
	/* The parameters may be changed.
	 * A fault has occurred in the drive.
	 * The quick stop functon is being executed.
	 * The drive function is enabled and power is applied to the motor.
	 * */

	fault,
	/* The drive parameters may be changed.
	 * A fault has occurred in the drive.
	 * High voltage switch-on/off depends on the application.
	 * The drive function is disabled.
	 * */
	none
} axle_state;
axle_state get_axle_state(uint16_t status_word);

static char *axle_device_state_str[] __attribute__((unused)) = {
		"no_ready_to_switch_on", "switch_on_disable", "ready_to_switch_on",
		"switched_on", "operation_enable", "quick_stop_active",
		"fault_reaction_active", "fault", "none" };
#define axle_state_to_str(state) axle_device_state_str[(state)]

//Object 605Bh: Shutdown option code

typedef enum {
	soc_disable_drive_function = 0, soc_Slow_down_with_slow_down_ramp = 1
} shutdown_option_code;

//Object 605Ch: Disable operation option code
typedef enum {
	dooc_disable_drive_function = 0, dooc_Slow_down_with_slow_down_ramp = 1
} disable_operation_option_code;

//Object 605Ah: Quick stop option code
typedef enum {
	qsoc_disable_drive_function = 0,
	qsoc_Slow_down_with_slow_down_ramp = 1,
	qsoc_slow_down_on_slow_down_ramp = 2,
	qsoc_slow_down_on_quick_stop_ramp = 3,
	qsoc_slow_down_on_the_current_limit = 4,
	qsoc_slow_down_on_the_voltage_limit = 5,
	qsoc_slow_down_on_slow_down_ramp_and_stay_in_QUICK_STOP = 6,
	qsoc_slow_down_on_quick_stop_ramp_and_stay_in_QUICK_STOP = 7,
	qsoc_slow_down_on_the_current_limit_and_stay_in_QUICK_STOP = 8,
	qsoc_slow_down_on_the_voltage_limit_and_stay_in_QUICK_STOP = 9

} quick_stop_option_code;

//Object 605Dh: Halt option code

typedef enum {
	hoc_disable_drive_motor_is_free_to_rotate = 0,
	hoc_slow_down_on_slow_down_ramp,
	hoc_slow_down_on_quick_stop_ramp,
	hoc_slow_down_on_the_current_limit,
	hoc_slow_down_on_the_voltage_limit
} halt_option_code;

//Object 605Eh: Fault reaction option code
typedef enum {
	froc_disable_drive_motor_is_free_to_rotate = 0,
	froc_disableslow_down_on_slow_down_ramp,
	froc_disableslow_down_on_quick_stop_ramp,
	froc_disableslow_down_on_the_current_limit,
	froc_disableslow_down_on_the_voltage_limit
} fault_reaction_option_code;

//Object 6061h: Modes of operation
static char *mode_of_operation_str[] __attribute__((unused)) = {
		"No mode assigned", "Profile Position Mode", "Velocity Mode",
		"Profile Velocity Mode", "Torque Profile Mode", "Reserved",
		"Homing Mode", "Interpolated Position Mode",
		"Cyclic sync Position Mode", "Cyclic sync Velocity Mode",
		"Cyclic sync Torque Mode", "None" };

typedef enum {
	op_mode_no = 0,
	op_mode_pp = 1,
	op_mode_vl = 2,
	op_mode_pv = 3,
	op_mode_hm = 6,
	op_mode_ip = 7,
	op_mode_csp = 8,
	op_mode_csv = 9,
	op_mode_cst = 10
} mode_of_operation;
#define get_mode_of_operation_str(v) mode_of_operation_str[(v) < LEN_STR(mode_of_operation_str) ? (v) : LEN_STR(mode_of_operation_str) - 1]

#endif /* DS402_H_ */
