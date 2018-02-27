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
int subnum = 1;
pthread_mutex_t *mutex;
int *spin_lock;
char *setting;
int opt_yield;
SortedListElement_t **element;
SortedList_t **sublist;
long long *mutex_time;
#define FNV_PRIME_32 16777619
#define FNV_OFFSET_32 2166136261U


void element_init();

void* threads_opt(void *id);

unsigned long hash(unsigned char *str);

int main(int argc, char *argv[])
{
	struct option long_options[] =
    {
        {"threads", optional_argument, NULL, 1},
        {"iterations", optional_argument, NULL, 2},
        {"sync", required_argument, NULL, 3},
        {"yield", required_argument, NULL, 4},
        {"lists", required_argument, NULL, 5},
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
            case 5:
            {
            	subnum = atoi(optarg);
            	break;
            }
        	default:
        	{
        		exit(1);
        	}
        }
	}

	spin_lock = (int *) malloc (subnum * sizeof(int));
	for (int z = 0; z < subnum; z++) {
		spin_lock[z] = 0;
	}
	mutex = (pthread_mutex_t *) malloc (subnum * sizeof(pthread_mutex_t));
	for (int a = 0; a < subnum; a++) {
		mutex[a] = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
	}
	element = (SortedListElement_t **) malloc(itenum * thdnum * sizeof(SortedListElement_t*));
	mutex_time = (long long *) malloc(thdnum * sizeof(long long));
	element_init();
	sublist = (SortedList_t **) malloc(subnum * sizeof(SortedList_t*));
	for (int x = 0; x < subnum; x++) {
		sublist[x] = (SortedList_t *) malloc(sizeof(SortedList_t));
		sublist[x] -> prev = sublist[x];
		sublist[x] -> next = sublist[x];
		sublist[x] -> key = NULL;
	}
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
		int returnVal = 0;
		int ret = pthread_join(threads[j], (void**)&returnVal);
    	if (ret) {
    		//int ernum = errno;
    		fprintf(stderr, "joining threads %d failed with error%s\n", j,strerror(returnVal));
    		exit(1);
    	}
	}
	struct timespec end_time;
	if (clock_gettime(CLOCK_MONOTONIC, &end_time)) { 
		int ernum = errno;
        fprintf(stderr, "ending clock failed with error%s\n", strerror(ernum));
        exit(1);
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
    printf(",%d,%d,%d,%lld,%lld,%lld,", thdnum, itenum, subnum, optnum, total_time, ave_time);
	if (setting != NULL && *setting == 'm') {
		long long totalMutx = 0;
		for (int i = 0; i < thdnum; i++) {
			totalMutx += mutex_time[i];
		}
		long long avgMutx = totalMutx / ((subnum + itenum * thdnum * 3) * thdnum);
		printf("%lld\n", avgMutx);
	}else {
		printf("0\n");
	}
	free(mutex);
	free(spin_lock);
	free(element);
	free(sublist);
	return 0;
}

