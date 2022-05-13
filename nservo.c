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
#include <sys/stat.h>
#include <fcntl.h>

#include "config.h"
#include "nservo.h"
#include "nser_pdo.h"
#include "nser_sdo.h"
/****************************************************************************/
static int setscheduler(pid_t pid, int policy, const struct sched_param *param);
uint32_t set_sdo(uint16_t index, uint8_t subindex, nser_axle *ns_axle,
		void *data, size_t data_size) {
	uint32_t abort_code;
	uint16_t index_new = index + 0x800 * ns_axle->axle_offset;
	if (ecrt_master_sdo_download(ns_axle->nser_master->ec_master,
			ns_axle->slave->slave_position, index_new, subindex, data,
			data_size, &abort_code)) {
		debug_warning("abort_code = %x\n", abort_code);
		return abort_code;
	}
	return 0;
}

uint32_t get_sdo(uint16_t index, uint8_t subindex, nser_axle *ns_axle,
		uint8_t *target, size_t target_size, size_t *result_size) {
	uint32_t abort_code;
	size_t result_size_local;
	uint16_t index_new = index + 0x800 * ns_axle->axle_offset;
	if (!result_size)
		result_size = &result_size_local;

	if (ecrt_master_sdo_upload(ns_axle->nser_master->ec_master,
			ns_axle->slave->slave_position, index_new, subindex, target,
			target_size, result_size, &abort_code)) {
		debug_warning("abort_code = %x\n", abort_code);
		return abort_code;
	}
	return 0;
}

float nser_sdo_get_Position_encoder_resolution(nser_axle *ns_axle) {
	uint32_t encoder_increments = 0, motor_resolution = 0;
	if (nser_sdo_get_PRE_Encoder_increments(ns_axle, &encoder_increments)) {
		debug_error("\n");
		return 0;
	}

	if (nser_sdo_get_PRE_Motor_revolutions(ns_axle, &motor_resolution)) {
		debug_error("\n");
		return 0;
	}

	if (!motor_resolution) {
		debug_error("motor_resolution  is 0\n");
		return 0;
	}
	return (float) encoder_increments / motor_resolution;
}

float nser_sdo_get_Velocity_encoder_resolution(nser_axle *ns_axle) {
	uint32_t encoder_increments_per_s, motor_resolution_per_s;
	if (nser_sdo_get_VNR_Encoder_increments_per_second(ns_axle,
			&encoder_increments_per_s)) {
		debug_error("\n");
		return 0;
	}

	if (nser_sdo_get_VNR_Motor_revolutions_per_second(ns_axle,
			&motor_resolution_per_s)) {
		debug_error("\n");
		return 0;
	}

	if (!motor_resolution_per_s) {
		debug_error("motor_resolution_second  is 0\n");
		return 0;
	}
	return (float) encoder_increments_per_s / motor_resolution_per_s;

}

float nser_sdo_get_Gear_ratio(nser_axle *ns_axle) {
	uint32_t motor, driving;
	if (nser_sdo_get_GR_Motor_revolutions(ns_axle, &motor)) {
		debug_error("\n");
		return 0;
	}

	if (nser_sdo_get_GR_Shaft_revolutions(ns_axle, &driving)) {
		debug_error("\n");
		return 0;
	}

	if (!driving) {
		debug_error("motor_resolution_second  is 0\n");
		return 0;
	}
	return (float) motor / driving;

}

/*************************/
nser_master *create_new_master(unsigned int num) {
	nser_master *ns_master;
	if (!(ns_master = malloc(sizeof(nser_master) * num))) {
		debug_error("Can not malloc memory for nser_master\n");
		return NULL;
	}
	ns_master->slave_number = 0;
	ns_master->ns_master_state = UNKNOWN;
	ns_master->reference_clock_slave = NULL;
	return ns_master;
}

/*************************/
nser_slave *create_new_slaves(unsigned int num) {
	nser_slave *ns_slave;
	int i;
	if (!(ns_slave = malloc(sizeof(nser_slave) * num))) {
		debug_error("Failed to malloc memory for nser_slaves\n");
		return NULL;
	}

	for (i = 0; i < num; i++) {
		ns_slave[i].name = NULL;
		ns_slave[i].VendorId = 0;
		ns_slave[i].ProductCode = 0;
		ns_slave[i].emerg_size = 0;
		ns_slave[i].force_pdo_assign = 0;
		ns_slave[i].ns_sdo_entry_number = 0;
		ns_slave[i].wd_divider = 0;
		ns_slave[i].wd_intervals = 0;

	}
	return ns_slave;
}

