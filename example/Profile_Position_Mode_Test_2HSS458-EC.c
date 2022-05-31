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
#include "pthread.h"

int run = 1;
int auto_start = 1;
void signal_handler(int sig)
{
	run = 0;
}

 enum new_pos_status
 {
         nps_init,
         nps_set_new_point,
         nps_wait_set_ack,
         nps_op,
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
	static int32_t position = 0;
	static int dir = 1;
	static enum new_pos_status pos_s = nps_init;
	controlword = nser_pdo_get_Controlword(&ns_data->ns_axles[0]);
	statusword = nser_pdo_get_Statusword(&ns_data->ns_axles[0]);
	printf("s=%x\n",statusword );

	if ((!auto_start
			&& axle_start(&ns_data->ns_axles[0], statusword, op_mode_pp) == 1)
			|| auto_start) {
		if (ns_data->ns_masteter[0].ns_master_state == ALL_OP) {
			switch (pos_s) {
			case nps_init:
				position = nser_pdo_get_Position_actual_value(&ns_data->ns_axles[0]);
				nser_pdo_set_Target_position(&ns_data->ns_axles[0], position);
				nser_pdo_set_Controlword(&ns_data->ns_axles[0],
						contrlword_new_set_point(controlword));
				pos_s = nps_set_new_point;
				break;

			case nps_set_new_point:
				if (is_status_target_reached(statusword)) {
					position = nser_pdo_get_Position_actual_value(
							&ns_data->ns_axles[0]);
					printf("Currect position is %d \n", position);
					if (position >= 20000 || position <= -20000) {
						dir *= -1;
					}
					position += (100 * dir);
					nser_pdo_set_Target_position(&ns_data->ns_axles[0],
							position);
					nser_pdo_set_Controlword(&ns_data->ns_axles[0],
							contrlword_new_set_point(controlword));
					printf("Next target speed is %d  \n\n", position);
					pos_s = nps_op;
				}
				break;
			case nps_wait_set_ack:
				if (!is_status_set_point_acknowledge(statusword))
					pos_s = nps_set_new_point;
				break;

			case nps_op:
				nser_pdo_set_Controlword(&ns_data->ns_axles[0], 0xf);
				pos_s = nps_set_new_point;
				break;

			}
		}
	}
	return ns_data->running;
}


int main( int argc, char **argv)
{
	nser_global_data *ns_data;
	signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);
	if (argc > 2)
	{
		if(argv[2][0] == 'n')
			auto_start = 0;
	}
	char *xmlfile = argv[1];
	if (!(ns_data = nser_app_run_init(xmlfile))) {
		fprintf(stderr, "Failed to initialize the App\n");
		return 0;
	}

	if (user_cycle_task_start(ns_data,update_target_Position ,auto_start))
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


