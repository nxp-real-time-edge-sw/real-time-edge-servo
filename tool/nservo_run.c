// SPDX-License-Identifier: (GPL-2.0 OR MIT)
/*
 * Copyright 2018-2020 NXP
 */

#include <stdio.h>
#include <unistd.h>	 //STDIN_FILENO
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include "nservo.h"
#include "nser_pdo.h"
#include <pthread.h>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/poll.h>
#include <errno.h>
#include <getopt.h>
#include <math.h>
#define MAX 80
#define PORT 6000
#define SA struct sockaddr

int running = 1;
void signal_handler(int sig) {
	running = 0;
}

struct axis_pp_status_t {
	int32_t target_position;
	int32_t profile_velocity;
	int status;
};

struct axis_pv_status_t {
	int32_t target_velocity;
	int status;
};

struct trajectory_planner_t
{
	uint32_t points_num;
	uint32_t current_point;
	double origin_pos;
	double target_pos;
	double uniform_speed;
	double accel_pos;
	double decel_pos;
	uint32_t point_accel_end;
	uint32_t point_decel_start;
	int32_t direction;
};

typedef enum {
	csp_status_stop = 0,
	csp_status_ready = 1,
	csp_status_running = 2,
	csp_status_pre_stop = 3,
} csp_status_t;


struct axis_csp_status_t {
	int32_t bias;
	uint32_t tp_num;
	uint32_t is_cyclic;
	uint32_t current_tp;
	double acceleration;
	double deceleration;
	double max_speed;
	csp_status_t csp_status;
	struct trajectory_planner_t *tp;
};

struct axis_status_t {
	uint8_t mode;
	int32_t current_position;
	int32_t current_velocity;
	uint32_t scale;
	void * axis_status;
};

/* API interface for CSP mode*/
#define skip_blank(p) while ((*(p)) == ' ') (p) += 1
inline static int skip_semicolon(char **str)
{
	char *p = strchr((char *)*str, ';');
	if (!p) {
		printf("Each instruction must end with ';'\r\n");
		return -1;
	}
	*str = p + 1;
	return 0;
}

static int load_parameter(char **str, char *parameter_name, uint32_t *parameter)
{
	int len = strlen(parameter_name);
	int n;
	char *s = *str;
	if (strncmp(parameter_name, s, len) == 0) {
		s += len;
		n = sscanf(s, "=%d", parameter);
		if (n != 1) {
			printf("The parameter \"%s\" can not be assigned\r\n", parameter_name);
			return -1;
		}
		if (skip_semicolon(str) < 0) {
			return -1;
		}
		return 1;
	}
	return 0;

}

static uint32_t get_next_pos(struct axis_status_t *axis_status)
{
	struct axis_csp_status_t *status = axis_status->axis_status;
	struct trajectory_planner_t *tp = &status->tp[status->current_tp];
	double value;
	uint32_t ret;
	uint32_t index = tp->current_point;
	if (index <  tp->point_accel_end)
		value = 0.5 * status->acceleration * index * index;
	else if (index > tp->point_decel_start)
		value = tp->decel_pos + 0.5 * status->deceleration * pow((index - tp->point_decel_start), 2);
	else
		value = tp->accel_pos + tp->uniform_speed * ( index - tp->point_accel_end);
	value *= tp->direction;
	value += tp->origin_pos;
	value *= axis_status->scale;
	ret = (uint32_t)value;
	tp->current_point++;
	if (tp->current_point < tp->points_num) {
		return ret;
	}

	tp->current_point = 0;
	status->current_tp++;
	if (status->current_tp < status->tp_num) {
		return ret;
	}

	if (status->is_cyclic && status->csp_status != csp_status_pre_stop) {
		status->current_tp = 1;
	} else {
		status->csp_status = csp_status_stop;
		status->current_tp = 0;
	}
	return ret;
}