/*************************/
nser_axle * create_new_axles(unsigned int num) {
	nser_axle *ns_axle;
	int i;
	if (!(ns_axle = malloc(sizeof(nser_axle) * num))) {
		debug_error("Failed to malloc memory for nser_axles\n");
		return NULL;
	}

	for (i = 0; i < num; i++) {
		memset(ns_axle[i].pdos_offset, 0, sizeof(int) * PDO_ENTRY_NUM_PER_AXLE);
		ns_axle[i].ns_reg_pdo_num = 0;
		ns_axle[i].last_ns_axle_state = no_ready_to_switch_on;
		ns_axle[i].ns_axle_state = no_ready_to_switch_on;
		ns_axle[i].slave = NULL;
	}
	return ns_axle;
}

/**************get offset****************************/
static int get_pdo_entry_offset(uint16_t index, uint8_t subindex,
		unsigned int offset) {
	int i;
	uint16_t __index = index - 0x800 * offset;
	for (i = 0; i < sizeof(pdo_entry_is_table) / sizeof(pdo_entry_is_table[0]);
			i++) {
		if (pdo_entry_is_table[i].index == __index
				&& pdo_entry_is_table[i].subindex == subindex) {
			return pdo_entry_is_table[i].offset;
		}
	}
	return -1;
}

/**************************************
 *
 * 	After xml created the ns_data, the function will complete
 * 	the configuration for a/all master(s).
 *
 * 	master_index:
 * 		(0x0 ~ 0x0FE) - the index of master which we want to activate.
 * 		 0xFF - Activate all master.
 * 	return:
 * 		-1 : Failed
 * 		0  : Success
 *
 */
