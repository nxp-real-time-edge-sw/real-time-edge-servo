// SPDX-License-Identifier: (GPL-2.0 OR MIT)
/*
 * Copyright 2018-2020 NXP
 */

#include <stdio.h>
#include <termios.h>            //termios, TCSANOW, ECHO, ICANON
#include <unistd.h>     //STDIN_FILENO
#include <ctype.h>
#include <string.h>
#include "nservo.h"
#include "pthread.h"
#include "tool.h"

#define ANSI_CURSOR_UP                  "\e[%dA"
#define ANSI_CURSOR_DOWN                "\e[%dB"
#define ANSI_CURSOR_FORWARD             "\e[%dC"
#define ANSI_CURSOR_BACK                "\e[%dD"
#define ANSI_CURSOR_NEXTLINE            "\e[%dE"
#define ANSI_CURSOR_PREVIOUSLINE        "\e[%dF"
#define ANSI_CURSOR_COLUMN              "\e[%dG"
#define ANSI_CURSOR_POSITION            "\e[%d;%dH"
#define ANSI_CURSOR_SHOW                "\e[?25h"
#define ANSI_CURSOR_HIDE                "\e[?25l"
#define ANSI_CLEAR_CONSOLE              "\e[2J"
#define ANSI_CLEAR_LINE_TO_END          "\e[0K"
#define ANSI_CLEAR_LINE                 "\e[2K"
#define ANSI_COLOR_RESET                "\e[0m"
#define ANSI_COLOR_REVERSE              "\e[7m"
#define ANSI_CURSOR_SAVE				"\e[s"
#define ANSI_CURSOR_RESTORE				"\e[u"

int termios_init(struct termios *oldt) {
	struct termios newt;
	tcgetattr( STDIN_FILENO, oldt);
	newt = *oldt;

	newt.c_lflag &= ~(ICANON);
	newt.c_lflag &= ~(ECHO);
	tcsetattr( STDIN_FILENO, TCSANOW, &newt);

	return 0;
}

int termios_end(struct termios *oldt) {
	/*restore the old settings*/
	tcsetattr( STDIN_FILENO, TCSANOW, oldt);
	return 0;
}

#define clear_scr()	fprintf(stdio, ANSI_CLEAR_CONSOLE)
#define clear_line()	fprintf(stdio, ANSI_CLEAR_LINE)
#define backspace() printf(ANSI_CURSOR_BACK, 1); printf(ANSI_CLEAR_LINE_TO_END)

void fresh_curr_line(char *prompt, char *buf) {
	printf(ANSI_CLEAR_LINE);
	printf(ANSI_CURSOR_COLUMN, 0);
	printf("%s", prompt);
	printf("%s", buf);
}

#define prompt "=>"

