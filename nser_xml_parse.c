// SPDX-License-Identifier: (GPL-2.0 OR MIT)
/*
 * Copyright 2018-2020 NXP
 */

#include <stdio.h>
#include <sched.h>
#include <string.h>
#include <libxml2/libxml/xmlreader.h>
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>
#include <libxml2/libxml/HTMLparser.h>
#include <libxml2/libxml/xmlmemory.h>
#include "config.h"
#include "nservo.h"
#include "debug.h"
//gcc -o example  `xml2-config --cflags` test_xml.c `xml2-config --libs`

static int64_t xml_str_to_value(const xmlChar *str) {
	int64_t ret;
	if (str[0] != '#') {
		return -1;
	}

	if (str[1] == 'x')
		sscanf((char *) &str[2], "%lx", &ret);
	else
		sscanf((char *) &str[1], "%ld", &ret);
	return ret;
}

static char *xml_str_dump(const xmlChar *str) {
	if (str[0] == '#') {
		return NULL;
	}
	return (char*) xmlStrdup(str);
}

static int find_node_number(xmlNode * a_node, const char *name) {
	xmlNode *cur_node = NULL;
	int num = 0;
	for (cur_node = a_node->children; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE
				&& !xmlStrcasecmp(cur_node->name, (const xmlChar *) name)) {
			num++;
		}
	}
	return num;
}

static xmlNode *find_subnode(xmlNode * a_node, const xmlChar *name) {
	xmlNode *cur_node = NULL;

	for (cur_node = a_node->children; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE
				&& !xmlStrcasecmp(cur_node->name, name)) {
			return cur_node;
		}
	}
	return NULL;
}

static xmlNode *find_subnode_index(xmlNode * a_node, const char *name,
		int index) {
	xmlNode *cur_node = NULL;
	int i = 0;
	for (cur_node = a_node->children; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE
				&& !xmlStrcasecmp(cur_node->name, (const xmlChar *) name)) {
			if (i == index)
				return cur_node;
			else
				i++;
		}
	}
	return NULL;
}

//static xmlNode *find_node(xmlNode * a_node, const char *name) {
//	xmlNode *cur_node = NULL;
//
//	for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
//		if (cur_node->type == XML_ELEMENT_NODE
//				&& !xmlStrcasecmp(cur_node->name, ( const xmlChar *)name)) {
//			return cur_node;
//		}
//	}
//	return NULL;
//}
static int64_t get_attr(xmlNode * a_node, const xmlChar *name) {
	xmlChar *ret;
	if (!(ret = xmlGetProp(a_node, name))) {
		return -1;
	}
	return xml_str_to_value(ret);
}

#define find_node_slaves(a_node) 			find_subnode(a_node, (const xmlChar *)"Slaves")
#define find_node_SyncManagers(a_node) 		find_subnode(a_node, (const xmlChar *)"SyncManagers")
#define find_node_RunParamter(a_node) 		find_subnode(a_node, (const xmlChar *)"RunParamter")
#define find_node_Masters(a_node) 			find_subnode(a_node, (const xmlChar *)"Masters")
#define find_node_Sdos(a_node) 				find_subnode(a_node, (const xmlChar *)"Sdos")
#define find_node_Axles(a_node)				find_subnode(a_node, (const xmlChar *)"Axles")
#define find_node_Index(a_node)				find_subnode(a_node, (const xmlChar *)"Index")
#define find_node_SubIndex(a_node)			find_subnode(a_node, (const xmlChar *)"SubIndex")
#define find_node_WatchDog(a_node)			find_subnode(a_node, (const xmlChar *)"WatchDog")
#define find_node_DC(a_node)				find_subnode(a_node, (const xmlChar *)"DC")
#define find_node_SYNC(a_node)				find_subnode(a_node, (const xmlChar *)"SYNC")

#define get_attr_master_index(a_node)		get_attr(a_node, (const xmlChar *)"master_index")
#define get_attr_SubIndex(a_node)			get_attr(a_node, (const xmlChar *)"SubIndex")
#define get_attr_slave_position(a_node)		get_attr(a_node, (const xmlChar *)"slave_position")
#define get_attr_Slave(a_node)	 			get_attr(a_node, (const xmlChar *)"Slave")
#define get_attr_alias(a_node)	 			get_attr(a_node, (const xmlChar *)"alias")
#define get_attr_AxleIndex(a_node)	 		get_attr(a_node, (const xmlChar *)"AxleIndex")
#define get_attr_AxleIndex(a_node)	 		get_attr(a_node, (const xmlChar *)"AxleIndex")
#define get_attr_force_pdo_assign(a_node)	get_attr(a_node, (const xmlChar *)"force_pdo_assign")
#define get_attr_AxleOffset(a_node)			get_attr(a_node, (const xmlChar *)"AxleOffset")
//