static int _nser_config_master(nser_master *ns_master, uint32_t period_time) {
	nser_slave *ns_slave;
	int i, j, k, h;
	int ret;
	nser_sdo_entry *ns_sdo_entry;
//	ec_sync_info_t sync_info;
	nser_sync_info *ns_sync_info;
	nser_pdo_info *ns_pdo_info;
	nser_pdo_entry *ns_pdo_entry;
	uint32_t entry_data;
	ec_pdo_entry_info_t *entry_info;
	debug_info("Start to config master%d\n", ns_master->master_index);
	ns_master->ec_master = ecrt_request_master(ns_master->master_index);
	if (!ns_master->ec_master) {
		debug_error("Failed to request master(%d)\n", ns_master->master_index);
		return -1;
	}

	if (!(ns_master->domain = ecrt_master_create_domain(ns_master->ec_master))) {
		debug_error("Failed to create domain for master(%d)\n",
				ns_master->master_index);
		return -1;
	}

	for (i = 0; i < ns_master->slave_number; i++) {
		ns_slave = &ns_master->slaves[i];
		/* ecrt configure slave*/
		debug_info(" Start to config slave%d\n", ns_slave->slave_position);
		if (!(ns_slave->sc = ecrt_master_slave_config(ns_master->ec_master,
				ns_slave->alias, ns_slave->slave_position, ns_slave->VendorId,
				ns_slave->ProductCode))) {
			debug_error("Failed to config slave%d configuration on master%d\n",
					i, ns_master->master_index);
			return -1;
		}
		/* ecrt configure sdo entry*/
		debug_info("  Start to config sdos\n");
		for (j = 0; j < ns_slave->ns_sdo_entry_number; j++) {
			ns_sdo_entry = &ns_slave->ns_sdo_entry[j];
			ret = -1;
			if (ns_sdo_entry->lenbit == 8) {
				ret = ecrt_slave_config_sdo8(ns_slave->sc, ns_sdo_entry->index,
						ns_sdo_entry->subindex, (uint8_t) (ns_sdo_entry->v32));
			} else if (ns_sdo_entry->lenbit == 16) {
				ret = ecrt_slave_config_sdo16(ns_slave->sc, ns_sdo_entry->index,
						ns_sdo_entry->subindex, (uint16_t) (ns_sdo_entry->v32));
			} else if (ns_sdo_entry->lenbit == 32) {
				ret = ecrt_slave_config_sdo32(ns_slave->sc, ns_sdo_entry->index,
						ns_sdo_entry->subindex, (ns_sdo_entry->v32));
			}
			if (ret) {
				debug_error(
						"Failed to config sdo entry(0x%x:0x%x) slave%d configuration on master%d\n",
						ns_sdo_entry->index, ns_sdo_entry->subindex,
						ns_slave->slave_position, ns_master->master_index);
				return -1;
			}debug_info("   Config sdo(0x%x:0x%x) bitlen=%d value=%d\n",
					ns_sdo_entry->index, ns_sdo_entry->subindex, ns_sdo_entry->lenbit, ns_sdo_entry->v32);
		}

		if (ns_slave->force_pdo_assign) {
			debug_info("  Start to config Sync info\n");
			for (j = 0; j < ns_slave->ns_sync_info_num; j++) {
				ns_sync_info = &ns_slave->ns_sync_info[j];
				debug_info("   config Sync_info(0x%x)\n", ns_sync_info->index);
				/*There are some pdo to assign this sm*/
				if (ns_sync_info->ns_pdo_info_num) {
					/*Disable this sm*/
					debug_info("    Disable Sync_info(0x%x)\n", ns_sync_info->index);
					if (ecrt_slave_config_sdo8(ns_slave->sc,
							ns_sync_info->index, 0, 0)) {
						debug_error("Failed to clear sm(0x%x : 0x0) pdo \n",
								ns_sync_info->index);
						return -1;
					}
					/*Assign every pdo to this sm*/
					debug_info("    	Find %d pdos\n", ns_sync_info->ns_pdo_info_num);
					for (k = 0; k < ns_sync_info->ns_pdo_info_num; k++) {
						ns_pdo_info = &ns_sync_info->ns_pdo_info[k];
						/*There are some pdo entry to assign to this pdo*/
						if (ns_pdo_info->ns_pdo_entry_num) {
							debug_info("    	Find %d entry on pdos(0x%x)\n", ns_pdo_info->ns_pdo_entry_num, ns_pdo_info->pdo_info->index);
							/*Disable this pdo*/
							if (ecrt_slave_config_sdo8(ns_slave->sc,
									ns_pdo_info->pdo_info->index, 0, 0)) {
								debug_error(
										"Failed to clear sm(0x%x: 0x0) pdo\n",
										ns_sync_info->index);
								return -1;
							}
							/*Assign every pdo entry to this pdo*/
							for (h = 0; h < ns_pdo_info->ns_pdo_entry_num;
									h++) {
								ns_pdo_entry = &ns_pdo_info->ns_pdo_entry[h];
								entry_info = ns_pdo_entry->pdo_entry;
								entry_data = (entry_info->index << 16)
										+ (entry_info->subindex << 8)
										+ entry_info->bit_length;
								if (ecrt_slave_config_sdo32(ns_slave->sc,
										ns_pdo_info->pdo_info->index,
										ns_pdo_entry->pdo_subindex,
										entry_data)) {
									debug_error(
											"Failed to set pdo(0x%x:0x%x)\n",
											ns_pdo_info->pdo_info->index,
											ns_pdo_entry->pdo_subindex);
									return -1;
								}debug_info("    	 Add entry(0x%x) to pdo(0x%x:0x%x)\n", entry_data, ns_pdo_info->pdo_info->index, ns_pdo_entry->pdo_subindex);
								/*Enable this pdo*/

							}

							if (ecrt_slave_config_sdo8(ns_slave->sc,
									ns_pdo_info->pdo_info->index, 0,
									ns_pdo_info->ns_pdo_entry_num)) {
								debug_error("Failed to set sm(0x%x: 0x0) pdo\n",
										ns_sync_info->index);
								return -1;
							}debug_info("    	Enable pdo(0x%x) with %d pdos\n", ns_pdo_info->pdo_info->index, ns_pdo_info->ns_pdo_entry_num);

							if (ecrt_slave_config_sdo16(ns_slave->sc,
									ns_sync_info->index,
									ns_pdo_info->sync_subindex,
									ns_pdo_info->pdo_info->index)) {
								debug_error("Failed to set pdo(0x%x)\n",
										ns_pdo_info->pdo_index);
								return -1;
							}debug_info("    	 Add pdo(0x%x) to sync_info(0x%x:0x%x)\n",ns_pdo_info->pdo_info->index , ns_sync_info->index, ns_pdo_info->sync_subindex);
						}
						/*Enable this sm*/
						if (ecrt_slave_config_sdo8(ns_slave->sc,
								ns_sync_info->index, 0,
								ns_sync_info->ns_pdo_info_num)) {
							debug_error("Failed to set sm(0x%x: 0x0) pdo\n",
									ns_sync_info->index);
							return -1;
						}debug_info("    Enable Sync_info(0x%x) with %d pdos\n", ns_sync_info->index, ns_sync_info->ns_pdo_info_num);
					}
				}
			}
		}

		ecrt_slave_config_watchdog(ns_slave->sc, ns_slave->wd_divider,
				ns_slave->wd_intervals);
		if (ns_slave->dc)
			ecrt_slave_config_dc(ns_slave->sc, 0x0300, period_time,
					ns_slave->shift_time, 0, 0);
		if (ecrt_slave_config_emerg_size(ns_slave->sc, ns_slave->emerg_size)) {
			debug_warning("Failed to set Emerg_size to %lu on slave%d\n",
					ns_slave->emerg_size, ns_slave->slave_position);
		}
		if (ecrt_slave_config_pdos(ns_slave->sc, EC_END, ns_slave->sync_info)) {
			debug_error("Failed to configure PDOs on %s.\n", ns_slave->name);
			return -1;
		}debug_info(" config reference_clock\n");
//		if (ecrt_master_select_reference_clock(ns_master->ec_master,
//				ns_master->reference_clock_slave->sc)) {
//			debug_warning("Failed to select Reference clock to use slave %d\n",
//					ns_master->reference_clock_slave->slave_position);
//		}
	}
	return 0;
}
int nser_config_master(nser_global_data *ns_data, unsigned int master_index) {
	int i, ret;
	if (master_index == 0xff) {
		for (i = 0; i < ns_data->num_master; i++) {
			if ((ret = _nser_config_master(&ns_data->ns_masteter[i],
					ns_data->period_time))) {
				return ret;
			}
		}
		return 0;
	} else {
		for (i = 0; i < ns_data->num_master; i++) {
			if (master_index == ns_data->ns_masteter[i].master_index) {
				return _nser_config_master(&ns_data->ns_masteter[i],
						ns_data->period_time);
			}
		}
		debug_error("Can not find the master with master_index ==  %d\n",
				master_index);
		return -1;
	}
}

