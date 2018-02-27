//  Name: Haoran Wang
//  ID: 505029637
//  Email: hwan252@ucla.edu

//  lab1b-server.c
//  PJ1B
//
//  Created by Rex Wang on 10/16/17.
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
#include <termios.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <mcrypt.h>

struct option long_option[] = {
    {"port", required_argument, NULL, 1},
    {"encrypt", required_argument, NULL, 2},
    {0, 0, 0, 0}
};

void SIGPIPE_Handler (int sig) {
    fprintf(stderr, "Shell has been terminated, cannot write to the shell via pipe error:  %s\n", strerror(sig));
}

void write_check(int wr_flag1, int wr_flag2) {
    if (wr_flag1 == -1) {
        int ernum = errno;
        fprintf(stderr, "write to shell failed: %s\n", strerror(ernum));
        exit(1);
    }
    if (wr_flag2 == -1) {
        int ernum = errno;
        fprintf(stderr, "write to socket failed: %s\n", strerror(ernum));
        exit(1);
    }
}

char* get_key(char *file) {
    struct stat key_stat;
    int keyfd = open(file, O_RDONLY);
    if (keyfd == -1) {
        int ernum = errno;
        fprintf(stderr, "open key file failed: %s\n", strerror(ernum));
        exit(1);
    }
    if(fstat(keyfd, &key_stat) == -1) {
        int ernum = errno;
        fprintf(stderr, "key status error: %s\n", strerror(ernum));
        exit(1);
    }
    char* key = (char*) malloc(key_stat.st_size * sizeof(char));
    if(read(keyfd, key, key_stat.st_size) == -1) {
        int ernum = errno;
        fprintf(stderr, "read key failed with error: %s\n", strerror(ernum));
        exit(1);
    }
    return key;
}


const char CtrlC[4] = {'^', 'C', '\r', '\n'};
const char CtrlD[4] = {'^', 'D', '\r', '\n'};
int port;
int enc_flag;
MCRYPT cryptfd;
MCRYPT decryptfd;