#define get_node_value(node) xmlNodeGetContent(node)

static int64_t get_subnode_value(xmlNode * a_node, char *sub_name) {
	xmlNode *cur_node;
	xmlChar *ret;

	if (!(cur_node = find_subnode(a_node, (xmlChar *) sub_name))) {
		debug_warning("Can not find node %s\n", sub_name);
		return -1;
	}
	if (!(ret = get_node_value(cur_node))) {
		debug_warning("Can not get %s's value\n", sub_name);
		return -1;
	}

	return xml_str_to_value(ret);
}

static char *get_subnode_value_str(xmlNode * a_node, char *sub_name) {
	xmlNode *cur_node;
	xmlChar *ret;

	if (!(cur_node = find_subnode(a_node, (xmlChar *) sub_name))) {
		debug_warning("Can not find node %s\n", sub_name);
		return NULL;
	}
	if (!(ret = get_node_value(cur_node))) {
		debug_warning("Can not get %s's value\n", sub_name);
		return NULL;
	}

	return xml_str_dump(ret);
}

static int xml_config_sdo(nser_sdo_entry *ns_sdo_entry, xmlNode * sdo_node) {
	int64_t ret;
	char *name;

	ret = get_subnode_value(sdo_node, "Index");
	if (ret < 0) {
		debug_error("Failed to get sdo Index\n");
		return -1;
	}
	ns_sdo_entry->index = (uint16_t) ret;

	ret = get_subnode_value(sdo_node, "SubIndex");
	if (ret < 0) {
		debug_error("Failed to get sdo SubIndex\n");
		return -1;
	}
	ns_sdo_entry->subindex = (uint8_t) ret;

	ret = get_subnode_value(sdo_node, "value");
	if (ret < 0) {
		debug_error("Failed to get sdo value\n");
		return -1;
	}
	ns_sdo_entry->v32 = (uint32_t) ret;

	ret = get_subnode_value(sdo_node, "BitLen");
	if (ret < 0) {
		debug_error("Failed to get sdo BitLen\n");
		return -1;
	}
	ns_sdo_entry->lenbit = (int) ret;

	if (ns_sdo_entry->lenbit > 32) {
		debug_error("sdo BitLen = %d is bigger then 32\n",
				ns_sdo_entry->lenbit);
		return -1;
	}
	name = get_subnode_value_str(sdo_node, "Name");
	debug_info("   Sdo config: name =%s Index = %d SubIndex = %d Value = %u BitLen = %d\n", name,
			ns_sdo_entry->index, ns_sdo_entry->subindex, ns_sdo_entry->v32, ns_sdo_entry->lenbit);
	xmlFree(name);
	return 0;

}

static int xml_config_entry(nser_slave *slave, xmlNode *entry_node,
		ec_pdo_entry_info_t *entry_info, nser_pdo_entry *ns_info) {
	int64_t ret;

	ret = get_subnode_value(entry_node, "Index");
	if (ret < 0) {
		debug_error("Failed to get entry Index\n");
		return -1;
	}
	entry_info->index = (uint16_t) ret;

	ret = get_subnode_value(entry_node, "SubIndex");
	if (ret < 0) {
		debug_error("Failed to get entry SubIndex\n");
		return -1;
	}
	entry_info->subindex = (uint8_t) ret;

	ret = get_subnode_value(entry_node, "BitLen");
	if (ret < 0) {
		debug_error("Failed to get entry BitLen\n");
		return -1;
	}
	entry_info->bit_length = (uint8_t) ret;
	ret = get_attr_SubIndex(entry_node);
	if (ret < 0) {
		debug_error("Failed to get entry attr SubIndex\n");
		return -1;
	}
	ns_info->pdo_subindex = (uint8_t) ret;
	ns_info->pdo_entry = entry_info;
	return 0;

}