/******************************
 *
 * 	index:
 * 		(0x0 ~ 0x0FE) - the index of axle which we want to configure.
 * 		 0xFF - configure all axles.
 * 	return:
 * 		-1 : Failed
 * 		0  : Success
 *
 */
static int _nser_config_axle(nser_axle *ns_axle) {
	int i, off, offset;
	nser_reg_pdo_entry *reg_pdo_entry;
	debug_info("Config axle%d\n",ns_axle->axle_index);debug_info("  Find %d reg_pdo\n", ns_axle->ns_reg_pdo_num);
	for (i = 0; i < ns_axle->ns_reg_pdo_num; i++) {

		reg_pdo_entry = &ns_axle->ns_reg_pdo[i];
		if ((off = ecrt_slave_config_reg_pdo_entry(ns_axle->slave->sc,
				reg_pdo_entry->index, reg_pdo_entry->subindex,
				ns_axle->nser_master->domain, NULL)) < 0) {
			debug_error("Failed to ecrt config reg_pod(0x%x 0x%x) offset\n",
					reg_pdo_entry->index, reg_pdo_entry->subindex);
			return -1;
		}

		if ((offset = get_pdo_entry_offset(reg_pdo_entry->index,
				reg_pdo_entry->subindex, ns_axle->axle_offset)) < 0) {
			debug_error("Failed to get reg_pod(0x%x 0x%x) offset\n",
					reg_pdo_entry->index, reg_pdo_entry->subindex);
			return -1;
		}
		ns_axle->pdos_offset[offset] = off;
		debug_info("  Register pdo entry(0x%x:0x%x) with offset = %d\n",reg_pdo_entry->index, reg_pdo_entry->subindex, offset);
	}
	return 0;
}
int nser_config_axle(nser_global_data *ns_data, unsigned int axle_index) {
	int i, ret;
	if (axle_index == 0xff) {
		for (i = 0; i < ns_data->axle_number; i++) {
			if ((ret = _nser_config_axle(&ns_data->ns_axles[i]))) {
				return ret;
			}
		}
		return 0;
	} else {

		if (axle_index > ns_data->axle_number) {
			debug_error("The axle_index %d is excess the sum of all axles\n",
					axle_index);
			return -1;
		} else {
			return _nser_config_axle(&ns_data->ns_axles[axle_index]);
		}
	}
}

