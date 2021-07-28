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
#define MAX 80
#define PORT 6000
#define SA struct sockaddr

int run = 1;

void signal_handler(int sig) {
	run = 0;
}

struct axle_status_t {
	uint8_t mode;
	int32_t current_position;
	int32_t target_position;
	int32_t profile_speed;
	int32_t current_speed;
	int32_t target_speed;
	int is_init_done;
};

struct axle_status_t  *axle_status;

static int axle_start(nser_axle *ns_axle, uint16_t status, uint8_t mode) {
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

static int update_target_position(nser_axle *ns_axle, struct axle_status_t *axle_status)
{
	uint16_t statusword = 0;
	uint16_t controlword = 0;

	controlword = nser_pdo_get_Controlword(ns_axle);
	statusword = nser_pdo_get_Statusword(ns_axle);

	if (axle_start(ns_axle, statusword, op_mode_pp) != 1) {
		return 0;
	}

	if (ns_axle->nser_master->ns_master_state == ALL_OP) {
		axle_status->current_speed = nser_pdo_get_Velocity_actual_value(ns_axle);
		axle_status->current_position = nser_pdo_get_Position_actual_value(ns_axle);
		
		if (axle_status->is_init_done == 0) {
			axle_status->target_position = axle_status->current_position;
			nser_pdo_set_Profile_velocity(ns_axle,
				axle_status->profile_speed);
			nser_pdo_set_Target_position(ns_axle,
				axle_status->target_position);
			nser_pdo_set_Controlword(ns_axle,
				contrlword_new_set_point(controlword));
			axle_status->is_init_done = 1;
		}
		else if (axle_status->is_init_done == 1) {
			if (is_status_target_reached(statusword)) {
				nser_pdo_set_Target_position(ns_axle,
					axle_status->target_position);
				nser_pdo_set_Profile_velocity(ns_axle,
					axle_status->profile_speed);
				nser_pdo_set_Controlword(ns_axle,
					contrlword_new_set_point(controlword));
				axle_status->is_init_done = 2;
			}
		}
		else if (axle_status->is_init_done == 2) {
			nser_pdo_set_Controlword(ns_axle, 0xf);
			axle_status->is_init_done = 1;
		}
	}
	return 0;
}

static int update_target_velocity(nser_axle *ns_axle, struct axle_status_t *axle_status)
{
	uint16_t statusword = 0;

	statusword = nser_pdo_get_Statusword(ns_axle);

	if (axle_start(ns_axle, statusword, op_mode_pv) != 1) {
		return 0;
	}

	if (ns_axle->nser_master->ns_master_state == ALL_OP) {

		axle_status->current_speed = nser_pdo_get_Velocity_actual_value(
				ns_axle);
		axle_status->current_position = nser_pdo_get_Position_actual_value(
				ns_axle);

		if (!axle_status->is_init_done) {
			axle_status->target_speed = axle_status->current_speed;
			axle_status->is_init_done = 1;
		}

		nser_pdo_set_Target_velocity(ns_axle, axle_status->target_speed);
	}
	return 0;
}

int main_task(nser_global_data *ns_data)
{
	int i;
	nser_axle *ns_axle;
	for (i = 0; i < ns_data->axle_number; i++) {
		ns_axle = &ns_data->ns_axles[i];
		if (ns_axle->mode == op_mode_pv)
			update_target_velocity(ns_axle, &axle_status[i]);
		else if (ns_axle->mode == op_mode_pp)
			update_target_position(ns_axle, &axle_status[i]);
	}	
	return ns_data->running;
}

int command_parse(char *command, struct axle_status_t *axle_status, int axle_num, int sock)
{
	char *p_colon = NULL;
	char *s_colon = NULL;
	int32_t value = 0;
	char buf[256];
	int axle = 0;
	int len;
	
	if (strcmp("exit", command) == 0) {
		send(sock, "exit..\n", 7, 0);
		return -1;	
	}

	if ((s_colon = strchr(command, ':')) == NULL){
		len = snprintf(buf, 256, "invalid cmd: %s\n", command);
		goto send;
	}
	if ((p_colon = strchr(s_colon + 1, ':')) != NULL){
		*p_colon = '\0';
	}
	*s_colon = '\0';
	
	axle = atoi(command);
	if (axle >= axle_num) {
		len = snprintf(buf, 256, "axle_index is bigger than the real number of axles(%d): %s\n", axle_num, command);
		goto send;	
	}

	command = s_colon + 1;
	if (strcmp("set_speed", command) == 0){
		if (axle_status[axle].mode == op_mode_pv) {
			if (!p_colon) {
				len = snprintf(buf, 256, "invalid cmd: %s\n", command);
				goto send;
			}
			sscanf(p_colon + 1, "%d", &value);
			axle_status[axle].target_speed = value;
		}
		len = snprintf(buf, 256, "set_speed of the axle %d : %d\n", axle, value);
	}
	else if (strcmp("set_profile_speed", command) == 0){
		if (axle_status[axle].mode == op_mode_pp) {
			if (!p_colon) {
				len = snprintf(buf, 256, "invalid cmd: %s\n", command);
				goto send;
			}
			sscanf(p_colon + 1, "%d", &value);
			axle_status[axle].profile_speed = value;
		}
		len = snprintf(buf, 256, "set_profile_speed of the axle %d : %d\n", axle, value);
	}
	else if (strcmp("set_position", command) == 0){
		if (axle_status[axle].mode == op_mode_pp) {
			if (!p_colon) {
				len = snprintf(buf, 256, "invalid cmd: %s\n", command);
				goto send;
			}
			sscanf(p_colon + 1, "%d", &value);
			axle_status[axle].target_position = value;
		}
		len = snprintf(buf, 256, "set_position of the axle %d : %d\n", axle, value);
	}

	else if (strcmp("get_speed", command) == 0) {
		len = snprintf(buf, 256, "get_speed of the axle %d : %d\n",axle, axle_status[axle].current_speed);
	}
	else if (strcmp("get_profile_speed", command) == 0) {
		len = snprintf(buf, 256,  "get_profile_speed of the axle %d : %d\n",axle, axle_status[axle].profile_speed);
	}
	else if (strcmp("get_target_speed", command) == 0) {
		len = snprintf(buf, 256, "get_target_speed of the axle %d : %d\n",axle, axle_status[axle].target_speed);
	}
	else if (strcmp("get_target_position", command) == 0) {
		len = snprintf(buf, 256, "get_target_position of the axle %d : %d\n",axle, axle_status[axle].target_position);
	}
	else if (strcmp("get_position", command) == 0) {
		len = snprintf(buf, 256, "get_current_position of the axle %d : %d\n",axle, axle_status[axle].current_position);
	}
	else if (strcmp("get_mode", command) == 0) {
		len = snprintf(buf, 256, "get_mode of the axle %d : %s\n",axle, get_mode_of_operation_str(axle_status[axle].mode));
	}
	else {
		len = snprintf(buf, 256, "invalid cmd: %s\n", command);
	}
send:
	send(sock, buf, len, 0);
	return 0;
}
const char *usage = "Usage: \n\
  nservo_run -f config.xml [-p port]\n\
    port : Tcp server port\n\
      defalut value : 6000\n";

int main(int argc, char **argv) {
	nser_global_data *ns_data;
	signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);
	char buffer[256];
	int sockfd, connfd = -1, len, on = 1, timeout, rc, ret, nfds, nfds_c, i;
	struct sockaddr_in servaddr;
	struct pollfd fds[6];
	int c;
        int service_port = PORT;
        char *conf_file_name = NULL;

//      daemon(0, 0);

        while ((c = getopt(argc, argv, "f:p:")) != -1) {
                switch (c) {
                        case 'p':
                                service_port = atoi(optarg);
                                break;
                        case 'f':
                                conf_file_name = strdup(optarg);
                                break;
                        default:
                                printf("%s", usage);
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
		
	if (!(axle_status = calloc(sizeof(struct axle_status_t),  ns_data->axle_number))) {
		fprintf(stderr, "Failed to malloc memory  for axle_status\n");
		goto destroy_master;
	}

	for (i = 0; i < ns_data->axle_number; i++)
	{
		axle_status[i].mode = ns_data->ns_axles[i].mode;
		axle_status[i].profile_speed = 800000;
	}

	if (user_cycle_task_start(ns_data, main_task, 0)) {
		fprintf(stderr, "Failed to start task\n");
		goto destroy_master;		
	}

	nfds = 1;
	len = 0;
	while (run) {
		rc = poll(fds, nfds, timeout);
		if (rc < 0) {
			perror("  poll() failed");
			run = 0;
			break;
		}
		if (rc > 0) {
			nfds_c = nfds;
			for (i = 0; i < nfds_c; i++) {
				if (fds[i].revents == 0)
					continue;
				if (fds[i].revents != POLLIN) {
					printf("  Error! revents = %d\n", fds[i].revents);
					run = 0;
					break;
				}
				if (fds[i].fd == sockfd) {
					connfd = accept(sockfd, NULL, NULL);
					if (connfd < 0) {
						if (errno != EWOULDBLOCK) {
							perror("  accept() failed");
							run = 0;
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
							if (buffer[len -2] == '\r')
								buffer[len - 2] = '\0';
							else
								buffer[len - 1] = '\0';
							if (command_parse(buffer, axle_status, ns_data->axle_number, fds[i].fd) < 0) {
								run = 0;
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
						connfd = -1;
						nfds = 1;
					}
				}
			}
		}
	}

	user_cycle_task_stop(ns_data);
destroy_master:
	nser_deactivate_all_masters(ns_data);
close_tcp:
	close(sockfd);
}