static int xml_config_pdo(nser_slave *slave, xmlNode * pdo_node,
		ec_pdo_info_t *pdo_info, nser_pdo_info *ns_info) {
	uint16_t index;
	uint8_t subindex;
	int i;
	int entry_num;
	int64_t ret;
	xmlNode * entry_node;
	ec_pdo_entry_info_t *entry_info;
	ret = get_subnode_value(pdo_node, "Index");
	if (ret < 0) {
		debug_error("Failed to get sync Index\n");
		return -1;
	}
	index = (uint16_t) ret;
	pdo_info->index = index;

	ret = get_attr_SubIndex(pdo_node);
	if (ret < 0) {
		debug_error("Failed to get sync SubIndex\n");
		return -1;
	}
	subindex = (uint8_t) ret;

	entry_num = find_node_number(pdo_node, "Entry");
	pdo_info->n_entries = entry_num;
	ns_info->pdo_index = index;
	ns_info->sync_subindex = subindex;
	ns_info->pdo_info = pdo_info;
	ns_info->ns_pdo_entry_num = entry_num;
	ns_info->data_len = 0;
	if (entry_num > 0) {
		if (!(entry_info = malloc(
				(sizeof(ec_pdo_entry_info_t) + sizeof(nser_pdo_entry))
						* (entry_num)))) {
			debug_error("Failed to malloc memory to entry_info node \n");
			return -1;
		}
		ns_info->ns_pdo_entry = (nser_pdo_entry *) (entry_info + entry_num);

		for (i = 0; i < entry_num; i++) {
			if (!(entry_node = find_subnode_index(pdo_node, "Entry", i))) {
				debug_error("Failed to find pdo 0x%x Entry%d node\n",
						pdo_info->index, i);
				return -1;
			}
			if (xml_config_entry(slave, entry_node, &entry_info[i],
					&ns_info->ns_pdo_entry[i])) {
				debug_error("Failed to get pdo 0x%x Entry%d subindex \n",
						pdo_info->index, i);
				return -1;
			} debug_info("    Pdo config (0x%x:0x%x) entry: index = %x subindex = %d bitlen =  %d\n",
					pdo_info->index, ns_info->ns_pdo_entry[i].pdo_subindex,
					entry_info[i].index, entry_info[i].subindex, entry_info[i].bit_length);
			ns_info->data_len += entry_info[i].bit_length / 8;

		}

		pdo_info->entries = entry_info;
	}

	return 0;
}

static void xml_config_watchdog(nser_slave *slave, xmlNode * watchdog_node) {
	int64_t ret;

	ret = get_subnode_value(watchdog_node, "Divider");
	if (ret < 0) {
		debug_warning(
				"Failed to get watchdog Divider node, The default value will be used\n");
		slave->wd_divider = 0;
	} else {
		slave->wd_divider = (uint16_t) ret;
	}

	ret = get_subnode_value(watchdog_node, "intervals");
	if (ret < 0) {
		debug_warning(
				"Failed to get watchdog Intervals node, The default value will be used\n");
		slave->wd_intervals = 0;
	} else {
		slave->wd_intervals = (uint16_t) ret;
	} debug_info("  Find watchdog config: divider=%d intervals=%d\n", slave->wd_divider, slave->wd_intervals);
}