/**************************************
 *
 * 	master_index:
 * 		(0x0 ~ 0x0FE) - the index of master which we want to activate.
 * 		 0xFF - Activate all master.
 * 	return:
 * 		-1 : Failed
 * 		0  : Success
 *
 */

int nser_activate_master(nser_global_data *ns_data, unsigned int master_index) {
	int i;
	int64_t ret = 0;
	if (master_index == 0xff) {
		for (i = 0; i < ns_data->num_master; i++) {
			if (ecrt_master_activate(ns_data->ns_masteter[i].ec_master)) {
				debug_error("Failed to activate master%d\n",
						ns_data->ns_masteter[i].master_index);
				ret = -1;
				break;
			}

			if (!(ns_data->ns_masteter[i].domain_dp = ecrt_domain_data(
					ns_data->ns_masteter[i].domain))) {
				debug_error("Failed to get domain point on master%d\n",
						ns_data->ns_masteter[i].master_index);
				ret = -1;
				break;
			}
			ns_data->ns_masteter[i].ns_master_state = ACTIVATE;
		}

		if (ret < 0) {
			for (i = 0; i < ns_data->num_master; i++) {
				if (ns_data->ns_masteter[i].ns_master_state == ACTIVATE) {
					ecrt_master_deactivate(ns_data->ns_masteter[i].ec_master);
					ns_data->ns_masteter[i].ns_master_state = UNKNOWN;
				}
			}

			return -1;
		} else {
			for (i = 0; i < ns_data->axle_number; i++) {
				ns_data->ns_axles[i].domain_dp =
						ns_data->ns_axles[i].nser_master->domain_dp;
			}

			return 0;
		}
	} else {
		if (ecrt_master_activate(
				ns_data->ns_masteter[master_index].ec_master)) {
			debug_error("Failed to activate master%d\n",
					ns_data->ns_masteter[master_index].master_index);
			return -1;
		}
		if (!(ns_data->ns_masteter[master_index].domain_dp = ecrt_domain_data(
				ns_data->ns_masteter[master_index].domain))) {
			debug_error("Failed to get domain point on master%d\n",
					ns_data->ns_masteter[master_index].master_index);
			ecrt_master_deactivate(
					ns_data->ns_masteter[master_index].ec_master);
			return -1;
		}

		ns_data->ns_masteter[master_index].ns_master_state = ACTIVATE;
		return 0;
	}
}

void nser_deactivate_master(nser_global_data *ns_data,
		unsigned int master_index) {
	int i;
	if (master_index == 0xff) {
		for (i = 0; i < ns_data->num_master; i++) {
			if (ns_data->ns_masteter[i].ns_master_state != UN_KNOWN) {
				ecrt_master_deactivate(ns_data->ns_masteter[i].ec_master);
				ns_data->ns_masteter[i].ns_master_state = UN_KNOWN;
			}
		}

	} else {

		if (ns_data->ns_masteter[master_index].ns_master_state != UN_KNOWN) {
			ecrt_master_deactivate(
					ns_data->ns_masteter[master_index].ec_master);
			ns_data->ns_masteter[master_index].ns_master_state = UN_KNOWN;
		}
	}
}

/********************************************************/