void* threads_opt(void *id) {
	int t_id = (intptr_t)id;
	/*add element*/
	int i;
	for (i = t_id; i < thdnum * itenum; i+=thdnum) {
		unsigned int list_id = hash((unsigned char*) element[i] -> key) % subnum;
		
		if (setting != NULL && *setting == 'm') {
			struct timespec m_start_time;
			struct timespec m_end_time;
			if (clock_gettime(CLOCK_MONOTONIC, &m_start_time)) { 
				int ernum = errno;
        		fprintf(stderr, "starting clock failed for mutex with error%s\n", strerror(ernum));
       			exit(1);
			}
			if(pthread_mutex_lock(&mutex[list_id])) {
         		int ernum = errno;
            	fprintf(stderr, "failed grabbing the lock, with error%s\n", strerror(ernum));
            	exit(1);
        	}
        	if (clock_gettime(CLOCK_MONOTONIC, &m_end_time)) { 
				int ernum = errno;
        		fprintf(stderr, "ending clock failed for mutex with error%s\n", strerror(ernum));
        		exit(1);
			}
			long long ind_mutex_time = (m_end_time.tv_sec - m_start_time.tv_sec) * 1000000000 + m_end_time.tv_nsec - m_start_time.tv_nsec;
			mutex_time[t_id] += ind_mutex_time;
		}
		if (setting != NULL && *setting == 's') { 
			while(__sync_lock_test_and_set(&spin_lock[list_id], 1));
		}
		
		SortedList_insert(sublist[list_id], element[i]);
		
		if (setting != NULL && *setting == 'm') {
			if(pthread_mutex_unlock(&mutex[list_id])) {
            	int ernum = errno;
            	fprintf(stderr, "failed grabbing the lock, with error%s\n", strerror(ernum));
            	exit(1);
       		}
		}
		if (setting != NULL && *setting == 's') { 
			__sync_lock_release(&spin_lock[list_id], 1);
		}	
	}
	/*get all locks*/
	for (int jl = 0; jl < subnum; jl++) {
		if (setting != NULL && *setting == 'm') {
			struct timespec m_start_time;
			struct timespec m_end_time;
			if (clock_gettime(CLOCK_MONOTONIC, &m_start_time)) { 
				int ernum = errno;
        		fprintf(stderr, "starting clock failed for mutex with error%s\n", strerror(ernum));
        		exit(1);
			}
			if(pthread_mutex_lock(&mutex[jl])) {
            	int ernum = errno;
           		fprintf(stderr, "failed grabbing the lock, with error%s\n", strerror(ernum));
           		exit(1);
        	}
        	if (clock_gettime(CLOCK_MONOTONIC, &m_end_time)) { 
				int ernum = errno;
        		fprintf(stderr, "ending clock failed for mutex with error%s\n", strerror(ernum));
        		exit(1);
			}
			long long ind_mutex_time = (m_end_time.tv_sec - m_start_time.tv_sec) * 1000000000 + m_end_time.tv_nsec - m_start_time.tv_nsec;
			mutex_time[t_id] += ind_mutex_time;
		}
		if (setting != NULL && *setting == 's') { 
			while(__sync_lock_test_and_set(&spin_lock[jl], 1));
		}
	}
	int listLength = 0;
	for (int j = 0; j < subnum; j++) {
		if (SortedList_length(sublist[j]) == -1) {
			fprintf(stderr, "corrupted list \n");
			exit(2);
		}
		listLength += SortedList_length(sublist[j]);
	}
	for (int jr=0; jr < subnum; jr++) {
		if (setting != NULL && *setting == 'm') {
			struct timespec m_start_time;
			struct timespec m_end_time;
			if (clock_gettime(CLOCK_MONOTONIC, &m_start_time)) { 
				int ernum = errno;
        		fprintf(stderr, "starting clock failed for mutex with error%s\n", strerror(ernum));
        		exit(1);
			}
			if(pthread_mutex_unlock(&mutex[jr])) {
            	int ernum = errno;
           		fprintf(stderr, "failed grabbing the lock, with error%s\n", strerror(ernum));
           		exit(1);
        	}
        	if (clock_gettime(CLOCK_MONOTONIC, &m_end_time)) { 
				int ernum = errno;
        		fprintf(stderr, "ending clock failed for mutex with error%s\n", strerror(ernum));
        		exit(1);
			}
			long long ind_mutex_time = (m_end_time.tv_sec - m_start_time.tv_sec) * 1000000000 + m_end_time.tv_nsec - m_start_time.tv_nsec;
			mutex_time[t_id] += ind_mutex_time;
		}
		if (setting != NULL && *setting == 's') { 
			__sync_lock_release(&spin_lock[jr], 1);
		}
	}
	/*lookup and delete*/
	SortedListElement_t *curr;
	for (int z = t_id; z < thdnum * itenum; z += thdnum) {
		unsigned int list_id = hash((unsigned char*) element[z] -> key) % subnum;
		if (setting != NULL && *setting == 'm') {
			struct timespec m_start_time;
			struct timespec m_end_time;
			if (clock_gettime(CLOCK_MONOTONIC, &m_start_time)) { 
				int ernum = errno;
        		fprintf(stderr, "starting clock failed for mutex with error%s\n", strerror(ernum));
       			exit(1);
			}
			if(pthread_mutex_lock(&mutex[list_id])) {
         		int ernum = errno;
            	fprintf(stderr, "failed grabbing the lock, with error%s\n", strerror(ernum));
            	exit(1);
        	}
        	if (clock_gettime(CLOCK_MONOTONIC, &m_end_time)) { 
				int ernum = errno;
        		fprintf(stderr, "ending clock failed for mutex with error%s\n", strerror(ernum));
        		exit(1);
			}
			long long ind_mutex_time = (m_end_time.tv_sec - m_start_time.tv_sec) * 1000000000 + m_end_time.tv_nsec - m_start_time.tv_nsec;
			mutex_time[t_id] += ind_mutex_time;
		}
		if (setting != NULL && *setting == 's') { 
			while(__sync_lock_test_and_set(&spin_lock[list_id], 1));
		}

		curr = SortedList_lookup(sublist[list_id], element[z] -> key);
		if (curr == NULL) {
			fprintf(stderr, "corrupted list\n");
			exit(2);
		}else
		if (SortedList_delete(curr) == 1) {
			fprintf(stderr, "corrupted list\n");
			exit(2);
		}
		
		if (setting != NULL && *setting == 'm') {
			if(pthread_mutex_unlock(&mutex[list_id])) {
            	int ernum = errno;
            	fprintf(stderr, "failed grabbing the lock, with error%s\n", strerror(ernum));
            	exit(1);
       		}
		}
		if (setting != NULL && *setting == 's') { 
			__sync_lock_release(&spin_lock[list_id], 1);
		}	

	}

	pthread_exit(NULL);
}

void element_init() {
	char pool[63] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	for (int i = 0; i < itenum * thdnum; i++) {
		char *key = malloc(11 * sizeof(char));
		for (int j = 0; j < 10; j++) {
			key[j] = pool[rand() % 62];
		}
		key[10] = '\0';
		element[i] = (SortedListElement_t*)malloc(sizeof(SortedListElement_t));
		element[i] -> prev = NULL;
		element[i] -> next = NULL;
		element[i] -> key = (const char *)key;
	}
}

/*FNV-1 hash function*/
unsigned long hash(unsigned char *s){
	uint32_t hash = FNV_OFFSET_32, i;
    for(i = 0; i < strlen((const char *)s); i++)
    {
        hash = hash ^ (s[i]); // xor next byte into the bottom of the hash
        hash = hash * FNV_PRIME_32; // Multiply by prime number found to work well
    }
    return hash;
}

