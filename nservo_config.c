// SPDX-License-Identifier: (GPL-2.0 OR MIT)
/*
 * Copyright 2018-2020 NXP
 */

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h> /* clock_gettime() */
#include <sys/mman.h> /* mlockall() */
#include <pthread.h>

#include "config.h"
#include "tool.h"
#include "nservo.h"
/****************************************************************************/

nser_global_data *nser_app_config_init(char *xmlfile) {

	nser_global_data *ns_data;
	if (!(ns_data = malloc(sizeof(nser_global_data)))) {
		debug_error("Failed to malloc global data\n");
		return NULL;
	}
	memset(ns_data, 0, sizeof(nser_global_data));
	if (xmlfile) {
		if ((nser_xmlconfig(ns_data, xmlfile))) {
			debug_error("Failed to open xml configuration file: %s \n",
					xmlfile);
			goto free_ns_data;
		}

		if (nser_config_all_masters(ns_data)) {
			debug_error("Failed to configure masters\n");
			goto free_ns_axles;
		}

		if (nser_config_all_axles(ns_data)) {
			debug_error("Failed to configure axles\n");
			goto free_ns_axles;
		}
	}
	return ns_data;

free_ns_axles:
	free(ns_data->ns_axles);
free_ns_data:
	free(ns_data);
	return NULL;
}

int nser_app_load_xml(nser_global_data *ns_data, char *xmlfile) {
	if (xmlfile) {
		if (nser_xmlconfig(ns_data, xmlfile)) {
			debug_error("Failed to open xml configuration file: %s \n",
					xmlfile);
			return -1;
		}

		if (nser_config_all_masters(ns_data)) {
			debug_error("Failed to configure masters\n");
			return -1;
		}

		if (nser_config_all_axles(ns_data)) {
			debug_error("Failed to configure axles\n");
			return -1;
		}
		return 0;
	}
	return -1;
}

/** The cycle task callback function ***/
static int update_all_pods(nser_global_data *ns_data) {
	tool_data *t_data = nser_get_private_date(ns_data);
	int i;
	if (!pthread_mutex_trylock(&t_data->mutex)) {
		for (i = 0; i < ns_data->num_master; i++) {
			memcpy(t_data->d_data[i].rx, t_data->d_data[i].domain,
					t_data->d_data[i].rx_len + t_data->d_data[i].tx_len);
			if (!t_data->d_data[i].initialed
					&& ns_data->ns_masteter[i].ns_master_state == ALL_OP) {
				memcpy(t_data->d_data[i].tx, t_data->d_data[i].domain,
						t_data->d_data[i].tx_len);
				t_data->d_data[i].initialed = 1;
			} else if (t_data->d_data[i].isUpdate) {
				memcpy(t_data->d_data[i].domain, t_data->d_data[i].tx,
						t_data->d_data[i].tx_len);
				t_data->d_data[i].isUpdate = 0;
			}
		}
		pthread_mutex_unlock(&t_data->mutex);
	}
	return ns_data->running;
}

int nser_tool_start_task(nser_global_data *ns_data, int isAutoStart) {
	if ((user_cycle_task_start(ns_data, update_all_pods, isAutoStart))) {
		debug_error("Failed to start task\n");
		return -1;
	}
	return 0;
}

tool_data *nser_config_tool_init(int max_domain_size) {
	tool_data * t_data = malloc(sizeof(tool_data));
	if (!t_data) {
		debug_error("Failed to malloc memory for tool_data \n");
		return NULL;
	}

	pthread_mutex_init(&t_data->mutex, NULL);
	t_data->isActivate = 0;
	t_data->isStart = 0;
	t_data->isLoadXML = 0;
	t_data->isFinished = 0;
	return t_data;
}

