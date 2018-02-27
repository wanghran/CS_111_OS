#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <sched.h>
#include <stdint.h>
#include "SortedList.h"

int thdnum = 1;
int itenum = 1;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int spin_lock = 0;
char *setting;
int opt_yield;
SortedList_t *list;
SortedListElement_t **element;

void element_init();

void* threads_opt(void *id);

int main(int argc, char *argv[])
{
	struct option long_options[] =
    {
        {"threads", optional_argument, NULL, 1},
        {"iterations", optional_argument, NULL, 2},
        {"sync", required_argument, NULL, 3},
        {"yield", required_argument, NULL, 4},
        {0, 0, 0, 0}
    };

	while(1) {
		int opt_flag = getopt_long(argc, argv, "", long_options, NULL);
		if (opt_flag == -1) {
			break;
		}
		switch (opt_flag){
        	case 1: 
        	{
        		thdnum = atoi(optarg);
        		break;
        	}
        	case 2:
        	{
        		itenum = atoi(optarg);
        		break;
        	}
        	case 3:
        	{
        		setting = optarg;
        		if (*setting != 'm' && *setting != 's') {
        			fprintf(stderr, "invalid sync argument\n");
        			exit(1);
        		}
        		break;
        	}
            case 4:
            {
            	int i = 0;
                while (*(optarg + i) != '\0') {
                	switch(*(optarg + i)) {
						case 'i':
							opt_yield |= INSERT_YIELD;
							break;
						case 'd':
							opt_yield |= DELETE_YIELD;
							break;
						case 'l':
							opt_yield |= LOOKUP_YIELD;
							break;
						default:
							fprintf(stderr, "invalid yield argument\n");
							exit(1);
							break;
					}
					++i;
                }
                break;
            }
        	default:
        	{
        		exit(1);
        	}
        }
	}

	list = (SortedListElement_t *) malloc(sizeof(SortedList_t*));
	list -> prev = list;
	list -> next = list;
	list -> key = NULL;
	element = (SortedListElement_t **) malloc(itenum * thdnum * sizeof(SortedListElement_t*));
	element_init();
	struct timespec start_time;
	if (clock_gettime(CLOCK_MONOTONIC, &start_time)) { 
		int ernum = errno;
        fprintf(stderr, "ending clock failed with error%s\n", strerror(ernum));
        exit(1);
	}
	pthread_t threads[thdnum];
	for (int i = 0; i < thdnum; ++i) {
		int id = i;
		int ret = pthread_create(&threads[i], NULL, threads_opt,(void *) (intptr_t)id);
		if (ret) {
    		int ernum = errno;
    		fprintf(stderr, "creating threads failed with error%s\n", strerror(ernum));
    		exit(1);
    	}
	}

	for (int j = 0; j < thdnum; ++j) {
		int ret = pthread_join(threads[j], NULL);
    	if (ret) {
    		int ernum = errno;
    		fprintf(stderr, "joining threads failed with error%s\n", strerror(ernum));
    		exit(1);
    	}
	}
	struct timespec end_time;
	if (clock_gettime(CLOCK_MONOTONIC, &end_time)) { 
		int ernum = errno;
        fprintf(stderr, "ending clock failed with error%s\n", strerror(ernum));
        exit(1);
	}
	if (SortedList_length(list) != 0) {
		fprintf(stderr, "list not empty after operation\n");
		exit(2);
	}

	printf("list-");
	if (opt_yield & INSERT_YIELD) {
		printf("i");
	}
	if (opt_yield & DELETE_YIELD) {
		printf("d");
	}
	if (opt_yield & LOOKUP_YIELD) {
		printf("l");
	}
	if (opt_yield == 0) {
		printf("none");
	}
	printf("-");
	if (setting == NULL) {
		printf("none");
	}else {
		printf("%s", setting);
	}
	long long total_time = (end_time.tv_sec - start_time.tv_sec) * 1000000000;
    long long optnum = thdnum * itenum * 3;
    total_time += end_time.tv_nsec;
    total_time -= start_time.tv_nsec;
    long long ave_time = total_time / optnum;
    printf(",%d,%d,1,%lld,%lld,%lld\n", thdnum, itenum, optnum, total_time, ave_time);
	return 0;
}

void* threads_opt(void *id) {
	int t_id = (intptr_t)id;
	if (setting != NULL && *setting == 'm') {
		if(pthread_mutex_lock(&mutex)) {
                int ernum = errno;
                fprintf(stderr, "failed grabbing the lock, with error%s\n", strerror(ernum));
                exit(1);
        }
	}
	if (setting != NULL && *setting == 's') {
		while(__sync_lock_test_and_set(&spin_lock, 1));
	}
	for (int i = t_id; i < itenum * thdnum; i += thdnum) {
		SortedList_insert(list, element[i]);
	}
	int list_length = SortedList_length(list);
	if (list_length == -1) {
		fprintf(stderr, "corrtuped list\n");
		exit(2);
	}
	for (int j = t_id; j < itenum * thdnum; j += thdnum) {
		SortedListElement_t *curr;
		curr = SortedList_lookup(list, element[j] -> key);
		if (curr == NULL) {
			continue;
		}
		if(SortedList_delete(curr) == 1) {
			fprintf(stderr, "corrtuped prev/next pointers\n");
			exit(2);
		}
	}
	if (setting != NULL && *setting == 'm') {
		if(pthread_mutex_unlock(&mutex)) {
                int ernum = errno;
                fprintf(stderr, "failed grabbing the lock, with error%s\n", strerror(ernum));
                exit(1);
        }
	}
	if (setting != NULL && *setting == 's') {
		__sync_lock_release(&spin_lock, 1);
	}

	pthread_exit(NULL);
}

void element_init() {
	char pool[63] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	for (int i = 0; i < itenum * thdnum; i++) {
		char key[11] = "";
		for (int j = 0; j < 10; j++) {
			key[j] = pool[rand() % 62];
		}
		element[i] = (SortedList_t*)malloc(sizeof(SortedList_t*));
		element[i] -> prev = NULL;
		element[i] -> next = NULL;
		element[i] -> key = (const char *)&key;
	}
}