static int xml_config_sync(nser_slave *slave, xmlNode * sync_node,
		ec_sync_info_t *sync_info, nser_sync_info *ns_info) {
	int64_t ret;
	uint16_t index;
	//char *name;
	char *dir_str, *watch_mode_str;
	unsigned int pdos_num, i;
	xmlNode * pdo_node;
	ec_pdo_info_t *pdo_info;

	ret = get_subnode_value(sync_node, "Index");
	if (ret < 0) {
		debug_error("Failed to get sync Index\n");
		return -1;
	}
	index = (uint16_t) ret;

	ret = get_attr_SubIndex(sync_node);
	if (ret < 0) {
		debug_error("Failed to get sync SubIndex\n");
		return -1;
	}
	sync_info->index = (uint8_t) ret;

	//name = get_subnode_value_str(sync_node, "Name");
	dir_str = get_subnode_value_str(sync_node, "Dir");
	watch_mode_str = get_subnode_value_str(sync_node, "Watchdog");

	if (!xmlStrcasecmp((xmlChar *) dir_str, (xmlChar *) "OUTPUT")) {
		sync_info->dir = EC_DIR_OUTPUT;
	} else {
		sync_info->dir = EC_DIR_INPUT;
	}

	if (!xmlStrcasecmp((xmlChar *) watch_mode_str, (xmlChar *) "DISABLE")) {
		sync_info->watchdog_mode = EC_WD_DISABLE;
	} else {
		sync_info->watchdog_mode = EC_WD_ENABLE;
	}

	pdos_num = find_node_number(sync_node, "Pdo");
	sync_info->n_pdos = pdos_num;

	debug_info("   Find a sync(0x%x) node: DIR=%s WD=%s PDO_NUM=%d\n",
			index, dir_str, watch_mode_str, pdos_num);
	ns_info->index = index;
	ns_info->sync_info = sync_info;
	ns_info->ns_pdo_info_num = pdos_num;
	if (pdos_num > 0) {
		if (!(pdo_info = malloc(
				(sizeof(ec_pdo_info_t) + sizeof(nser_pdo_info)) * (pdos_num)))) {
			debug_error("Failed to malloc memory to pdo_info node\n");
			return -1;
		}
		ns_info->ns_pdo_info = (nser_pdo_info *) (pdo_info + pdos_num);

		for (i = 0; i < pdos_num; i++) {
			if (!(pdo_node = find_subnode_index(sync_node, "Pdo", i))) {
				debug_error("Failed to find sm%d Pdo%d\n", sync_info->index, i);
				return -1;
			}
			if ((xml_config_pdo(slave, pdo_node, &pdo_info[i],
					&ns_info->ns_pdo_info[i])) < 0) {
				debug_error("Failed to get smd%d pdo%d subindex\n",
						sync_info->index, i);
				return -1;
			}

		}
		sync_info->pdos = pdo_info;
	}
	return 0;
}