static int trajectory_planner(struct axis_status_t *axis_status, struct trajectory_planner_t *tp)
{
	struct axis_csp_status_t *status = axis_status->axis_status;
	double delta_pos = tp->target_pos - tp->origin_pos;
	double accel = status->acceleration;
	double decel = status->deceleration;
	double point_num = tp->points_num;
	double a0 = 0.5*(accel + accel * accel / decel);
	double b0 = -accel*point_num;
	double c0;
	tp->direction = 1;
	if (delta_pos < 0) {
		tp->direction = -1;
	}
	delta_pos *= tp->direction;
	c0 = delta_pos;
	double delta = b0 * b0 - 4 * a0 * c0;
	if ( delta < 0 ) {
		return -1;
	}
	double t0 = (-1 * b0 - sqrt(delta)) / 2 / a0;
	double t1 = point_num - accel * t0 / decel;
	if (t0 * accel > status->max_speed) {
		return -2;
	}
	tp->accel_pos = 0.5 * accel * t0 * t0;
	tp->decel_pos = tp->accel_pos + accel * t0 * (t1 - t0);
	tp->point_accel_end = (uint32_t)(t0 + 0.5);
	tp->point_decel_start = (uint32_t)(t1 + 0.5);
	tp->uniform_speed = accel * t0;
	return 0;
}

static int Tp_arrays_paster(struct axis_status_t *axis_status, char *tp_str, uint32_t period_ns)
{
	struct axis_csp_status_t *status = axis_status->axis_status;
	uint32_t next_pos, time;
	int n = 0, i = 0;
	char *p = tp_str;
	char *tp_arrays = NULL;
	int ret;
	uint32_t value;
	double origin_pos = (double)axis_status->current_position / axis_status->scale;
	if (axis_status->mode != op_mode_csp) {
		return -1;
	}

	if (status->csp_status == csp_status_running) {
		return -2;
	}

	while (*p != '\0') {
		skip_blank(p);
		ret = load_parameter(&p, "Cyclic", &value);
		if (ret < 0)
			return -5;
		else if ( ret == 1) {
			status->is_cyclic = value > 0 ? 1 : 0;
			continue;
		}

		ret = load_parameter(&p, "Scale", &value);
		if (ret < 0)
			return -5;
		else if ( ret == 1) {
			axis_status->scale = value;
			continue;
		}

		ret = load_parameter(&p, "Bias", &value);
		if (ret < 0)
			return -1;
		else if ( ret == 1) {
			status->bias = value;
			continue;
		}

		ret = load_parameter(&p, "Accel", &value);
		if (ret < 0)
			return -1;
		else if ( ret == 1) {
			status->acceleration = value;
			continue;
		}

		ret = load_parameter(&p, "Decel", &value);
		if (ret < 0)
			return -1;
		else if ( ret == 1) {
			status->deceleration = value;
			continue;
		}

		ret = load_parameter(&p, "Max_speed", &value);
		if (ret < 0)
			return -1;
		else if ( ret == 1) {
			status->max_speed = value;
			continue;
		}

		if (strncmp("TpArrays", p, 8) == 0) {
		p += 8;
		tp_arrays = strchr(p, '[');
		if (!tp_arrays) {
			return -5;
		}
		char *pp = strchr(p, ']');
		if (!pp) {
			return -5;
		}
		tp_arrays += 1; // skip '['
			*pp = '\0';
			pp = strchr(pp + 1, ';');
			if (!pp) {
				return -5;
			}
			p = pp + 1;
			continue;
		}
		p++;
	}
	p = tp_arrays;
	while (1) {
		if (p == NULL) {
			break;
		}
		if ((p = strchr(p, '(')) == NULL)
			break;
		n++;
		p++;
	}
	if (n == 0) {
		return -3;
	}

	if (status->is_cyclic) {
		n += 1;
	}

	if (!(status->tp = calloc(sizeof(struct  trajectory_planner_t),  n))) {
		return -4;
	}

	status->tp_num = n;

	p = tp_arrays;
	while (1) {
		n = sscanf(p, "(%d:%d)", &next_pos, &time);
		if (n != 2)
			return -5;
		status->tp[i].points_num = time * 1000000 / period_ns;
		status->tp[i].current_point = 0;
		status->tp[i].origin_pos = origin_pos;
		status->tp[i].target_pos = next_pos;
		trajectory_planner(axis_status, status->tp + i);
		i++;
		origin_pos = next_pos;
		if ((p = strchr(p, ',')) == NULL)
			break;
		p++;
	}
	if (status->is_cyclic) {
		n = sscanf(tp_arrays, "(%d:%d)", &next_pos, &time);
		if (n != 2)
			return -5;
		status->tp[i].points_num = time * 1000000 / period_ns;
		status->tp[i].current_point = 0;
		status->tp[i].origin_pos = origin_pos;
		status->tp[i].target_pos = next_pos;
		trajectory_planner(axis_status, status->tp + i);
	}
	status->csp_status = csp_status_ready;
	return 0;
}
/* The end of API interface for CSP mode*/