int main(int argc, char *argv[]) {

    while(1) {
        int opt_flag = getopt_long(argc, argv, "", long_option, NULL);
        if (opt_flag == -1) {
            break;
        }
        switch (opt_flag) {
            case 1:
            {
                port = atoi(optarg);
                break;
            }
            case 2:
            {
                enc_flag = 1;
                char *key = get_key(optarg);
                cryptfd = mcrypt_module_open("twofish", NULL, "cfb", NULL);
                if(cryptfd == MCRYPT_FAILED) {
                    fprintf(stderr, "Module Failed");
                    exit(1);
                }
                if(mcrypt_generic_init(cryptfd, key, strlen(key), NULL) < 0) {
                    fprintf(stderr, "Failed initializing encryption");
                    exit(1);
                }
                decryptfd = mcrypt_module_open("twofish", NULL, "cfb", NULL);
                if (decryptfd == MCRYPT_FAILED) {
                    fprintf(stderr, "Module failed");
                    exit(1);
                }
                if(mcrypt_generic_init(decryptfd, key, strlen(key), NULL) < 0) {
                    fprintf(stderr, "Failed initializing decryption");
                    exit(1);
                }
                break;
            }
            default:
                exit(1);
                break;
        }
    }

    int socfd, newsocfd, clilen;
    struct sockaddr_in serv_addr, cli_addr;
    socfd = socket(AF_INET, SOCK_STREAM, 0);
    if(socfd == -1) {
        int ernum = errno;
        fprintf(stderr, "socket() failed with error: %s\n", strerror(ernum));
        exit(1);
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    
    if (bind(socfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        int ernum = errno;
        fprintf(stderr, "bind failed with errror %s\n", strerror(ernum));
        exit(1);
    }
    if (listen(socfd, 1) == -1) {
        int ernum = errno;
        fprintf(stderr, "listen failed with errror %s\n", strerror(ernum));
        exit(1);
    }
    clilen = sizeof(cli_addr);
    newsocfd = accept(socfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
    if (newsocfd == -1) {
        int ernum = errno;
        fprintf(stderr, "accept failed with errror %s\n", strerror(ernum));
        exit(1);
    }

    signal(SIGPIPE, SIGPIPE_Handler);
    int to_child_pipe[2];
    int from_child_pipe[2];
    pid_t child_pid = -1;
    if (pipe(to_child_pipe) == -1) {
        int ernum = errno;
        fprintf(stderr, "to_child_pipe failed :%s\n", strerror(ernum));
        exit(1);
    }
    if (pipe(from_child_pipe) == -1) {
        int ernum = errno;
        fprintf(stderr, "from_child_pipe failed: %s\n", strerror(ernum));
        exit(1);
    }
    child_pid = fork();
    /*Parent Process*/
    if (child_pid > 0) {
        close(to_child_pipe[0]);
        close(from_child_pipe[1]);
        struct pollfd fds[3];
        fds[0].fd = newsocfd;
        fds[0].events = POLLIN;
        fds[1].fd = from_child_pipe[0];
        fds[1].events = POLLIN;
        while(1) {
            int ret = poll(fds, 2, -1);
            if (ret == -1) {
                int ernum = errno;
                fprintf(stderr, "poll() failed: %s\n", strerror(ernum));
                exit(1);
            }else if(ret == 0) {
                fprintf(stderr, "poll TIMEOUT\n");
                exit(1);
            }
            if(fds[0].revents & POLLIN) {
                int close_flag = 0;
                int kill_flag = 0;
                char buf[2048];
                int count = read(newsocfd, buf, 2048);
                if (count == -1) {
                    int ernum = errno;
                    fprintf(stderr, "read from Socket failed: %s\n", strerror(ernum));
                    exit(1);
                }
                if (enc_flag == 1) {
                    if (mdecrypt_generic(decryptfd, buf, count)) {
                        fprintf(stderr, "Decrypt message in failed");
                        exit(1);
                    }
                }
                int i = 0;
                while (i < count) {
                    if (buf[i] == '\004') {
                        close_flag = 1;
                        break;
                    }else if (buf[i] == '\n' || buf[i] == '\r') {
                        if (write(to_child_pipe[1], "\n", 1) == -1) {
                            int ernum = errno;
                            fprintf(stderr, "write to shell failed: %s\n", strerror(ernum));
                            exit(1);
                        }
                        i++;
                    }else if (buf[i] == '\003') {
                        kill_flag = 1;
                        break;
                    }else {
                        if (write(to_child_pipe[1], &buf[i], 1) == -1) {
                            int ernum = errno;
                            fprintf(stderr, "write to shell failed: %s\n", strerror(ernum));
                            exit(1);
                        }
                        i++;
                    }
                }
                if (kill_flag == 1) {
                    kill(child_pid,SIGINT);
                }
                if (close_flag == 1) {
                    close(to_child_pipe[1]);
                }
            }
            if (fds[0].revents & (POLLHUP + POLLERR)) {
                close(to_child_pipe[1]);
            }
            if (fds[1].revents & POLLIN) {
                char buff[2048];
                int count = read(from_child_pipe[0], buff, 2048);
                if (count == -1) {
                    int ernum = errno;
                    fprintf(stderr, "read from shell failed, error: %s\n", strerror(ernum));
                    exit(1);
                }
                if (enc_flag == 1) {
                    if (mcrypt_generic(cryptfd, buff, count) != 0) {
                        fprintf(stderr, "Encrypt message out failed");
                        exit(1);
                    }
                }
                int wr_flag = write(newsocfd, buff, count);
                if (wr_flag == -1) {
                    int ernum = errno;
                    fprintf(stderr, "write to socket failed, error: %s\n", strerror(ernum));
                    exit(1);
                }
            }
            if (fds[1].revents & (POLLHUP + POLLERR)) {
                int status;
                int close_flag2 = close(from_child_pipe[0]);
                if (close_flag2 == -1) {
                    int ernum = errno;
                    fprintf(stderr, "failed when trying to close from_pipe, %s\n\r", strerror(ernum));
                    exit(1);
                }
                pid_t check = waitpid(child_pid, &status, WNOHANG);
                if (check == -1) {
                    int ernum = errno;
                    fprintf(stderr, "waitpid() failed with error: %s\n\r", strerror(ernum));
                }
                fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n\r", WTERMSIG(status), WEXITSTATUS(status));
                close(socfd);
                close(newsocfd);
                mcrypt_generic_deinit(cryptfd);
                mcrypt_module_close(cryptfd);
                mcrypt_generic_deinit(decryptfd);
                mcrypt_module_close(decryptfd);
                exit(0);
            }
        }
        /*Child Process*/
    }else if (child_pid == 0){
        close(to_child_pipe[1]);
        close(from_child_pipe[0]);
        dup2(to_child_pipe[0], STDIN_FILENO);
        dup2(from_child_pipe[1], STDOUT_FILENO);
        dup2(from_child_pipe[1], STDERR_FILENO);
        char *execvp_argv[2];
        char execvp_filename[] = "/bin/bash";
        execvp_argv[0] = execvp_filename;
        execvp_argv[1] = NULL;
        if (execvp(execvp_filename, execvp_argv) == -1) {
            int ernum = errno;
            fprintf(stderr, "execvp() failed, error: %s\n", strerror(ernum));
            exit(1);
        }
    }else {
        int ernum = errno;
        fprintf(stderr, "fork() failed: %s\n", strerror(ernum));
        exit(1);
    }






    return 0;
}
