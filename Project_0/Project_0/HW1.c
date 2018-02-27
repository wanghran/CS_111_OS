//  Name: Haoran Wang
//  Email: hwan252@ucla.edu
//  ID: 505029637
//
//  Created by Rex Wang on 10/2/17.
//  Copyright © 2017 Rex Wang. All rights reserved.
//

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

//signal handler, catch the signal and print the error message with error code SIGSEGV
void sig_handler (int signum) {
    fprintf(stderr, "Catch command was called, Signal received, error : %s \n", strerror(signum));
    exit(4);
}

int main(int argc, char *argv[])
{

    //long options
    struct option long_options[] =
    {
        {"input", required_argument, NULL, 1},
        {"output", required_argument, NULL, 2},
        {"segfault", no_argument, NULL, 3},
        {"catch", no_argument, NULL, 4},
        {0, 0, 0, 0}
    };
    
    //reference prepared to store the arguments if either input or output is called
    char *in = NULL;
    char *out = NULL;
    
    
    //flag to determine if all commands are read
    int flag = 0;
    int fault_flag = 0;
    int catch_flag = 0;
    while (1) {
        flag = getopt_long(argc, argv, "", long_options, NULL);
        //if all commands are read, quit the loop
        if (flag ==  -1) {
            break;
        }
        
        switch (flag) {
            case 1: {
                in = optarg;
                break;
            }
                
            case 2: {
                out = optarg;
                break;
            }
            case 3: {
                fault_flag = 1;
                break;
            }
            case 4: {
                catch_flag = 1;
                break;
            }
            default: {
                exit(1);
            }
        }
        
    }
    
    //if catch was called, register singal handler first
    if (catch_flag == 1) {
        signal(SIGSEGV, sig_handler);
    }
    
    //if seg_fault was called, make a segmentation fault
    if (fault_flag == 1) {
        char *f = NULL;
        *f = 'f';
    }
    
    //if input was called, redirect the input file
    if (in != NULL) {
        int fd0 = open(in, O_RDONLY, 06440);
        if (fd0 == -1) {
            int errnum = errno;
            fprintf(stderr, "input command failed, %s could not be opened, %s \n", in, strerror(errnum));
            exit(2);
        }
        close(0);
        dup(fd0);
        close(fd0);
    }
    
    //if output was called, redirect the output file
    if(out != NULL) {
        int fd1 = open(out, O_CREAT | O_WRONLY,06440);
        if (fd1 == -1) {
            int errnum = errno;
            fprintf(stderr, "output command failed, %s could not be opened, %s \n", out, strerror(errnum));
            exit(3);
        }
        close(1);
        dup(fd1);
        close(fd1);
    }
    
    //copy input to output
    char *buf[1];
    
    int read_flag = 1;
    int write_flag = 1;
    while (1) {
        read_flag = read(0, buf, 1);
        if (read_flag == 0) {
            break;
        }
        if (read_flag == -1) {
            int errnum = errno;
            fprintf(stderr, "read failed, %s could not be read, %s \n", in, strerror(errnum));
            exit(2);
        }
        write_flag = write(1, buf, 1);
        if (write_flag == -1) {
            int errnum = errno;
            fprintf(stderr, "write failed, %s could not be written, %s \n", out, strerror(errnum));
            exit(3);
        }
    }
    exit(0);
    
    return 0;
}