struct axis_status_t  *axis_status;

static int axis_start(nser_axle *ns_axis, uint16_t status, uint8_t mode) {
	int ret = 0;
	axle_state s = get_axle_state(status);
	if (status & 0x8) {
		nser_pdo_set_Controlword(ns_axis, 0x80);
		return 0;
	}

	switch (s) {
	case (no_ready_to_switch_on):
	case (switch_on_disable):
		nser_pdo_set_Controlword(ns_axis, contrlword_shutdown(0));
		break;
	case (ready_to_switch_on):
		nser_pdo_set_Controlword(ns_axis, contrlword_switch_on(0));
		break;
	case (switched_on):
		nser_pdo_set_Controlword(ns_axis, contrlword_enable_operation(0));
		nser_pdo_set_Modes_of_operation(ns_axis, mode);
		break;
	case (operation_enable):
		ret = 1;
		break;
	case (quick_stop_active):
	case (fault_reaction_active):
	case (fault):
	default:
		ret = -1;
	}
	return ret;
}

static int pp_update_target_position(nser_axle *ns_axis, struct axis_status_t *axis_status)
{
	uint16_t statusword = 0;
	uint16_t controlword = 0;
	struct axis_pp_status_t *status = axis_status->axis_status;
	controlword = nser_pdo_get_Controlword(ns_axis);
	statusword = nser_pdo_get_Statusword(ns_axis);

	if (axis_start(ns_axis, statusword, op_mode_pp) != 1) {
		return 0;
	}

	if (ns_axis->nser_master->ns_master_state == ALL_OP) {
		axis_status->current_velocity = nser_pdo_get_Velocity_actual_value(ns_axis);
		axis_status->current_position = nser_pdo_get_Position_actual_value(ns_axis);
		if (status->status == 0) {
			status->target_position = axis_status->current_position;
			nser_pdo_set_Profile_velocity(ns_axis,
				status->profile_velocity);
			nser_pdo_set_Target_position(ns_axis,
				status->target_position);
			nser_pdo_set_Controlword(ns_axis, 0xf);
			if (status->profile_velocity != 0) {
				status->status = 1;
			}
		} else if (status->status > 0  && status->status < 20) {
			nser_pdo_set_Target_position(ns_axis,
				status->target_position);
			nser_pdo_set_Profile_velocity(ns_axis,
				status->profile_velocity);
			status->status++;
		}
		else if (status->status >= 20 && status->status < 30 ) {
			nser_pdo_set_Controlword(ns_axis,
			contrlword_new_set_point(controlword));
			status->status++;
		}
		else if (status->status == 30) {
			if (is_status_target_reached(statusword)) {
				status->status = 1;
			}
			nser_pdo_set_Controlword(ns_axis, 0xf);
		}
	}
	return 0;
}