static void update_master_domain_states(nser_global_data *ns_data) {

	int i;
	ec_master_state_t master_state;
	ec_domain_state_t domain_state;
	domain_state.wc_state = EC_WC_ZERO;
	nser_master *ns_master;
	if (!(ns_data->cycle_counter % ns_data->master_status_update_freq)) {
		for (i = 0; i < ns_data->num_master; i++) {
			ns_master = &ns_data->ns_masteter[i];
			ecrt_domain_state(ns_master->domain, &domain_state);
			ecrt_master_state(ns_master->ec_master, &master_state);
			ns_master->ns_domain_state = domain_state.wc_state;
			if (master_state.slaves_responding == 0) {
				ns_master->ns_master_state = NO_SLAVE;
			} else {
				if (master_state.al_states == 0x08) {
					ns_master->ns_master_state = ALL_OP;
				} else {
					ns_master->ns_master_state = NO_ALL_OP;
				}
			}
		}
	}
}

static void nser_receive_all(nser_global_data *ns_data) {
	int i;
	nser_master *ns_master;
	for (i = 0; i < ns_data->num_master; i++) {
		ns_master = &ns_data->ns_masteter[i];
		ecrt_master_receive(ns_master->ec_master);
		ecrt_domain_process(ns_master->domain);
	}
}

/*****************************************************************************/
static void update_slave_config_states(nser_global_data *ns_data) {
	ec_slave_config_state_t s;
	int i, j;
	nser_master *ns_master;
	nser_slave *ns_slave;
	if (!(ns_data->cycle_counter % ns_data->slave_status_update_freq)) {
		for (i = 0; i < ns_data->num_master; i++) {
			ns_master = &ns_data->ns_masteter[i];
			for (j = 0; j < ns_master->slave_number; j++) {
				ns_slave = &ns_master->slaves[j];
				ecrt_slave_config_state(ns_slave->sc, &s);
				ns_slave->ns_slave_state = s.al_state;
			}
		}
	}
}

static void update_axles_states(nser_global_data *ns_data) {
	int i;
	nser_axle *ns_axle;
	uint16_t statusword;
	if (!(ns_data->cycle_counter % ns_data->axle_status_update_freq)) {
		for (i = 0; i < ns_data->axle_number; i++) {
			ns_axle = &ns_data->ns_axles[i];
			statusword = nser_pdo_get_Statusword(ns_axle);
			ns_axle->ns_axle_state = get_axle_state(statusword);
		}
	}
}

static void nser_send_all(nser_global_data *ns_data) {
	int i;
	nser_master *ns_master;
	for (i = 0; i < ns_data->num_master; i++) {
		ns_master = &ns_data->ns_masteter[i];
		ecrt_domain_queue(ns_master->domain);
		ecrt_master_send(ns_master->ec_master);
	}
}

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
/*****************************************************************************/

enum device_state_t {
	ds_not_read = 0,
	ds_switch_disable,
	ds_ready_to,
	ds_switch_on,
	ds_operation,
	ds_quick_stop,
	ds_fault_reaction,
	ds_fault
};

//void read_sdo(ec_sdo_request_t *sdo) {
//	switch (ecrt_sdo_request_state(sdo)) {
//	case EC_REQUEST_UNUSED: // request was not used yet
//		ecrt_sdo_request_read(sdo); // trigger first read
//		break;
//	case EC_REQUEST_BUSY:
//		break;
//	case EC_REQUEST_SUCCESS:
//		printf("----------------------------------------->SDO value: 0x%04X\n",
//				EC_READ_U16(ecrt_sdo_request_data(sdo)));
//		ecrt_sdo_request_read(sdo); // trigger next read
//		break;
//	case EC_REQUEST_ERROR:
//		ecrt_sdo_request_read(sdo); // retry reading
//		break;
//	}
//}

static int nser_axle_auto_start(nser_axle *ns_axle) {
	int ret = 0;
	axle_state s = ns_axle->ns_axle_state;
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
		nser_pdo_set_Modes_of_operation(ns_axle, ns_axle->mode);
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
	if (ns_axle->last_ns_axle_state != s) {
		debug_info(" Axle%d device_state is changed to %s\n", ns_axle->axle_index, axle_state_to_str(s));
		ns_axle->last_ns_axle_state = s;
	}
	return ret;
}

/****************************************************************************/

static void stack_prefault(void) {
	unsigned char dummy[MAX_SAFE_STACK];
	memset(dummy, 0, MAX_SAFE_STACK);
}

