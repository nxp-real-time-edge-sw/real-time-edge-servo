// SPDX-License-Identifier: (GPL-2.0 OR MIT)
/*
 * Copyright 2018-2020 NXP
 */

#include <stdio.h>
#include <unistd.h>     //STDIN_FILENO
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <netdb.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#define PORT 6000
#define SA struct sockaddr 

const char *usage = "Usage:\n nservo_client [-s ip] [-p port] [-a axis_index] -c cmd\n \
    port : Tcp server port\n \
      defalut value : 6000\n\
    ip : Tcp server ip\n\
      defalut value : 127.0.0.1\n\
    axis_index : Specify the axis to set\n\
      defalut value : 0\n\
    cmd : \n\
      For CSP mode: \n\
          set_tparrays:\"Cyclic=1; Scale=1000; Bias=0; Accel=8; Decel=8; Max_speed=3600; TpArrays=[(0:1000),(45:1000),(45:1000),(90:1000)];\"\n\
          set_start -- Start to run after TP arrays loaded\n\
          set_stop -- Stop running\n\
          get_current_position\n\
          get_current_velocity\n\
          get_mode\n\
          load_tp_file:\"<Tp arrays file >\" -- Load TP arrays from a file\n\
          set_start_all -- Start to run all axis in CSP mode after TP arrays loaded\n\
          set_stop_all -- Stop all axis in CSP mode running\n\
      For PP mode: \n\
          set_target_position:200\n\
          set_profile_velocity:200\n\
          get_current_position\n\
          get_current_velocity\n\
          get_target_position\n\
          get_profile_velocity\n \
          get_mode\n\
      For PV mode: \n\
          set_target_velocity:2000\n\
          get_current_position\n\
          get_current_velocity\n\
          get_target_velocity\n\
          get_mode\n\
        exit\n";

int main(int argc, char **argv) {
	char buffer[256];
	int sockfd, len;
	struct sockaddr_in servaddr;
	char *service_ip = strdup("127.0.0.1");
	int service_port = PORT;
	int axis = 0;
	char *cmd = NULL;
	int c;
	while ((c = getopt(argc, argv, "a:c:p:s:")) != -1) {
		switch (c) {
			case 'p':
				service_port = atoi(optarg);
				break;
			case 's':
				free(service_ip);
				service_ip = strdup(optarg);
				break;
			case 'a':
				axis = atoi(optarg);
				break;
			case 'c':
				if (cmd != NULL){
					free(cmd);
				}
				cmd = strdup(optarg);
				break;
			default:
				free(service_ip);
				if (cmd != NULL){
					free(cmd);
				}
				printf("%s", usage);
			return -1;
		}
	}

	if (!cmd) {
		printf("%s", usage);
		return -1;
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	} else
	
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(service_ip);
	servaddr.sin_port = htons(service_port);
	
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
			printf("connection with the server failed...\n");
			exit(0);
	}

	if (strcmp(cmd, "exit") == 0) {
		len = snprintf(buffer, 256, "%s\n", cmd);
	}
	else if (strcmp(cmd, "set_start_all") == 0) {
		len = snprintf(buffer, 256, "%s\n", cmd);
	}
	else if (strcmp(cmd, "set_stop_all") == 0) {
		len = snprintf(buffer, 256, "%s\n", cmd);
	}
	else if (strncmp(cmd, "load_tp_file", 12) == 0) {
		len = snprintf(buffer, 256, "%s\n", cmd);
	} else {
		len = snprintf(buffer, 256, "%d:%s\n", axis, cmd);
	}
	send(sockfd, buffer, len, 0);
	len = recv(sockfd, buffer, 256, 0);
	if (len > 0) {
		buffer[len] = '\0';
		printf("%s\n",buffer);
	}
	close(sockfd);
	free(cmd);
	free(service_ip);
}
