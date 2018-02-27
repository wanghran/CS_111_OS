#include <math.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <time.h>
#include <mraa.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdint.h>

sig_atomic_t volatile run_flag = 1;
const int r0 = 100000;
const int B = 4275;
int log_flag = 0;
char timebuf[10];
int c_flag = 0;
int f_flag = 1;
int period = 1;
int logfd;
char buffer[2048];
int off_sig = 0;
FILE* fp;
int p_flag = 0;
int s_flag = 0;

struct option long_options[] = 
{
	{"log", optional_argument, NULL, 1},
	{"period", required_argument, NULL, 2},
	{"scale", required_argument, NULL, 3},
	{0,0,0,0}
};

void but_handler();
float t_convert(float temp);
void command_handler(char *str);
void ppt(char *str);
void* threads_opt(void *id);

int main(int argc, char *argv[])
{

	while(1) {
		int opt_flag = getopt_long(argc, argv, "", long_options, NULL);
		if (opt_flag == -1) {
			break;
		}
		switch(opt_flag) {
			case 1:
			{
				log_flag = 1;
				if (optarg == NULL) {
					logfd = open("log.txt", O_RDWR | O_CREAT | O_TRUNC, 0666);
					if (logfd == -1) {
						int ernum = errno;
						fprintf(stderr, "cannot create a log file with error: %s", strerror(ernum));
						exit(1);
					}
					fp = fdopen(logfd, "w");
				}else {
					logfd = open(optarg, O_RDWR | O_CREAT | O_TRUNC, 0666);
					if (logfd == -1) {
						int ernum = errno;
						fprintf(stderr, "cannot create a log file with error: %s", strerror(ernum));
						exit(1);
					}
					fp = fdopen(logfd, "w");
				}
                break;
			}
			case 2:
			{
				period = atoi(optarg);
				p_flag = 1;
				break;
			}
			case 3:
			{
				s_flag = 1;
				if (*optarg == 'F') {
					f_flag = 1;
					c_flag = 0;
				}else if (*optarg == 'C') {
					f_flag = 0;
					c_flag = 1;
				}else {
					fprintf(stderr, "invalid temperature unit\n");
					exit(1);
				}
				break;
			}
			default:
			{
				exit(1);
			}
		}
	}

	pthread_t threads[2];
	if (log_flag == 1) {
		if (s_flag == 1) {
			if (f_flag == 1 && c_flag == 0) {
				fprintf(fp, "SCALE=F\n");
			}else if (c_flag == 1 && f_flag == 0) {
				fprintf(fp, "SCALE=C\n");
			}
		}
		if (p_flag == 1) {
			dprintf(logfd, "PERIOD=%d\n", period);
		}
	}
	int i = 0;
	for (; i < 2; ++i) {
		int id = i;
		int ret = pthread_create(&threads[i], NULL, threads_opt,(void *) (intptr_t)id);
		if (ret) {
   			int ernum = errno;
   			fprintf(stderr, "creating threads failed with error%s\n", strerror(ernum));
   			exit(1);
   		}
	}
	int j = 0;
	for (; j < 2; ++j) {
		int returnVal = 0;
		int ret = pthread_join(threads[j], (void**)&returnVal);
    	if (ret) {
    		fprintf(stderr, "joining threads %d failed with error%s\n", j,strerror(returnVal));
    		exit(1);
    	}
	}
	
	return 0 ;

}


void but_handler() {
	run_flag = 0;
	time_t ltime;
	struct tm *endtime;
	time(&ltime);
	endtime = localtime(&ltime);
	strftime(timebuf, 10, "%X", endtime);
	if (log_flag == 1) {
		fprintf(fp, "%s SHUTDOWN\n", timebuf);
	}else {
		fprintf(stdout, "%s SHUTDOWN\n", timebuf);
	}
	off_sig = 1;
	exit(0);
}

float t_convert(float temp){
	float r = 1023.0/temp - 1.0;
	r = r0 * r;
	float temperature = 1.0 / (log(r/r0)/B+1/298.15)-273.15;
	if (f_flag) {
		temperature = temperature * 1.8 + 32;
		return temperature;
	}else if (c_flag) {
		return temperature;
	}else {
		fprintf(stderr, "Inappropriate temperature setting\n");
		exit(1);
	}	
}

void command_handler(char *str){
	int i = 0;
	while (str[i] != '\0' && str[i] != '\004') {
		int start = i;
		while(str[i] != '\n') {
			i++;
		}
		char substring[i - start + 1];
		memcpy(substring, &str[start], i-start);
		substring[i-start] = '\0';
		if (i - start == 3) {
			if (strcmp(substring, "OFF") == 0) {
				ppt("OFF");
				but_handler();
			}else if (log_flag == 1){
				fprintf(fp, "%s\n", substring);
			}
		}else if (i - start == 4) {
			if (strcmp(substring, "STOP") == 0) {
				run_flag = 0;
				ppt("STOP");
			}else if (log_flag == 1) {
				fprintf(fp, "%s\n", substring);
			}
		}else if (i - start == 5) {
			if (strcmp(substring, "START") == 0) {
				run_flag = 1;
				ppt("START");
			}else if (log_flag == 1) {
				fprintf(fp, "%s\n", substring);
			}
		}else if (i - start == 7) {
			if (strcmp(substring, "SCALE=F") == 0) {
				f_flag = 1;
				c_flag = 0;
				ppt("SCALE=F");
			}else if (strcmp(substring, "SCALE=C") == 0) {
				c_flag = 1;
				f_flag = 0;
				ppt("SCALE=C");
			}else if (log_flag == 1) {
				fprintf(fp, "%s\n", substring);
			}
		}else if (strstr(substring, "PERIOD=") && strstr(substring, "PERIOD=") == substring) {
			period = atoi(substring + 7);
			ppt(substring);
		}else {
			if (log_flag == 1) {
				fprintf(fp, "%s\n", substring);
			}
		}
		i++;
	}
}

void ppt(char *str){
	if (log_flag == 1) {
		fprintf(fp, "%s\n", str);
	}
}

void* threads_opt(void *id){
	int t_id = (intptr_t)id;
	if (t_id == 0) {
		while (1) {
			if (off_sig == 1) {
				break;
			}
			int count = read(STDIN_FILENO, buffer, 2048);
			if (count == -1) {
				int ernum = errno;
				fprintf(stderr, "read from StdIn failed: %s\n", strerror(ernum));
				exit(1);
			}
			command_handler(buffer);
			memset(buffer, 0, sizeof(buffer));
		}
	}else {
		time_t rawtime;
		struct tm *info;
		float value = 0.0;
		mraa_aio_context temp;
		mraa_gpio_context button;
		temp = mraa_aio_init(1);
		button = mraa_gpio_init(60);
		mraa_gpio_dir(button, MRAA_GPIO_IN);
		mraa_gpio_isr(button, MRAA_GPIO_EDGE_RISING, &but_handler, NULL);

		while(1) {
			while (run_flag) {
				value = mraa_aio_read(temp);
				time(&rawtime);
				info = localtime(&rawtime);
				strftime(timebuf, 10, "%X", info);
				if (log_flag == 1) {
					fprintf(fp ,"%s %.1f\n", timebuf, t_convert(value));
				}else {
					fprintf(stdout ,"%s %.1f\n", timebuf, t_convert(value));
				}
				usleep(period * 1000000);
			}
		}
		
	}

	pthread_exit(NULL);
}