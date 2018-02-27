#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <sched.h>

long long counter = 0;
int itenum = 1;
int thdnum = 1;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int spin_lock = 0;
char *setting;
int opt_yield;

void add(long long *pointer, long long value) {
    long long sum = *pointer + value;
    if (opt_yield)
        sched_yield();
    *pointer = sum;
}

void compadd(long long *pointer, long long value) {
	long long temp;
    do{
        temp = *pointer;
        if (opt_yield)
            sched_yield();
	}while(__sync_val_compare_and_swap(&counter, temp, temp + value) != temp);
}

void* thread_add(char *setting) {
	for (int i = 0; i < itenum; ++i) {
		if (setting == NULL) {
			add(&counter, 1);
		}else if (*setting == 'm') {
			if(pthread_mutex_lock(&mutex)) {
                int ernum = errno;
                fprintf(stderr, "failed grabbing the lock, with error%s\n", strerror(ernum));
                exit(1);
            }
			add(&counter, 1); 
			if (pthread_mutex_unlock(&mutex)) {
                int ernum = errno;
                fprintf(stderr, "failed releasing the lock, with error%s\n", strerror(ernum));
                exit(1);
            }
		}else if (*setting == 's') {
			while(__sync_lock_test_and_set(&spin_lock, 1));
			add(&counter, 1);
			__sync_lock_release(&spin_lock);
		}else if (*setting == 'c') {
			compadd(&counter, 1);
		}else {
			fprintf(stderr, "invalid sync argument\n");
			exit(1);
		}

	}

	for (int j = 0; j < itenum; ++j) {
		if (setting == NULL) {
			add(&counter, -1);
		}else if (*setting == 'm') {
			pthread_mutex_lock(&mutex);
			add(&counter, -1);
			pthread_mutex_unlock(&mutex);
		}else if (*setting == 's') {
			while(__sync_lock_test_and_set(&spin_lock, 1));
			add(&counter, -1);
			__sync_lock_release(&spin_lock);
		}else if (*setting == 'c') {
			compadd(&counter, -1);
		}else {
			fprintf(stderr, "invalid sync argument\n");
			exit(1);
		}
	}
	pthread_exit(NULL);
	return NULL;

}

struct option long_options[] =
    {
        {"threads", optional_argument, NULL, 1},
        {"iterations", optional_argument, NULL, 2},
        {"sync", required_argument, NULL, 3},
        {"yield", no_argument, NULL, 4},
        {0, 0, 0, 0}
    };


int main(int argc, char *argv[]) {
	 
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
                if (*setting != 'm' && *setting != 's' && *setting != 'c') {
                    fprintf(stderr, "invalid sync argument\n");
                    exit(1);
                }
        		break;
        	}
            case 4:
            {
                opt_yield = 1;
                break;
            }
        	default:
        	{
        		exit(1);
        	}
        }

    }
    pthread_t threads[thdnum];
    
    struct timespec start_time;
    if(clock_gettime(CLOCK_MONOTONIC, &start_time) == -1) {
        int ernum = errno;
        fprintf(stderr, "ending clock failed with error%s\n", strerror(ernum));
        exit(1);
    }

    for (int i = 0; i < thdnum; ++i) {
    	int ret = pthread_create(&threads[i], NULL, (void *)thread_add, setting);
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
    if(clock_gettime(CLOCK_MONOTONIC, &end_time) == -1) {
        int ernum = errno;
        fprintf(stderr, "ending clock failed with error%s\n", strerror(ernum));
        exit(1);
    }
    long long total_time = (end_time.tv_sec - start_time.tv_sec) * 1000000000;
    long long optnum = thdnum * itenum * 2;
    total_time += end_time.tv_nsec;
    total_time -= start_time.tv_nsec;
    long long ave_time = total_time / optnum;
    if (opt_yield == 1) {
        printf("add-yield-");
    }else {
        printf("add-");
    }
    if (setting == NULL) {
        printf("none");
    }else if (*setting == 'm') {
        printf("m");
    }else if (*setting == 's') {
        printf("s");
    }else if (*setting == 'c') {
        printf("c");
    }
    printf(",%d,%d,%lld,%lld,%lld,%lld\n", thdnum, itenum, optnum, total_time, ave_time, counter);
	return 0;
}