static int csp_update_target_position(nser_axle *ns_axis, struct axis_status_t *axis_status)
{
	uint16_t statusword = nser_pdo_get_Statusword(ns_axis);
	struct axis_csp_status_t *status = axis_status->axis_status;

	if (axis_start(ns_axis, statusword, op_mode_csp) != 1) {
		return 0;
	}

	if (ns_axis->nser_master->ns_master_state == ALL_OP) {

		axis_status->current_velocity = nser_pdo_get_Velocity_actual_value(ns_axis);
		axis_status->current_position = nser_pdo_get_Position_actual_value(ns_axis);

		if (status->csp_status == csp_status_running || status->csp_status == csp_status_pre_stop) {
			nser_pdo_set_Target_position(ns_axis, get_next_pos(axis_status));
		}
	}
	return 0;
}

static int pv_update_target_velocity(nser_axle *ns_axis, struct axis_status_t *axis_status)
{
	uint16_t statusword = nser_pdo_get_Statusword(ns_axis);
	struct axis_pv_status_t *status = axis_status->axis_status;

	if (axis_start(ns_axis, statusword, op_mode_pv) != 1) {
		return 0;
	}

	if (ns_axis->nser_master->ns_master_state == ALL_OP) {

		axis_status->current_velocity = nser_pdo_get_Velocity_actual_value(ns_axis);
		axis_status->current_position = nser_pdo_get_Position_actual_value(ns_axis);

		if (!status->status) {
			status->target_velocity = axis_status->current_velocity;
			status->status = 1;
		}

		nser_pdo_set_Target_velocity(ns_axis, status->target_velocity);
	}
	return 0;
}

int main_task(nser_global_data *ns_data)
{
	int i;
	nser_axle *ns_axis;
	for (i = 0; i < ns_data->axle_number; i++) {
		ns_axis = &ns_data->ns_axles[i];
		if (ns_axis->mode == op_mode_pv)
			pv_update_target_velocity(ns_axis, &axis_status[i]);
		else if (ns_axis->mode == op_mode_pp)
			pp_update_target_position(ns_axis, &axis_status[i]);
		else if (ns_axis->mode == op_mode_csp)
			csp_update_target_position(ns_axis, &axis_status[i]);
	}	
	return ns_data->running;
}

