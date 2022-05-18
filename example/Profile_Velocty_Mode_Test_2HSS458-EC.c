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
void signal_handler(int sig)
{
	run = 0;
}


static int update_target_velocity(nser_global_data *ns_data)
{
	uint16_t statusword = 0;
	static int32_t speed = 0;
	static int dir = 1;
	signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);
	if (ns_data->ns_masteter[0].ns_master_state == ALL_OP) {
		statusword = nser_pdo_get_Statusword(&ns_data->ns_axles[0]);
		if (is_status_target_reached(statusword)) {
			printf("Currect speed is %d rpm \n", speed *60 /4000);
			if (speed >= 80000 || speed <= -80000) {
				dir *= -1;
			}
			speed += 4000 * dir;
			nser_pdo_set_Target_velocity(&ns_data->ns_axles[0], speed);
			printf("Next target speed is %d rpm \n", speed *60 /4000);
		}
	}
	return ns_data->running;

}

int main( int argc, char **argv)
{
	nser_global_data *ns_data;
	char *xmlfile = argv[1];
	if (!(ns_data = nser_app_run_init(xmlfile))) {
		fprintf(stderr, "Failed to initialize the App\n");
		return 0;
	}

	if (user_cycle_task_start(ns_data,update_target_velocity ,1))
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