static char buff[16][64] = { 0 };
static int buff_index = 0;
static int hist_index = 0;
#define buff_size 16
#define buff_len 64
#define prompt_len 3
char* get_cmd() {
	char c;
	char tmp[64];
	int l = 0;
	printf(prompt);
	int dir = 0;
	int space = 0;
	int col = 0;
	int index = 0;
	buff[buff_index % buff_size][0] = '\0';
	while (1) {
		c = getc(stdin);
		if (c == 0x1b) {
			dir = 1;
			continue;
		} else if (c == 0x5b) {
			if (dir == 1)
				dir = 2;
			else
				dir = 0;
			continue;
		} else if (dir == 2) {
			if (c == 0x41) {
				if (hist_index == 0) {
					dir = 0;
					continue;
				}

				if (buff_index % buff_size == (hist_index - 1) % buff_size) {
					dir = 0;
					continue;
				}

				hist_index--;
				fresh_curr_line(prompt, buff[hist_index % buff_size]);
				l = strlen(buff[hist_index % buff_size]);
				col = l;
			} else if (c == 0x42) {
				if (hist_index >= buff_index) {
					dir = 0;
					continue;
				}
				if (buff_index % buff_size == hist_index % buff_size) {
					dir = 0;
					continue;
				}
				hist_index++;
				fresh_curr_line(prompt, buff[hist_index % buff_size]);
				l = strlen(buff[hist_index % buff_size]);
				col = l;
			} else if (c == 0x43) {
				if (col < l) {
					col++;
					printf(ANSI_CURSOR_FORWARD, 1);
				}
			} else if (c == 0x44) {
				if (col > 0) {
					col--;
					printf(ANSI_CURSOR_BACK, 1);
				}
			}
			dir = 0;
			continue;
		} else {
			dir = 0;
		}

		if (c == 0x7f) {
			if (hist_index != buff_index) {
				strncpy(buff[buff_index % buff_size],
						buff[hist_index % buff_size], buff_len);
				hist_index = buff_index;
			}
			if (col) {
				strcpy(buff[buff_index % buff_size] + col - 1,
						buff[buff_index % buff_size] + col);
				col--;
				l--;
			}
			fresh_curr_line(prompt, buff[buff_index % buff_size]);
			printf(ANSI_CURSOR_COLUMN, col + prompt_len);
		} else if (c == '\n') {
			if (hist_index != buff_index) {
				memcpy(buff[buff_index % buff_size],
						buff[hist_index % buff_size], buff_len - 1);
			}
			index = buff_index % buff_size;
			if ((l = strlen(buff[buff_index % buff_size])) > 0)
				buff_index++;
			printf(ANSI_CURSOR_COLUMN, l + prompt_len);
			hist_index = buff_index;
			break;
		} else if (isprint(c)) {
			putchar(c);
			if (hist_index != buff_index) {
				strncpy(buff[buff_index % buff_size],
						buff[hist_index % buff_size], buff_len);
				l = strlen(buff[buff_index % buff_size]);
				hist_index = buff_index;
			}
			if (c == ' ' && space == 0)
				continue;

			if (l < (buff_len - 2)) {
				space = 1;
				if (col != l) {
					strncpy(tmp, buff[buff_index % buff_size] + col, buff_len);
					buff[buff_index % buff_size][col++] = c;
					strcpy(buff[buff_index % buff_size] + col, tmp);
					l++;
				} else {
					buff[buff_index % buff_size][l++] = c;
					buff[buff_index % buff_size][l] = '\0';
					col++;
				}
				fresh_curr_line(prompt, buff[buff_index % buff_size]);
				printf(ANSI_CURSOR_COLUMN, col + prompt_len);
			} else {
				buff[buff_index % buff_size][l] = '\0';
				fprintf(stderr, "Cmd is too long\n");
				break;
			}
		}
	}
	return buff[index];
}

int parse_cmd(char *cmd, char **argv) {
	int i = 0, l, j = 0;
	l = strlen(cmd);
	if (l > buff_len || l <= 0)
		return 0;

	while (i < l) {
		while (isspace(*(cmd + i)) || *(cmd + i) == '\0') {
			if (*(cmd + i) == '\0')
				return j;
			i++;
		};
		argv[j++] = cmd + i;
		while (!isspace(*(cmd + i))) {
			if (*(cmd + i) == '\0')
				return j;
			i++;
		}
		*(cmd + i++) = '\0';
	}
	return j;
}

/***
 *
 *   manage cmd:
 *   	load_xml   			: int nser_app_load_xml(nser_global_data *ns_data, char *xmlfile)
 *		sdo_cmd				: all sdo cmd
 *		-- add_sdo
 *		-- add_sync
 *		-- add_slave
 *		-- add_axle
 *		save_xml			: No Support now
 *   	configure_finish 	: int nser_config_tool_finish(nser_global_data *ns_data, tool_data *t_data)
 *   	activate			: int nser_activate_all_masters(nser_global_data *ns_data)
 *   	start_task 			: int nser_tool_start_task(nser_global_data *ns_data, int isAutoStart)
 *   	pdo_cmd				: all pdo cmd
 *		stop_task			: void user_cycle_task_stop(nser_global_data *ns_data)
 *
 *
 * ***/