#define timespec_add(a, b, result)                          \
	do {                                                      \
		(result)->tv_sec = (a)->tv_sec + (b)->tv_sec;           \
		(result)->tv_nsec = (a)->tv_nsec + (b)->tv_nsec;        \
		if ((result)->tv_nsec >= NSEC_PER_SEC)                  \
		{                                                       \
			++(result)->tv_sec;                             \
			(result)->tv_nsec -= NSEC_PER_SEC;              \
		}                                                       \
	} while (0)

static int latency_target_fd = -1;

/* Latency trick
 * if the file /dev/cpu_dma_latency exists,
 * open it and write a zero into it. This will tell 
 * the power management system not to transition to 
 * a high cstate (in fact, the system acts like idle=poll)
 * When the fd to /dev/cpu_dma_latency is closed, the behavior
 * goes back to the system default.
 * 
 * Documentation/power/pm_qos_interface.txt
 */
static void set_low_latency(void)
{
	struct stat s;
	int ret;
	int32_t latency_target_value = 0;

	if (stat("/dev/cpu_dma_latency", &s) == 0) {
		latency_target_fd = open("/dev/cpu_dma_latency", O_RDWR);
		if (latency_target_fd == -1)
			return;
		ret = write(latency_target_fd, &latency_target_value, 4);
		if (ret == 0) {
			printf("# error setting cpu_dma_latency to %d!: %s\n", latency_target_value, strerror(errno));
			close(latency_target_fd);
			return;
		}
		printf("# /dev/cpu_dma_latency set to %dus\n", latency_target_value);
	}
}

void stop_low_latency(void)
{
	if (latency_target_fd >= 0)
		close(latency_target_fd);
}

static void *cycle_task(void *data) {
	int ret, i, running = 1;
	nser_global_data *ns_data = data;
	nser_cycle_task_t task = ns_data->task;
	struct timespec wakeup_time;
	struct timespec cycletime;
	struct timespec start_time;
	struct sched_param param;
	memset(&param, 0, sizeof(param));
	param.sched_priority = ns_data->sched_priority;
	setscheduler(0, ns_data->sched_policy, &param);
	debug_info("Starting cycle task with dt=%u ns.\n", ns_data->period_time);
	set_low_latency();
	cycletime.tv_nsec = ns_data->period_time;
	cycletime.tv_sec = 0;
	clock_gettime(CLOCK_REALTIME, &wakeup_time);
	while (running) {
		timespec_add(&wakeup_time, &cycletime, &wakeup_time);
		ret = clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &wakeup_time,
		NULL);
		if (ret) {
			debug_error("Failed to clock_nanosleep(): %s\n", strerror(ret));
			break;
		}
		clock_gettime(CLOCK_REALTIME, &start_time);
		nser_receive_all(ns_data);
		update_master_domain_states(ns_data);
		update_slave_config_states(ns_data);
		update_axles_states(ns_data);

		ret = 0;
		if (ns_data->is_auto_start) {
			for (i = 0; i < ns_data->num_master; i++) {
				if (ns_data->ns_masteter[i].ns_master_state == ALL_OP) {
					ret++;
				}
			}
			for (i = 0; i < ns_data->axle_number; i++) {
				ret += nser_axle_auto_start(&ns_data->ns_axles[i]);
			}
			if (ret == (ns_data->num_master + ns_data->axle_number)) {
				ns_data->is_auto_start = 0;
			}
		}
		running = (*task)(ns_data);
		for (i = 0; i < ns_data->num_master; i++){
			ecrt_master_application_time(ns_data->ns_masteter[i].ec_master,
					TIMESPEC2NS(start_time));
			ecrt_master_sync_reference_clock(
					ns_data->ns_masteter[i].ec_master);
			ecrt_master_sync_slave_clocks(ns_data->ns_masteter[i].ec_master);
		}
		nser_send_all(ns_data);
		ns_data->cycle_counter++;
	}
	stop_low_latency();
	return NULL;
}
static int raise_soft_prio(int policy, const struct sched_param *param)
{
        int err;
        int policy_max; /* max for scheduling policy such as SCHED_FIFO */
        int soft_max;
        int hard_max;
        int prio;
        struct rlimit rlim;

        prio = param->sched_priority;

        policy_max = sched_get_priority_max(policy);
        if (policy_max == -1) {
                err = errno;
                debug_info("WARN: no such policy\n");
                return err;
        }

        err = getrlimit(RLIMIT_RTPRIO, &rlim);
        if (err) {
                err = errno;
                debug_info("WARN: getrlimit failed");
                return err;
        }

        soft_max = (rlim.rlim_cur == RLIM_INFINITY) ? policy_max : rlim.rlim_cur;
        hard_max = (rlim.rlim_max == RLIM_INFINITY) ? policy_max : rlim.rlim_max;

        if (prio > soft_max && prio <= hard_max) {
                rlim.rlim_cur = prio;
                err = setrlimit(RLIMIT_RTPRIO, &rlim);
                if (err) {
                        err = errno;
                        debug_info("WARN: setrlimit failed");
                        /* return err; */
                }
        } else {
                err = -1;
        }

        return err;
}