static int xml_create_new_slaves(xmlNode * master_node, nser_master *master) {
	xmlNode *slave_node, *syncs_node, *sync_node, *sdos_node, *sdo_node,
			*dc_node, *watchdog_node;
	int slaves_num, sdos_num, syncs_num;
	ec_sync_info_t *sync_info;
	int64_t ret;
	int i, j;
	nser_slave *slaves, *slave;
	nser_sdo_entry *sdo_entrys = NULL;

	if (!(slaves_num = find_node_number(master_node, "Slave"))) {
		debug_error("Failed to the number of Slave node on master%d\n",
				master->master_index);
		return -1;
	}

	debug_info("  Find %d slaves\n", slaves_num);

	if ((slaves = create_new_slaves(slaves_num)) == NULL) {
		debug_error("Failed to create new slaves on master%d\n",
				master->master_index);
		return -1;
	}
	/*Configuration for every slave */
	for (i = 0; i < slaves_num; i++) {
		if (!(slave_node = find_subnode_index(master_node, "slave", i))) {
			debug_error("Failed to find slave%d on master%d\n", i,
					master->master_index);
			goto free_slaves;
		}
		/*Get slave's position, vendorId, produceID and alias*/
		ret = get_attr_slave_position(slave_node);
		if (ret < 0) {
			debug_error(
					"Failed to get slave%d attr slave_position on master%d\n",
					i, master->master_index);
			goto free_slaves;
		}
		slave = &slaves[(uint16_t) ret];
		slave->slave_position = (uint16_t) ret;
//		if (slave->slave_position >= slaves_num) {
//			debug_error(
//					"The position of slave%d exceed the sum off all slaves on master%d\n",
//					slave->slave_position, master->master_index);
//			return -1;
//		}

		if (slave->VendorId != 0) {
			debug_error(
					"The position of slave%d has been occupied on master%d\n",
					i, master->master_index);
			goto free_slaves;
		}

		ret = get_subnode_value(slave_node, "VendorId");
		if (ret < 0) {
			debug_error("Failed to get slave%d VendorId on master%d\n", i,
					master->master_index);
			goto free_slaves;
		}
		slave->VendorId = (uint32_t) ret;

		ret = get_subnode_value(slave_node, "ProductCode");
		if (ret < 0) {
			debug_error("Failed to get slave%d ProductCode on master%d\n", i,
					master->master_index);
			goto free_slaves;
		}
		slave->ProductCode = (uint32_t) ret;

		slave->name = get_subnode_value_str(slave_node, "name");

		ret = get_attr_alias(slave_node);
		if (ret < 0) {
			debug_error("Failed to get slave%d attr alias on master%d\n", i,
					master->master_index);
			goto free_slave_name;
		}
		slave->alias = (uint32_t) ret;

		if ((dc_node = find_node_DC(slave_node))) {
			if ((dc_node = find_node_SYNC(dc_node))) {

				ret = get_subnode_value(dc_node, "Shift");
				if (ret < 0) {
					slave->dc = 0;

				} else {
					slave->dc = 1;
					slave->shift_time = (uint32_t) ret;
				}
			}
		}

		debug_info("  Find a slave with: VID=0x%x PID=0x%x position=%d  alias=%d\n",
				slave->VendorId, slave->ProductCode, slave->slave_position, slave->alias);
		if (slave->dc) {
			debug_info("   Find DC configuration with shift_time = %u\n", slave->shift_time);
		}
		/* Start to config sdo entry*/
		if ((sdos_node = find_node_Sdos(slave_node))) {

			if ((sdos_num = find_node_number(sdos_node, "sdo"))) {
				debug_info("   Find %d sdo entry\n", sdos_num);
				if (!(sdo_entrys = malloc(sizeof(nser_sdo_entry) * (sdos_num)))) {
					debug_error(
							"Failed to malloc memory to nser_sdo_entry on slave%d master%d\n",
							i, master->master_index);
					goto free_slave_name;
				}
				for (j = 0; j < sdos_num; j++) {
					if (!(sdo_node = find_subnode_index(sdos_node, "sdo", j))) {
						debug_error(
								"Failed to find sdo%d on slave%d master%d\n", j,
								i, master->master_index);
						goto free_sdo_entrys;
					}
					if ((xml_config_sdo(&sdo_entrys[j], sdo_node))) {
						debug_error(
								"Failed to config sdo%d on slave%d master%d\n",
								j, i, master->master_index);
						goto free_sdo_entrys;
					}
				}
				slave->ns_sdo_entry = sdo_entrys;
				slave->ns_sdo_entry_number = sdos_num;
			}
		}

		/* Initial WatchDog*/
		if (!(watchdog_node = find_node_WatchDog(slave_node))) {
			debug_info(
					"  No WatchDog node found. The default watchdog setting will be used\n");
		} else {
			xml_config_watchdog(slave, watchdog_node);
		}

		/* Initial emergency ring buffer size*/
		if ((ret = get_subnode_value(slave_node, "Emerg_size")) < 0) {
			debug_info(
					"  No Emerg_size node, The Emergency message will be dropped\n");
		} else {
			slave->emerg_size = (size_t) ret;
			debug_info("  Emerg_size is %lu\n", slave->emerg_size);
		}

		/* Initial SyncManager*/
		if (!(syncs_node = find_node_SyncManagers(slave_node))) {
			debug_error(
					"  Failed to find SyncManagers node on slave%d master%d\n",
					i, master->master_index);
			goto free_sdo_entrys;
		}

		if ((ret = get_attr_force_pdo_assign(syncs_node)) < 0) {
			debug_info("  No force_pdo_assign attr found,the default is 0\n");
			slave->force_pdo_assign = 0;
		} else {
			slave->force_pdo_assign = (int) ret;
			debug_info("  Force_pdo_assig is %d\n", slave->force_pdo_assign);
		}

		if (!(syncs_num = find_node_number(syncs_node, "SyncManager"))) {
			debug_error(
					"Failed to find the number of SyncManager node on slave%d master%d\n",
					i, master->master_index);
			goto free_sdo_entrys;
		}

		if (!(sync_info = malloc(
				(sizeof(ec_sync_info_t) * (syncs_num + 1))
						+ (sizeof(nser_sync_info) * syncs_num)))) {
			debug_error(
					"Failed to malloc memory to sync_info on slave%d master%d\n",
					i, master->master_index);
			goto free_sdo_entrys;
		}
		slave->ns_sync_info = (nser_sync_info *) (sync_info + (syncs_num + 1));
		slave->ns_sync_info_num = syncs_num;
		debug_info("  Find %d SM\n", syncs_num);

		for (j = 0; j < syncs_num; j++) {
			if (!(sync_node = find_subnode_index(syncs_node, "SyncManager", j))) {
				debug_error(
						"Failed to find SyncManager%d node on slave%d master%d\n",
						j, i, master->master_index);
				goto free_sync_info;
			}
			if (xml_config_sync(slave, sync_node, &sync_info[j],
					&slave->ns_sync_info[j])) {
				debug_error(
						"Failed to config SyncManager%d node on slave%d master%d\n",
						j, i, master->master_index);
				goto free_sync_info;
			}
		}
		sync_info[syncs_num].index = 0xff;
		slave->sync_info = sync_info;

//		if (ecrt_slave_config_pdos(slave->sc, EC_END, slave->sync_info)) {
//			debug_error("Failed to configure PDOs on %s.\n", slave->name);
//			return -1;
//		}
	}
	master->slaves = slaves;
	master->slave_number = slaves_num;
	return 0;

free_sync_info:
	free(sync_info);
free_sdo_entrys:
	if (sdo_entrys != NULL) {
		free(sdo_entrys);
	}
free_slave_name:
	free(slave->name);
free_slaves:
	free(slaves);
	return -1;
}