char *help =
		"\
 *\n\
 *   	load_xml  xml.file 	: load xml configuration file, allow to load only once .\n\
 *		sdo_cmd				: all sdo cmd\n\
 *		-- add_sdo\n\
 *		-- add_sync\n\
 *		-- add_slave\n\
 *		-- add_axle\n\
 *		save_xml xml.file	: No Support now\n\
 *   	activate			: int nser_activate_all_masters(nser_global_data *ns_data)\n\
 *   	start_task 			: int nser_tool_start_task(nser_global_data *ns_data)\n\
 *      pdo_cmd				: all pdo cmd\n\
 *      stop_task			: void user_cycle_task_stop(nser_global_data *ns_data)\n\
 *      quit				: exit\n";

int str_to_int(char *str) {
	int t = 0;
	if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
		sscanf((char *) &str[2], "%x", &t);
	} else {
		sscanf(str, "%d", &t);
	}
	return t;
}

uint32_t str_to_uint(char *str) {
	uint32_t t = 0;
	if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
		sscanf((char *) &str[2], "%x", &t);
	} else {
		sscanf(str, "%u", &t);
	}
	return t;
}

void start_interaction(tool_data *t_data, nser_global_data *ns_data) {
	struct termios oldt;
	char buf[64];
	termios_init(&oldt);
	char *argv[8];
	int argc;
	pdo_cmd_fun_t pdo_cmd_fun;
	sdo_cmd_fun_t sdo_cmd_fun;
	int axle_index;
	uint32_t value = 0;
	int size;
	while (1) {
		strncpy(buf, get_cmd(), 63);
		printf("\n");
		argc = parse_cmd(buf, argv);

		if (!argc)
			continue;

		if (!strncmp(argv[0], "sdo_set", 7)) {
			/*sdo cmd**/
			if (t_data->isActivate) {
				fprintf(stderr,
						"Master has been activated, SDO CMD must be executed before activating \n");
				continue;
			}
			if (argc < 3) {
				fprintf(stderr, "sdo_set_xx axle_index value \n");
				continue;
			}
			axle_index = str_to_int(argv[1]);
			value = str_to_uint(argv[2]);

			sdo_cmd_fun = find_sdo_cmd_fun(argv[0]);

			if (sdo_cmd_fun) {
				if (!(*sdo_cmd_fun)(&ns_data->ns_axles[axle_index], &value,
						&size)) {
					fprintf(stdout,
							"sdo_set is success(value = 0x%x size = %d) \n",
							(uint8_t) value, size);

				} else {

					fprintf(stdout, "sdo_set %s failed\n", argv[0]);
				}

			} else {
				fprintf(stdout, "No find %s \n", argv[0]);
			}

		} else if (!strncmp(argv[0], "sdo_get", 7)) {
			/*sdo cmd**/
			if (t_data->isActivate) {
				fprintf(stderr,
						"Master has been activated, SDO CMD must be executed before activating \n");
				continue;
			}
			if (argc < 2) {
				fprintf(stderr, "sdo_get_xx axle_index  \n");
				continue;
			}
			axle_index = str_to_int(argv[1]);

			sdo_cmd_fun = find_sdo_cmd_fun(argv[0]);

			if (sdo_cmd_fun) {
				if (!(*sdo_cmd_fun)(&ns_data->ns_axles[axle_index], &value,
						&size)) {
					if (size == 1)
						fprintf(stdout,
								"sdo_get is success(value = 0x%x size = %d) \n",
								(uint8_t) value, size);
					else if (size == 2)
						fprintf(stdout,
								"sdo_get is success(value = 0x%x size = %d) \n",
								(uint16_t) value, size);
					else if (size == 4)
						fprintf(stdout,
								"sdo_get is success(value = 0x%x size = %d) \n",
								(uint32_t) value, size);
				} else {

					fprintf(stdout, "sdo_get %s failed\n", argv[0]);
				}

			} else {
				fprintf(stdout, "No find %s \n", argv[0]);
			}

		} else if (!strncmp(argv[0], "set_", 4)) {
			/**pdo cmd*/
			if (!t_data->isStart) {
				fprintf(stderr,
						"Task is not starting, PDO CMD must be executed after task starting\n");
				continue;
			}
			if (argc < 3) {
				fprintf(stderr, "pdo_set_xx axle_index value \n");
				continue;
			}
			axle_index = str_to_int(argv[1]);
			value = str_to_uint(argv[2]);

			pdo_cmd_fun = find_pdo_cmd_fun(argv[0]);
			if (pdo_cmd_fun) {
				(*pdo_cmd_fun)(&t_data->t_axle[axle_index], t_data, &value,
						&size);
				fprintf(stdout, "pdo_set is success(value = 0x%x size = %d) \n",
						value, size);
			} else {
				fprintf(stdout, "No find %s \n", argv[0]);
			}

		} else if (!strncmp(argv[0], "get_", 4)) {
			/**pdo cmd*/
			if (!t_data->isStart) {
				fprintf(stderr,
						"Task is not starting, PDO CMD must be executed after task starting\n");
				continue;
			}
			if (argc < 2) {
				fprintf(stderr, "pdo_get_xx axle_index  \n");
				continue;
			}
			axle_index = str_to_int(argv[1]);

			pdo_cmd_fun = find_pdo_cmd_fun(argv[0]);

			if (pdo_cmd_fun) {
				(*pdo_cmd_fun)(&t_data->t_axle[axle_index], t_data, &value,
						&size);
				if (size == 1)
					fprintf(stdout,
							"pdo_get is success(value = 0x%x size = %d) \n",
							(uint8_t) value, size);
				else if (size == 2)
					fprintf(stdout,
							"pdo_get is success(value = 0x%x size = %d) \n",
							(uint16_t) value, size);
				else if (size == 4)
					fprintf(stdout,
							"pdo_get is success(value = 0x%x size = %d) \n",
							(uint32_t) value, size);
				else {
					fprintf(stdout, "size = %d\n", size);
				}
			} else {
				fprintf(stdout, "No find %s \n", argv[0]);
			}
		} else {
			if (!strncmp(argv[0], "load_xml", 8)) {
				if (argc != 2 || t_data->isLoadXML) {
					fprintf(stderr, "Configuration xml file has been loaded\n");
					continue;
				}
				if (!nser_app_load_xml(ns_data, argv[1])) {
					t_data->isLoadXML = 1;
				}
			} else if (!strncmp(argv[0], "activate", 8)) {
				if (t_data->isActivate) {
					fprintf(stderr, "All masters has been Activated\n");
					continue;
				}
				if (!nser_activate_all_masters(ns_data)) {
					t_data->isActivate = 1;
				}
				if (t_data->isFinished) {
					fprintf(stderr, "Configure has been finished\n");
					continue;
				}
				if (!nser_config_tool_finish(ns_data, t_data)) {
					t_data->isFinished = 1;
				}
			} else if (!strncmp(argv[0], "start_task", 9)) {
				if (t_data->isStart) {
					fprintf(stderr, "The task has been starting\n");
					continue;
				}
				int isAutoStart = 1;
				if (argc == 2) {
					if (argv[1][0] == 'n')
						isAutoStart = 0;
				}
				if (!nser_tool_start_task(ns_data, isAutoStart)) {
					t_data->isStart = 1;
				}
			} else if (!strncmp(argv[0], "stop_task", 9)) {
				if (!t_data->isStart) {
					fprintf(stderr, "The task is not starting\n");
					continue;
				}
				user_cycle_task_stop(ns_data);
				t_data->isStart = 0;

			} else if (!strcmp(buf, "quit")) {

				nser_deactivate_all_masters(ns_data);
				break;
			} else {
				printf("%s\n", help);
			}
		}

	}
	termios_end(&oldt);
}

int main(int argc, char **argv) {
	int max_domain_size = 0;
	tool_data *t_data;
	nser_global_data *ns_data;

	if (argc == 2) {
		max_domain_size = atoi(argv[2]);
	}

	if (!(ns_data = nser_app_config_init(NULL))) {
		fprintf(stderr, "Failed to initialize App's configuration \n");
		return 0;
	}

	if (!(t_data = nser_config_tool_init(max_domain_size))) {
		fprintf(stderr, "Failed to initialize tool's configuration \n");
		return 0;
	}

	start_interaction(t_data, ns_data);
}

