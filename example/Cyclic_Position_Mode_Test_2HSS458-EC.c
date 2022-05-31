// SPDX-License-Identifier: (GPL-2.0 OR MIT)
/*
 * Copyright 2018-2020 NXP
 */

#include <stdio.h>
#include <unistd.h>     //STDIN_FILENO
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include "nservo.h"
#include "nser_pdo.h"
#include "nser_sdo.h"
#include "pthread.h"

int run = 1;
int32_t init_position = 0;
void signal_handler(int sig)
{
	run = 0;
}

 enum new_pos_status
 {
         nps_set_taget,
         nps_reached,
         nps_wait_reach
 };

 enum stop_steps
 {
         ss_switch_to_quick_stop,
         ss_wait_quick_stop_completed
 };

typedef struct {
        int mode;
        int target_position;
        int position;
        int profile_speed;
        int target_speed;
        int speed;
        int time;  // unit is ms
        int new_oper;
}app_pram_t;

float a0, a1, a2, a3;
app_pram_t  pram = {
        .speed = 0,
        .time = 10,
	.target_position = 40000,
        .new_oper = 1
};

 static int axle_start(nser_axle *ns_axle, uint16_t status, uint8_t mode)
 {
         int ret = 0;
         axle_state s = get_axle_state(status);
         switch (s) {
         case (no_ready_to_switch_on):
         case (switch_on_disable):
                 nser_pdo_set_Controlword(ns_axle, contrlword_shutdown(0));
                 break;
         case (ready_to_switch_on):
                 nser_pdo_set_Controlword(ns_axle, contrlword_switch_on(0));
                 break;
         case (switched_on):
                 nser_pdo_set_Controlword(ns_axle, contrlword_enable_operation(0));
                 nser_pdo_set_Modes_of_operation(ns_axle, mode);
                 nser_pdo_set_Target_position(ns_axle, init_position);

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


static int update_target_Position(nser_global_data *ns_data)
{
	uint16_t statusword = 0;
	uint16_t controlword = 0;
	static int32_t next_position = 0;

	static int time_index = 0;
	static enum new_pos_status pos_s = nps_set_taget;
	static enum stop_steps sstep = ss_switch_to_quick_stop;
	static int sstep_timeout = 0;
	float t = 0;
	controlword = nser_pdo_get_Controlword(&ns_data->ns_axles[0]);
	statusword = nser_pdo_get_Statusword(&ns_data->ns_axles[0]);
	if (ns_data->running == 0) {
		switch (sstep) {
		case ss_switch_to_quick_stop:
			nser_pdo_set_Controlword(&ns_data->ns_axles[0], contrlword_quick_stop(controlword));
			sstep = ss_wait_quick_stop_completed;
			return 1;
		case ss_wait_quick_stop_completed:
			if (ns_data->ns_axles[0].ns_axle_state == switch_on_disable){
				return 0;
			}
		}
		if (sstep_timeout++ > 2000000000 / ns_data->period_time) { //time out after 2s
					nser_pdo_set_Controlword(&ns_data->ns_axles[0], contrlword_disable_voltage(controlword));
					return 0;
		}else {
			return 1;
		}
	}
	else if ( axle_start(&ns_data->ns_axles[0], statusword, op_mode_csp)) {
			switch (pos_s) {
				case nps_set_taget:
					pram.position = nser_pdo_get_Position_actual_value(&ns_data->ns_axles[0]);
	                                a0 = pram.position;
	                                a1 = 0;
	                                a2 = 3.0 / (pram.time * pram.time) * (pram.target_position - pram.position);
	                                a3 = -2.0 / (pram.time * pram.time * pram.time) * (pram.target_position - pram.position);
	                                next_position = a0 + a1 * 0.005 + a2 * 0.005 * 0.005 + a3 * 0.005 * 0.005 * 0.005;  // the first 5ms
					time_index = 2;
	                                if (is_status_target_reached(statusword)) {
	                                      nser_pdo_set_Target_position(&ns_data->ns_axles[0], next_position);
	                                }
	
					pos_s = nps_reached;
					break;
	
				case nps_reached:
					t = time_index * 0.005;
	                                if (t >= pram.time) {
	                                     next_position = pram.target_position;
	                                }else {
	                                      next_position = a0 + a1 * t + a2 * t * t + a3 * t * t *t;
	                                      time_index++;
	                                }
	                                if (is_status_target_reached(statusword)) {
	                                      nser_pdo_set_Target_position(&ns_data->ns_axles[0], next_position);
	                                }
	
					break;
				case nps_wait_reach:
				       break;	
			}
		
	}
	return 1;
}


int main( int argc, char **argv)
{
	nser_global_data *ns_data;
	char *xmlfile = argv[1];
	signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);
	if (!(ns_data = nser_app_run_init_without_activate(xmlfile))) {
		fprintf(stderr, "Failed to initialize the App\n");
		return 0;
	}

	nser_sdo_get_Position_actual_value(&ns_data->ns_axles[0], &init_position);
	printf("The initial position is %d\n", init_position);


	if (nser_activate_all_masters(ns_data)) {
		printf("Failed to activate all masters\n");
		return 0;
	}

	if (user_cycle_task_start(ns_data,update_target_Position ,0))
	{
		fprintf(stderr, "Failed to start task\n");
		return 0;
	}

	while (run) {
	 sched_yield();
	}
	user_cycle_task_stop(ns_data);
	nser_deactivate_all_masters(ns_data);
}