int nser_config_tool_finish(nser_global_data *ns_data, tool_data *t_data) {
	int i, j, k, l;
	nser_master * ns_master;
	nser_slave * ns_slave;
	t_data->d_data = malloc(sizeof(domain_data) * ns_data->num_master);
	if (!t_data->d_data) {
		debug_error("Failed to malloc memory for domain_data \n");
		return -1;
	}
	memset(t_data->d_data, 0, sizeof(domain_data) * ns_data->num_master);
	for (i = 0; i < ns_data->num_master; i++) {
		ns_master = &ns_data->ns_masteter[i];
		for (j = 0; j < ns_master->slave_number; j++) {
			ns_slave = &ns_master->slaves[j];
			for (k = 0; k < ns_slave->ns_sync_info_num; k++) {
				if (ns_slave->ns_sync_info[k].sync_info->dir == EC_DIR_INPUT) {
					for (l = 0; l < ns_slave->ns_sync_info[k].ns_pdo_info_num;
							l++) {
						t_data->d_data[i].rx_len +=
								ns_slave->ns_sync_info[k].ns_pdo_info[l].data_len;
					}
				} else if (ns_slave->ns_sync_info[k].sync_info->dir
						== EC_DIR_OUTPUT) {
					for (l = 0; l < ns_slave->ns_sync_info[k].ns_pdo_info_num;
							l++) {
						t_data->d_data[i].tx_len +=
								ns_slave->ns_sync_info[k].ns_pdo_info[l].data_len;
					}
				}
			}
		}
	}

	t_data->t_axle = malloc(sizeof(tool_axle) * ns_data->axle_number);
	if (!t_data->t_axle) {
		debug_error("Failed to malloc memory for tool_axle \n");
		return -1;
	}
	memset(t_data->t_axle, 0, sizeof(tool_axle) * ns_data->axle_number);
	for (i = 0; i < ns_data->num_master; i++) {
		t_data->d_data[i].domain = ns_data->ns_masteter[i].domain_dp;
		t_data->d_data[i].rx = malloc(t_data->d_data[i].rx_len);
		if (!t_data->d_data[i].rx) {
			debug_error("Failed to malloc memory for d_data[%d.rx] len= %d.\n",
					i, t_data->d_data[i].rx_len);
			return -1;
		}
		t_data->d_data[i].tx = malloc(t_data->d_data[i].tx_len);
		if (!t_data->d_data[i].tx) {
			debug_error("Failed to malloc memory for d_data[%d].tx len= %d.\n",
					i, t_data->d_data[i].tx_len);
			return -1;
		}
		t_data->d_data[i].isUpdate = 0;
		t_data->d_data[i].isUpdate = 0;
		t_data->d_data[i].initialed = 0;
	}

	for (i = 0; i < ns_data->axle_number; i++) {
		for (j = 0; j < ns_data->num_master; j++) {
			if (ns_data->ns_axles[i].nser_master->master_index
					== ns_data->ns_masteter->master_index) {
				t_data->t_axle[i].ns_axle = &ns_data->ns_axles[i];
				t_data->t_axle[i].d_data = &t_data->d_data[j];
			}
		}
	}

	t_data->isFinished = 1;
	nser_set_private_data(ns_data, t_data);
	return 0;
}

pdo_cmd_fun_t find_pdo_cmd_fun(char *cmd) {
	int i;
	for (i = 0; i < sizeof(pdo_cmd_list) / sizeof(pdo_cmd_list[0]); i++) {
		if (!strcmp(cmd, pdo_cmd_list[i].str)) {
			return pdo_cmd_list[i].fun;
		}
	}
	return NULL;
}

sdo_cmd_fun_t find_sdo_cmd_fun(char *cmd) {
	int i;
	for (i = 0; i < sizeof(sdo_cmd_list) / sizeof(sdo_cmd_list[0]); i++) {
		if (!strcmp(cmd, sdo_cmd_list[i].str)) {
			return sdo_cmd_list[i].fun;
		}

	}
	return NULL;
}

/****************************************************************************/