static int xml_create_new_master(xmlNode * root_element,
		nser_global_data *ns_data) {
	xmlNode * masters_node, *master_node;
	int num, i, j;
	nser_master *ns_master;
	nser_master *master;
	int64_t ret;
	uint32_t position;
	if (!(masters_node = find_node_Masters(root_element))) {
		debug_error("Failed to find Masters node\n");
		return -1;
	}

	if (!(num = find_node_number(masters_node, "Master"))) {
		debug_error("Failed to get number of Master node\n");
		return -1;
	} debug_info("Find %d masters node\n", num);
	if (!(ns_master = create_new_master(num))) {
		debug_error("Failed to create new masters\n");
		return -1;
	}

	/* Configuration for every master*/
	for (i = 0; i < num; i++) {
		if (!(master_node = find_subnode_index(masters_node, "Master", i))) {
			debug_error("Failed to find master%d\n", i);
			goto free_ns_master;
		}
		master = ns_master + i;

		ret = get_subnode_value(master_node, "Master_index");
		if (ret < 0) {
			debug_error("Failed to get master%d Master_index\n", i);
			goto free_ns_master;
		}
		master->master_index = (unsigned int) ret;
		debug_info(" Start to find slaves for master%d\n", master->master_index);
		if (xml_create_new_slaves(master_node, master)) {
			debug_error("Failed to create slaves for master(%d)\n",
					master->master_index);
			goto free_ns_master;
		}

		/* Configure reference clock;*/

		if ((ret = get_subnode_value(master_node, "Reference_clock")) < 0) {
			debug_warning(
					"Failed to find Reference_clock node, will use default slave as DC reference clock\n");
		} else {
			position = (uint32_t) ret;

			for (j = 0; j < master->slave_number; j++) {
				if (position == master->slaves[j].slave_position) {
					master->reference_clock_slave = &master->slaves[j];
					break;
				}
			}
			if (!master->reference_clock_slave) {
				debug_warning(
						"Failed to find slave_position%d for setting Reference clock\n",
						position);
			} else {
				debug_info(" Slave%d will be used as the Reference_clock for master%d\n",
						master->reference_clock_slave->slave_position ,master->master_index);
			}
		}

	}
	ns_data->ns_masteter = ns_master;
	ns_data->num_master = num;
	return 0;

free_ns_master:
	free(ns_master);
	return -1;
}

static nser_slave *get_ns_slave(nser_master *ns_master, uint16_t position) {
	int i;
	for (i = 0; i < ns_master->slave_number; i++) {
		if (ns_master->slaves[i].slave_position == position)
			return &(ns_master->slaves[i]);
	}
	return NULL;
}

static int xml_config_reg_pdo(xmlNode *reg_pdo, nser_axle *ns_axle,
		nser_reg_pdo_entry *ns_info) {
	uint8_t subindex;
	uint16_t index;
	int64_t ret;
	ret = get_subnode_value(reg_pdo, "Index");
	if (ret < 0) {
		debug_error("Failed to get reg_pdo Index\n");
		return -1;
	}
	index = (uint16_t) ret;

	ret = get_subnode_value(reg_pdo, "SubIndex");
	if (ret < 0) {
		debug_error("Failed to get reg_pdo SubIndex\n");
		return -1;
	}
	subindex = (uint8_t) ret;

	ns_info->index = index;
	ns_info->subindex = subindex;
	return 0;
}