int command_parse(char *command, nser_global_data *ns_data, struct axis_status_t *axis_status, int sock)
{
	char *p_colon = NULL;
	char *s_colon = NULL;
	int32_t value = 0;
	char buf[256] = {'\0'};
	int axis = 0;
	int len = 0;
	int i;
	FILE *stream = NULL;
	char *line = NULL;
	int l;
	int ret;
	size_t lm = 0;
	struct axis_pp_status_t *pp_status;
	struct axis_pv_status_t *pv_status;
	struct axis_csp_status_t *csp_status;

	if (strcmp("exit", command) == 0) {
		send(sock, "exit..\n", 7, 0);
		return -1;	
	}

	if (strcmp(command, "set_start_all") == 0) {
		struct axis_csp_status_t *status;
		for (i = 0; i < ns_data->axle_number; i++) {
			if (axis_status[i].mode == op_mode_csp) {
				status = axis_status[i].axis_status;
				if (status->csp_status == csp_status_ready) {
					status->csp_status = csp_status_running;
				}
			}
		}
		len = snprintf(buf, 256, "All CSP axis in ready status start to run \n");
		goto send;
	}
	else if (strcmp(command, "set_stop_all") == 0) {
		struct axis_csp_status_t *status;
		for (i = 0; i < ns_data->axle_number; i++) {
			if (axis_status[i].mode == op_mode_csp) {
				status = axis_status[i].axis_status;
				if (status->csp_status == csp_status_running) {
					status->csp_status = csp_status_pre_stop;
				}
			}
		}
		len = snprintf(buf, 256, "All CSP axis in running status start to stop \n");
		goto send;
	}
	else if (strncmp(command, "load_tp_file", 12) == 0) {
		if ((p_colon = strchr(command, ':')) == NULL) {
			len = snprintf(buf, 256, "invalid cmd: %s\n", command);
		goto send;
		}
		p_colon++;
		stream = fopen(p_colon, "r");
		if (stream == NULL) {
			len = snprintf(buf, 256, "fopen (%s) failed\r\n", p_colon);
			goto send;
		}
		while ((l = getline(&line, &lm, stream)) != -1) {
			if (sscanf(line, "Axis=%d", &axis) != 1) {
				len = snprintf(buf, 256, "Read \"Axis\" label from fp file \"%s\" failed\r\n", p_colon);
				free(line);
				goto send;
			}
			if ((s_colon = strchr(line, ';')) == NULL){
				len = snprintf(buf, 256, "invalid line from fp file \"%s\" : %s\n", p_colon, line);
				free(line);
				goto send;
			}
			if (axis >= ns_data->axle_number) {
				len = snprintf(buf, 256, "invalid \"Axis\" label from fp file \"%s\": %s\n", p_colon, line);
				free(line);
				goto send;
			}

			ret = Tp_arrays_paster(&axis_status[axis], s_colon + 1, ns_data->period_time);	
			if (ret == -1) {
				len = snprintf(buf, 256, "invalid \"Axis\" label from fp file \"%s\": %s\n", p_colon, line);
			} else if (ret == -2) {
				len = snprintf(buf, 256, "Axis %d is running, please stop it first: %s\n", axis, line);
			} else if (ret == -3) {
				len = snprintf(buf, 256, "Axis %d: No Tp array be specificed by \"TpArrays=\"\n", axis);
			} else if ( ret == -4) {
				len = snprintf(buf, 256, "Axis %d: Failed to malloc memory  for trajectory_planner\n", axis);
			} else if ( ret == -5) {
				len = snprintf(buf, 256, "Axis %d: invalid trajectory array\n", axis);
			} else {
				free(line);
				lm = 0;
			}
			if (ret != 0) {
				free(line);
				goto send;
			}
		}
		len = snprintf(buf, 256, "trajectory planner file: %s is loaded \n", p_colon);
	} else {
		if ((s_colon = strchr(command, ':')) == NULL){
			len = snprintf(buf, 256, "invalid cmd: %s\n", command);
			goto send;
		}
		if ((p_colon = strchr(s_colon + 1, ':')) != NULL){
			*p_colon = '\0';
			p_colon++;
		}
		*s_colon = '\0';

		axis = atoi(command);

		if (axis >= ns_data->axle_number) {
			len = snprintf(buf, 256, "axis_index is bigger than the real number of axis(%d): %s\n", ns_data->axle_number, command);
			goto send;
		}

		command = s_colon + 1;
		if (strcmp("set_target_velocity", command) == 0){
			if (axis_status[axis].mode == op_mode_pv) {
				if (!p_colon) {
					len = snprintf(buf, 256, "invalid cmd: %s\n", command);
					goto send;
				}
				sscanf(p_colon, "%d", &value);
				pv_status = axis_status[axis].axis_status;
				pv_status->target_velocity = value;
				len = snprintf(buf, 256, "set_target_velocity of the axis %d : %d\n", axis, value);
			} else {
				len = snprintf(buf, 256,  "the axis %d does not work in PV mode.\n",axis);
			}
		}
		else if (strcmp("set_profile_velocity", command) == 0) {
			if (axis_status[axis].mode == op_mode_pp) {
				if (!p_colon) {
					len = snprintf(buf, 256, "invalid cmd: %s\n", command);
					goto send;
				}
				sscanf(p_colon, "%d", &value);
				pp_status = axis_status[axis].axis_status;
				pp_status->profile_velocity  = value;
				len = snprintf(buf, 256, "set_profile_velocity of the axis %d : %d\n", axis, value);
			} else {
				len = snprintf(buf, 256,  "the axis %d does not work in PP mode.\n",axis);
			}
		}
		else if (strcmp("set_target_position", command) == 0) {
			if (axis_status[axis].mode == op_mode_pp) {
				if (!p_colon) {
					len = snprintf(buf, 256, "invalid cmd: %s\n", command);
					goto send;
				}
				sscanf(p_colon, "%d", &value);
				pp_status = axis_status[axis].axis_status;
				pp_status->target_position = value;
				len = snprintf(buf, 256, "set_target_position of the axis %d : %d\n", axis, value);
			} else {
				len = snprintf(buf, 256,  "the axis %d does not work in PP mode.\n",axis);
			}
		}
		else if (strcmp("set_scale", command) == 0) {
			if (!p_colon) {
				len = snprintf(buf, 256, "invalid cmd: %s\n", command);
				goto send;
			}
			sscanf(p_colon, "%d", &value);
			axis_status[axis].scale = value;
			len = snprintf(buf, 256, "set_scale of the axis %d : %d\n", axis, value);
		}
		else if (strcmp("set_tparrays", command) == 0) {
			if (axis_status[axis].mode == op_mode_csp) {
				if (!p_colon) {
					len = snprintf(buf, 256, "invalid cmd: %s\n", command);
					goto send;
				}
				ret = Tp_arrays_paster(&axis_status[axis], p_colon, ns_data->period_time);
				if (ret == -1) {
					len = snprintf(buf, 256, "invalid \"Axis\" label from fp file \"%s\": %s\n", p_colon, line);
				} else if (ret == -2) {
					len = snprintf(buf, 256, "Axis %d is running, please stop it first: %s\n", axis, line);
				} else if (ret == -3) {
					len = snprintf(buf, 256, "Axis %d: No Tp array be specificed by \"TpArrays=\"\n", axis);
				} else if ( ret == -4) {
					len = snprintf(buf, 256, "Axis %d: Failed to malloc memory  for trajectory_planner\n", axis);
				} else if ( ret == -5) {
					len = snprintf(buf, 256, "Axis %d: invalid trajectory array\n", axis);
				} else {
					len = snprintf(buf, 256, "set_tparrays of the axis %d : %s\n", axis, p_colon);
				}
			} else {
				len = snprintf(buf, 256,  "the axis %d does not work in CSP mode.\n",axis);
			}
		}
		else if (strcmp("set_start", command) == 0) {
			if (axis_status[axis].mode == op_mode_csp) {
				csp_status = axis_status[axis].axis_status;
				if (csp_status->csp_status == csp_status_ready) {
					csp_status->csp_status = csp_status_running;
					len = snprintf(buf, 256, "set_start of the axis %d\n", axis);
				}
				len = snprintf(buf, 256, "set_start of the axis %d\n", axis);
			} else {
				len = snprintf(buf, 256,  "the axis %d does not work in CSP mode.\n",axis);
			}
		}
		else if (strcmp("set_stop", command) == 0) {
			if (axis_status[axis].mode == op_mode_csp) {
				csp_status = axis_status[axis].axis_status;
				if (csp_status->csp_status == csp_status_running) {
					csp_status->csp_status = csp_status_pre_stop;
				}
				len = snprintf(buf, 256, "set_stop of the axis %d\n", axis);
			} else {
				len = snprintf(buf, 256,  "the axis %d does not work in CSP mode.\n",axis);
			}
		}
		else if (strcmp("get_current_velocity", command) == 0) {
			len = snprintf(buf, 256, "get_current_velocity of the axis %d : %d\n",axis, axis_status[axis].current_velocity);
		}
		else if (strcmp("get_profile_velocity", command) == 0) {
			if (axis_status[axis].mode == op_mode_pp) {
				pp_status = axis_status[axis].axis_status;
				len = snprintf(buf, 256,  "get_profile_velocity of the axis %d : %d\n",axis, pp_status->profile_velocity);
			} else {
				len = snprintf(buf, 256,  "the axis %d does not work in PP mode.\n",axis);
			}
		}
		else if (strcmp("get_target_velocity", command) == 0) {
			if (axis_status[axis].mode == op_mode_pv) {
				pv_status = axis_status[axis].axis_status;
				len = snprintf(buf, 256,  "get_target_velocity of the axis %d : %d\n",axis, pv_status->target_velocity);
			} else {
				len = snprintf(buf, 256,  "the axis %d does not work in PV mode.\n",axis);
			}
		}
		else if (strcmp("get_target_position", command) == 0) {
			if (axis_status[axis].mode == op_mode_pp) {
				pp_status = axis_status[axis].axis_status;
				len = snprintf(buf, 256,  "get_target_position of the axis %d : %d\n",axis, pp_status->target_position);
			} else {
				len = snprintf(buf, 256,  "the axis %d does not work in PP mode.\n",axis);
			}
		}
		else if (strcmp("get_current_position", command) == 0) {
			len = snprintf(buf, 256, "get_current_position of the axis %d : %d\n",axis, axis_status[axis].current_position);
		}
		else if (strcmp("get_mode", command) == 0) {
			len = snprintf(buf, 256, "get_mode of the axis %d : %s\n",axis, get_mode_of_operation_str(axis_status[axis].mode));
		}
		else {
			len = snprintf(buf, 256, "invalid cmd: %s\n", command);
		}
	}
send:
	send(sock, buf, len, 0);
	if (stream) {
		fclose(stream);
	}
	return 0;
}

