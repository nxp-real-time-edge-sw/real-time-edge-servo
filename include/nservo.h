// SPDX-License-Identifier: (GPL-2.0 OR MIT)
/*
 * Copyright 2018-2020 NXP
 */

#ifndef NSERVO_H_
#define NSERVO_H_
#include "config.h"
#include "ds402.h"

typedef struct _nser_master nser_master;
typedef struct _nser_slave nser_slave;
typedef struct _nser_axle nser_axle;
typedef struct _nser_global_data nser_global_data;
typedef struct _nser_sdo_entry nser_sdo_entry;
typedef struct _nser_pdo_entry nser_pdo_entry;
typedef struct _nser_pdo_info nser_pdo_info;
typedef struct _nser_sync_info nser_sync_info;
typedef struct _nser_reg_pdo_entry nser_reg_pdo_entry;

#define NSEC_PER_SEC (1000000000)
#define FREQUENCY (NSEC_PER_SEC / PERIOD_NS)
#define DIFF_NS(A, B) (((B).tv_sec - (A).tv_sec) * NSEC_PER_SEC + \
        (B).tv_nsec - (A).tv_nsec)

#define TIMESPEC2NS(T) ((uint64_t) (T).tv_sec * NSEC_PER_SEC + (T).tv_nsec)

uint32_t set_sdo(uint16_t index, uint8_t subindex, nser_axle *ns_axle,
		void *data, size_t data_size);

uint32_t get_sdo(uint16_t index, uint8_t subindex, nser_axle *ns_axle,
		uint8_t *target, size_t target_size, size_t *result_size);

#define nser_sdo_set_brake(axle) nser_sdo_set_DO_Physical_outputs(axle, 0x01)
float nser_sdo_get_Position_encoder_resolution(nser_axle *ns_axle);
float nser_sdo_get_Velocity_encoder_resolution(nser_axle *ns_axle);
float nser_sdo_get_Gear_ratio(nser_axle *ns_axle);
int ecrt_slave_config_emerg_size(ec_slave_config_t *sc, size_t elements);

nser_master *create_new_master(unsigned int num);
nser_slave *create_new_slaves(unsigned int num);
nser_axle * create_new_axles(unsigned int num);

#ifdef EC_MASTER_IN_USERSPACE
int ecus_init(uint32_t master_count, const char *const *master_macs,
              uint32_t backup_count, const char *const *backup_macs,
              uint32_t dbg_level, uint32_t cycle_ns, uint32_t core_mask);
int ecus_done(void);

int ecus_slave_is_ready(uint32_t master_index, uint16_t alias, uint16_t position);
int ecus_master_operation_loop(ec_master_t *master, int cycle);

int ecus_bind_cpu_core(uint32_t core_mask);
#endif

typedef enum {
	ZERO = 0, /**< No registered process data were exchanged. */
	INCOMPLETE, /**< Some of the registered process data were exchanged. */
	COMPLETE /**< All registered process data were exchanged. */
} nser_domain_state;

typedef enum {
	UNKNOWN = 0x00,
	/**< unknown state */
	INIT = 0x01,
	/**< INIT state (no mailbox communication, no IO) */
	PREOP = 0x02,
	/**< PREOP state (mailbox communication, no IO) */
	BOOT = 0x03,
	/**< Bootstrap state (mailbox communication, firmware update) */
	SAFEOP = 0x04,
	/**< SAFEOP (mailbox communication and input update) */
	OP = 0x08,
	/**< OP (mailbox communication and input/output update) */
	ACK_ERR = 0x10
/**< Acknowledge/Error bit (no actual state) */
} nser_slave_state;

typedef enum {
	UN_KNOWN, /**< unknown state */
	ACTIVATE, /*the master has been activated */
	NO_SLAVE, /*Any slave is found on this bus*/
	NO_ALL_OP, /*There is at least one slave in this bus which doesn't enter into operation mode*/
	ALL_OP /*All slaves at this bus enter into operation mode*/
} nser_master_state;

struct _nser_sdo_entry {
	uint16_t index;
	uint8_t subindex;
	uint32_t v32;
	int lenbit;
};
struct _nser_reg_pdo_entry {
	uint16_t index;
	uint8_t subindex;
};

struct _nser_pdo_entry {
	uint8_t pdo_subindex;
	ec_pdo_entry_info_t *pdo_entry;
};

struct _nser_pdo_info {
	uint8_t pdo_index;
	uint8_t sync_subindex;
	ec_pdo_info_t *pdo_info;
	nser_pdo_entry *ns_pdo_entry;
	int ns_pdo_entry_num;
	int data_len;
};