static int xml_config_axles(xmlNode *root_element, nser_global_data *ns_data) {
	xmlNode * axles_node, *axle_node, *reg_pdo_node;
	nser_axle *ns_axles;
	nser_master *ns_master;
	nser_slave *ns_slave;
	int axles_num, i, reg_pdo_num, j;
	unsigned int master_index;
	uint16_t slave_position;
	unsigned int axle_index;
	nser_reg_pdo_entry *ns_info;
	int64_t ret;
	char *mode;
	unsigned int axle_offset = 0;

	if (!(axles_node = find_node_Axles(root_element))) {
		debug_error("Failed to find axles node\n");
		return -1;
	}

	if (!(axles_num = find_node_number(axles_node, "Axle"))) {
		debug_error("Failed to get number of Axle node\n");
		return -1;
	}

	if (!(ns_axles = create_new_axles(axles_num))) {
		debug_error("Failed to create new axles\n");
		return -1;
	} debug_info("Find %d Axles\n",axles_num);
	for (i = 0; i < axles_num; i++) {
		if (!(axle_node = find_subnode_index(axles_node, "Axle", i))) {
			debug_error("Failed to find Axle%d node\n", i);
			goto free_ns_axles;
		}

		ret = get_attr_master_index(axle_node);
		if (ret < 0) {
			debug_error("Failed to get Axle%d attr master_index \n", i);
			goto free_ns_axles;
		}
		master_index = (unsigned int) ret;

		ret = get_attr_slave_position(axle_node);
		if (ret < 0) {
			debug_error("Failed to get Axle%d attr slave_position \n", i);
			goto free_ns_axles;
		}
		slave_position = (uint16_t) ret;

		ret = get_attr_AxleIndex(axle_node);
		if (ret < 0) {
			debug_error("Failed to get Axle%d attr AxleIndex \n", i);
			goto free_ns_axles;
		}
		axle_index = (unsigned int) ret;

		if (axle_index > axles_num) {
			debug_error("The axle_index %d is excess the sum of all axles",
					axle_index);
			goto free_ns_axles;
		}

		ret = get_attr_AxleOffset(axle_node);
		if (ret < 0) {
			debug_warning("No Axle%d attr AxleOffset, will use the default value 0\n", i);
			ret = 0;
		}
		axle_offset = (unsigned int) ret;
		if (master_index >= ns_data->num_master) {
			debug_error(
					"Failed to Axle%d attr master_index is bigger then the number of master%d\n",
					i, ns_data->num_master);
			goto free_ns_axles;
		} debug_info(" Find axle%d on master%d slave%d bus with axle_offset=%d\n",
				axle_index, master_index, slave_position, axle_offset);
		ns_master = &(ns_data->ns_masteter[master_index]);

		if (!(ns_slave = get_ns_slave(ns_master, slave_position))) {
			debug_error("Failed to find slave for Axle%d on master%d\n", i,
					ns_master->master_index);
			goto free_ns_axles;
		}

		if (ns_axles[axle_index].slave) {
			debug_error("The axle_index %d has been occupied\n", axle_index);
			goto free_ns_axles;
		}

		ns_axles[axle_index].slave = ns_slave;
		ns_axles[axle_index].nser_master = ns_master;
		ns_axles[axle_index].axle_index = axle_index;
		ns_axles[axle_index].name = get_subnode_value_str(axle_node, "Name");
		ns_axles[axle_index].axle_offset = axle_offset;

		mode = get_subnode_value_str(axle_node, "Mode");

		if (mode == NULL)
			ns_axles[axle_index].mode = op_mode_no;
		else if (mode[0] == 'p' && mode[1] == 'p')
			ns_axles[axle_index].mode = op_mode_pp;
		else if (mode[0] == 'v' && mode[1] == 'l')
			ns_axles[axle_index].mode = op_mode_vl;
		else if (mode[0] == 'p' && mode[1] == 'v')
			ns_axles[axle_index].mode = op_mode_pv;
		else if (mode[0] == 'h' && mode[1] == 'm')
			ns_axles[axle_index].mode = op_mode_hm;
		else if (mode[0] == 'i' && mode[1] == 'p')
			ns_axles[axle_index].mode = op_mode_ip;
		else if (mode[0] == 'c' && mode[2] == 'p')
			ns_axles[axle_index].mode = op_mode_csp;
		else if (mode[0] == 'c' && mode[2] == 'v')
			ns_axles[axle_index].mode = op_mode_csv;
		else if (mode[0] == 'c' && mode[1] == 't')
			ns_axles[axle_index].mode = op_mode_cst;
		else
			ns_axles[axle_index].mode = op_mode_no;

		debug_info(" 	axle%d works on %s mode\n", axle_index, get_mode_of_operation_str(ns_axles[axle_index].mode));

		if (!(reg_pdo_num = find_node_number(axle_node, "reg_pdo"))) {
			debug_error("Failed to get number of reg_pdo node on axle%d\n", i);
			goto free_ns_axles;
		} debug_info(" 	axle%d find %d reg_pdo\n", axle_index, reg_pdo_num);
		if (!(ns_info = malloc(sizeof(nser_reg_pdo_entry) * reg_pdo_num))) {
			debug_error(
					"Failed to malloc memory to nser_reg_pdo_entry on axle%d \n",
					i);
			goto free_ns_axles;
		}
		for (j = 0; j < reg_pdo_num; j++) {
			if (!(reg_pdo_node = find_subnode_index(axle_node, "reg_pdo", j))) {
				debug_error("Failed to find reg_pdo%d on axle%d\n", j, i);
				goto free_ns_info;
			}
			if (xml_config_reg_pdo(reg_pdo_node, &ns_axles[i], &ns_info[j])) {
				debug_error("Failed to config reg_pdo%d on axle%d\n", j, i);
				goto free_ns_info;
			} debug_info(" 	 Register pdo(0x%x:0x%x) to axle%d \n", ns_info[j].index, ns_info[j].subindex, axle_index);
		}
		ns_axles[axle_index].ns_reg_pdo = ns_info;
		ns_axles[axle_index].ns_reg_pdo_num = reg_pdo_num;
	}
	ns_data->axle_number = axles_num;
	ns_data->ns_axles = ns_axles;
	return 0;

free_ns_info:
	free(ns_info);
free_ns_axles:
	free(ns_axles);
	return -1;
}