const char *usage = "Usage: \n\
  nservo_run -f config.xml [-p port]\n\
	port : Tcp server port\n\
	  defalut value : 6000\n";

int main(int argc, char **argv)
{
	nser_global_data *ns_data;
	signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);
	char buffer[256];
	int sockfd, connfd, len, on = 1, timeout, rc, ret, nfds, nfds_c, i;
	struct sockaddr_in servaddr;
	struct pollfd fds[6];
	int c;
	int service_port = PORT;
	char *conf_file_name = NULL;
	int axis_csp_num, axis_pp_num, axis_pv_num;

//	  daemon(0, 0);
	while ((c = getopt(argc, argv, "f:p:")) != -1) {
		switch (c) {
		case 'p':
			service_port = atoi(optarg);
			 break;
		case 'f':
			if (conf_file_name != NULL){
				free(conf_file_name);
			}
				conf_file_name = strdup(optarg);
				break;
		default:
			printf("%s", usage);
			if (conf_file_name != NULL){
				free(conf_file_name);
			}
			return -1;
		}
	}

	if (!conf_file_name) {
		printf("Please set xml config file using '-f' \n");
		printf("%s", usage);
		return -1;
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	} else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(service_port);

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*) &on, sizeof(on))
			< 0) {
		perror("setsockopt() failed");
		goto close_tcp;

	}

	if (ioctl(sockfd, FIONBIO, (char*) &on)) {
		perror("ioctl() failed");
		goto close_tcp;
	}

	// Binding newly created socket to given IP and verification
	if ((bind(sockfd, (SA*) &servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed...\n");
		goto close_tcp;
	}
	
	printf("Socket successfully binded..\n");

	// Now server is ready to listen and verification
	if ((listen(sockfd, 5)) != 0) {
		printf("Listen failed...\n");
		goto close_tcp;
	}

	printf("Server listening..\n");

	memset(fds, 0, sizeof(fds));
	fds[0].fd = sockfd;
	fds[0].events = POLLIN;
	timeout = 10000;


	if (!(ns_data = nser_app_run_init(conf_file_name))) {
		fprintf(stderr, "Failed to initialize the App\n");
		goto close_tcp;
	}
		
	if (!(axis_status = calloc(sizeof(struct axis_status_t),  ns_data->axle_number))) {
		fprintf(stderr, "Failed to malloc memory  for axis_status\n");
		goto destroy_master;
	}

	axis_csp_num = 0;
	axis_pp_num = 0;
	axis_pv_num = 0;
	for (i = 0; i < ns_data->axle_number; i++)
	{
		if ( ns_data->ns_axles[i].mode == op_mode_csp ) {
			axis_csp_num += 1;
		}
		else if ( ns_data->ns_axles[i].mode == op_mode_pp ) {
			axis_pp_num += 1;
		}
		else if ( ns_data->ns_axles[i].mode == op_mode_pv ) {
			axis_pv_num += 1;
		}
		axis_status[i].mode = ns_data->ns_axles[i].mode;
	}

	void *p_csp = NULL;
	void *p_pp = NULL;
	void *p_pv = NULL;
	if (!(p_csp = calloc(sizeof(struct axis_csp_status_t),  axis_csp_num))) {
		fprintf(stderr, "Failed to malloc memory  for axle_csp_status\n");
		goto destroy_master;
	}

	axis_csp_num = 0;
	for (i = 0; i < ns_data->axle_number; i++)
	{
		if ( axis_status[i].mode == op_mode_csp ) {
			axis_status[i].axis_status = &((struct axis_csp_status_t*)p_csp)[axis_csp_num++];
		}
	}

	if (!(p_pp = calloc(sizeof(struct axis_pp_status_t),  axis_pp_num))) {
		fprintf(stderr, "Failed to malloc memory  for axle_pp_status\n");
		goto destroy_master;
	}

	axis_pp_num = 0;
	for (i = 0; i < ns_data->axle_number; i++)
	{
		if ( axis_status[i].mode == op_mode_pp ) {
			axis_status[i].axis_status = &((struct axis_pp_status_t*)p_pp)[axis_pp_num++];
		}
	}

	if (!(p_pv = calloc(sizeof(struct axis_pv_status_t),  axis_pv_num))) {
		fprintf(stderr, "Failed to malloc memory  for axle_pv_status\n");
		goto destroy_master;
	}

	axis_pv_num = 0;
	for (i = 0; i < ns_data->axle_number; i++)
	{
		if ( axis_status[i].mode == op_mode_pv ) {
			axis_status[i].axis_status = &((struct axis_pp_status_t*)p_pv)[axis_pv_num++];
		}
	}

	if (user_cycle_task_start(ns_data, main_task, 0)) {
		fprintf(stderr, "Failed to start task\n");
		goto destroy_master;		
	}

	nfds = 1;
	len = 0;
	while (running) {
		rc = poll(fds, nfds, timeout);
		if (rc < 0) {
			perror("  poll() failed");
			running = 0;
			break;
		}
		if (rc > 0) {
			nfds_c = nfds;
			for (i = 0; i < nfds_c; i++) {
				if (fds[i].revents == 0)
					continue;
				if (fds[i].revents != POLLIN) {
					printf("  Error! revents = %d\n", fds[i].revents);
					running = 0;
					break;
				}
				if (fds[i].fd == sockfd) {
					connfd = accept(sockfd, NULL, NULL);
					if (connfd < 0) {
						if (errno != EWOULDBLOCK) {
							perror("  accept() failed");
							running = 0;
						}
						continue;
					}
					fds[nfds].fd = connfd;
					fds[nfds].events = POLLIN;
					nfds = 2;

				} else {
					ret = recv(fds[i].fd, buffer + len, sizeof(buffer) - len, 0);
					if (ret > 0) {
						len += ret;
						if (buffer[len - 1] == '\n') {
							if (len > 1 && buffer[len -2] == '\r')
								buffer[len - 2] = '\0';
							else
								buffer[len - 1] = '\0';
							if (command_parse(buffer, ns_data, axis_status, fds[i].fd) < 0) {
								running = 0;
								close(fds[0].fd);
								close(fds[1].fd);
							}
							len = 0;
						}
					} else {
						if (ret < 0 && errno == EWOULDBLOCK) {
							continue;
						}
						close(fds[i].fd);
						fds[i].fd = -1;
						nfds = 1;
					}
				}
			}
		}
	}

	user_cycle_task_stop(ns_data);
	
destroy_master:
	nser_deactivate_all_masters(ns_data);
	if (p_csp) {
		free(p_csp);
	}
	if (p_pp) {
                free(p_pp);
        }
	if (p_pv) {
                free(p_pv);
        }

close_tcp:
	close(sockfd);
	free(ns_data);
	free(conf_file_name);
}