struct _nser_sync_info {
	uint16_t index;
	ec_sync_info_t *sync_info;
	nser_pdo_info * ns_pdo_info;
	int ns_pdo_info_num;
};

struct _nser_slave {
	uint8_t index;
	nser_master *master;
	ec_slave_config_t *sc;
	ec_sync_info_t *sync_info;
	uint16_t slave_position;
	uint16_t alias;
	nser_axle *igh_axle;
	uint32_t VendorId;
	uint32_t ProductCode;
	size_t emerg_size;
	uint32_t shift_time;
	int dc;
	char *name;
	int force_pdo_assign;
	unsigned int master_index;
	nser_sdo_entry *ns_sdo_entry;
	int ns_sdo_entry_number;
	nser_sync_info *ns_sync_info;
	int ns_sync_info_num;
	uint16_t wd_divider;
	uint16_t wd_intervals;
	nser_slave_state ns_slave_state;

};

struct _nser_axle {
	unsigned int axle_offset;
	uint8_t axle_index;           // A slave may have many axles.
	nser_slave *slave;
	mode_of_operation mode;
	nser_master *nser_master;
	char *name;
	int pdos_offset[PDO_ENTRY_NUM_PER_AXLE];
	nser_reg_pdo_entry *ns_reg_pdo;
	int ns_reg_pdo_num;
	void *domain_dp;
	axle_state ns_axle_state;
	axle_state last_ns_axle_state;

};

struct _nser_master {
	unsigned int master_index;
	ec_master_t *ec_master;
	ec_domain_t *domain;
	void *domain_dp;
	nser_slave *slaves;
	unsigned int slave_number;
	nser_master_state ns_master_state;
	nser_domain_state ns_domain_state;
	nser_slave *reference_clock_slave;
};
typedef int (*nser_cycle_task_t)(nser_global_data *ns_data);
struct _nser_global_data {
	nser_master * ns_masteter;
	unsigned int num_master;
	nser_axle *ns_axles;
	unsigned int axle_number;
	uint32_t period_time;
	pthread_t cyclic_thread;
	int running;
	nser_cycle_task_t task;
	void *private_data;
	int is_auto_start;
	int cycle_counter;
	int master_status_update_freq;
	int slave_status_update_freq;
	int axle_status_update_freq;
	int sync_ref_update_freq;
};

#define nser_set_sched_priority(ns_data, data)				 ns_data->sched_priority = (data)
#define nser_set_master_status_update_freq(ns_data, data)	 ns_data->master_status_update_freq = (data)
#define nser_set_slave_status_update_freq(ns_data, data) 	ns_data->slave_status_update_freq = (data)
#define nser_set_axle_status_update_freq(ns_data, data) 	ns_data->axle_status_update_freq = (data)
#define nser_get_cycle_counter(ns_data)						ns_data->cycle_counter

#define nser_set_private_data(ns_data, data)   ns_data->private_data = (void *)(data)
#define nser_get_private_date(ns_data)		  ns_data->private_data

float nser_sdo_get_Position_encoder_resolution(nser_axle *ns_axle);
float nser_sdo_get_Velocity_encoder_resolution(nser_axle *ns_axle);
float nser_sdo_get_Gear_ratio(nser_axle *ns_axle);

int nser_xmlconfig(nser_global_data *ns_data, char *xmlfile);
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
#define nser_config_all_masters(ns_data) 	nser_config_master(ns_data, 0xff)
int nser_config_master(nser_global_data *ns_data, unsigned int master_index);

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
#define nser_config_all_axles(ns_data) nser_config_axle(ns_data, 0xff)
int nser_config_axle(nser_global_data *ns_data, unsigned int axle_index);

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

#define nser_deactivate_all_masters(ns_data) nser_deactivate_master(ns_data, 0xff)
void nser_deactivate_master(nser_global_data *ns_data,
		unsigned int master_index);
#define nser_activate_all_masters(ns_data) nser_activate_master(ns_data, 0xff)
int nser_activate_master(nser_global_data *ns_data, unsigned int master_index);
int user_cycle_task_start(nser_global_data *ns_data, nser_cycle_task_t task,
		int isAutoStart);
void user_cycle_task_stop(nser_global_data *ns_data);
nser_global_data *nser_app_run_init(char *xmlfile);
nser_global_data *nser_app_run_init_without_activate(char *xmlfile);
/****************************************************************************/

#endif /* NSERVO_H_ */