/****************************************************************************/
int user_cycle_task_start(nser_global_data *ns_data, nser_cycle_task_t task,
		int isAutoStart) {

	int ret = 0;

	/* Lock memory */

	if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1) {
		debug_warning("Failed to lock memory: %s\n",
				strerror(errno));
	}

	stack_prefault();

	pthread_attr_t thattr;
	pthread_attr_init(&thattr);
	//pthread_attr_setstack(&thattr, NULL, 1024*1024*64);
	ns_data->task = task;
	ns_data->running = 1;
	ns_data->is_auto_start = isAutoStart;
	ret = pthread_create(&ns_data->cyclic_thread, &thattr, &cycle_task,
			(void *) ns_data);
	if (ret) {
		debug_error("Failed to create cyclic task: %s \n", strerror(-ret));

	}
	return ret;
}

void user_cycle_task_stop(nser_global_data *ns_data) {
	ns_data->running = 0;
	pthread_join(ns_data->cyclic_thread, NULL);
}

nser_global_data *nser_app_run_init(char *xmlfile) {

	nser_global_data *ns_data;
	if (!(ns_data = malloc(sizeof(nser_global_data)))) {
		debug_error("Failed to malloc global data\n");
		return NULL;
	}
	memset(ns_data, 0, sizeof(nser_global_data));
	if ((nser_xmlconfig(ns_data, xmlfile))) {
		debug_error("Failed to open xml configuration file: %s \n", xmlfile);
		return NULL;
	}

	if (nser_config_all_masters(ns_data)) {
		debug_error("Failed to configure masters\n");
		return NULL;
	}

	if (nser_config_all_axles(ns_data)) {
		debug_error("Failed to configure axles\n");
		return NULL;
	}

	if (nser_activate_all_masters(ns_data)) {
		debug_error("Failed to activate all masters\n");
		return NULL;
	}
	return ns_data;
}

nser_global_data *nser_app_run_init_without_activate(char *xmlfile) {

	nser_global_data *ns_data;
	if (!(ns_data = malloc(sizeof(nser_global_data)))) {
		debug_error("Failed to malloc global data\n");
		return NULL;
	}
	memset(ns_data, 0, sizeof(nser_global_data));
	if ((nser_xmlconfig(ns_data, xmlfile))) {
		debug_error("Failed to open xml configuration file: %s \n", xmlfile);
		goto free_ns_data;
	}

	if (nser_config_all_masters(ns_data)) {
		debug_error("Failed to configure masters\n");
		goto free_ns_data;
	}

	if (nser_config_all_axles(ns_data)) {
		debug_error("Failed to configure axles\n");
		goto free_ns_data;
	}
	return ns_data;

free_ns_data:
	free(ns_data);
	return NULL;
}

/* Check the error status of sched_setscheduler
 * If an error can be corrected by raising the soft limit priority to
 * a priority less than or equal to the hard limit, then do so.
 */
static int setscheduler(pid_t pid, int policy, const struct sched_param *param)
{
        int err = 0;

try_again:
        err = sched_setscheduler(pid, policy, param);
        if (err) {
                err = errno;
                if (err == EPERM) {
                        int err1;
                        err1 = raise_soft_prio(policy, param);
                        if (!err1) goto try_again;
                }
        }

        return err;
}