static int xml_config_global_data(nser_global_data *ns_data,
		xmlNode *root_element) {
	int64_t ret;

	ret = get_subnode_value(root_element, "PeriodTime");
	if (ret < 0) {
		debug_warning("Failed to get PeriodTime, default value is 1ms\n");
		ns_data->period_time = 1000000;
	} else {
		ns_data->period_time = (uint32_t) ret;
	}

	ret = get_subnode_value(root_element, "master_status_update_freq");
	if (ret < 0) {
		ns_data->master_status_update_freq = 1000;
	} else
		ns_data->master_status_update_freq = (int) ret;

	ret = get_subnode_value(root_element, "slave_status_update_freq");
	if (ret < 0) {
		ns_data->slave_status_update_freq = 500;
	} else
		ns_data->slave_status_update_freq = (int) ret;

	ret = get_subnode_value(root_element, "axle_status_update_freq");
	if (ret < 0) {
		ns_data->axle_status_update_freq = 100;
	} else
		ns_data->axle_status_update_freq = (int) ret;

	ret = get_subnode_value(root_element, "sync_ref_update_freq");
	if (ret < 0) {
		ns_data->sync_ref_update_freq = 5;
	} else
		ns_data->sync_ref_update_freq = (int) ret;

	ret = get_subnode_value(root_element, "sched_priority");
	if (ret < 0) {
		ns_data->sched_priority = 82;
	} else
		ns_data->sched_priority = (int) ret;

	char *policy = get_subnode_value_str(root_element, "sched_policy");
	if (!policy) {
		ns_data->sched_policy = SCHED_OTHER;
	} else if ( !strncmp(policy, "SCHED_FIFO", 10)) {
		ns_data->sched_policy = SCHED_FIFO;
	} else if ( !strncmp(policy, "SCHED_RR", 8)) {
		ns_data->sched_policy = SCHED_RR;
	}else {
		debug_error("Unknown sched_policy: %s\n", policy);
		return -1;
	}

	if ((xml_create_new_master(root_element, ns_data))) {
		debug_error("Failed to creat new master\n");
		return -1;
	}
	if ((xml_config_axles(root_element, ns_data))) {
		debug_error("Failed to config  axles\n");
		return -1;
	}
	ns_data->cycle_counter = 0;
	return 0;
}

int nser_xmlconfig(nser_global_data *ns_data, char *xmlfile) {
	xmlDoc *doc = NULL;
	xmlNode *root_element = NULL;
	int ret = 0;
	/*
	 * this initialize the library and check potential ABI mismatches
	 * between the version it was compiled for and the actual shared
	 * library used.
	 */
	LIBXML_TEST_VERSION

	/*parse the file and get the DOM */
	doc = xmlReadFile(xmlfile, NULL, 0);

	if (doc == NULL) {
		debug_error("Failed to open xml file %s\n", xmlfile);
		return -1;
	}

	/*Get the root element node */
	root_element = xmlDocGetRootElement(doc);
	ret = xml_config_global_data(ns_data, root_element);

	xmlFreeDoc(doc);
	xmlCleanupParser();

	return ret;
}
