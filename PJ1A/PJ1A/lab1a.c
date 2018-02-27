//  Name: Haoran Wang
//  ID: 505029637
//  Email: hwan252@ucla.edu
//  Created by Rex Wang on 10/6/17.
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
    
struct termios saved_attributes;
/*following reset and set code are from gnu sample code https://www.gnu.org/software/libc/manual/html_node/Noncanon-Example.html#Noncanon-Example    */

void reset_input_mode (void) {
    tcsetattr (STDIN_FILENO, TCSANOW, &saved_attributes);
}
    
void set_input_mode (void) {
    struct termios tattr;
        
    /* Make sure stdin is a terminal. */
    if (!isatty (STDIN_FILENO))
    {
        fprintf (stderr, "Not a terminal.\n");
        exit (1);
    }
        
    /* Save the terminal attributes so we can restore them later. */
    tcgetattr (STDIN_FILENO, &saved_attributes);
    atexit (reset_input_mode);
        
    /* Set the funny terminal modes. */
    tcgetattr (STDIN_FILENO, &tattr);
    tattr.c_cc[VMIN] = 1;
    tattr.c_cc[VTIME] = 0;
    tattr.c_iflag = ISTRIP;	/* only lower 7 bits	*/
    tattr.c_oflag = 0;		/* no processing	*/
    tattr.c_lflag = 0;		/* no processing	*/
    tcsetattr (STDIN_FILENO, TCSAFLUSH, &tattr);
}

void write_check(int wr_flag1, int wr_flag2) {
    if (wr_flag1 == -1) {
        int ernum = errno;
        fprintf(stderr, "write to child failed: %s\n", strerror(ernum));
        exit(1);
    }
    if (wr_flag2 == -1) {
        int ernum = errno;
        fprintf(stderr, "echo on screen failed: %s\n", strerror(ernum));
        exit(1);
    }
}

void SIGPIPE_Handler (int sig) {
    fprintf(stderr, "Shell has been terminated, cannot write to the shell via pipe error:  %s\n", strerror(sig));
}



int main(int argc, char *argv[]) {
    
    struct option long_options[] =
    {
        {"shell", no_argument, NULL, 1},
        {0, 0, 0, 0}
    };
    
    const char map[2] = {'\r', '\n'};
    const char CtrlC[4] = {'^', 'C', '\r', '\n'};
    const char CtrlD[4] = {'^', 'D', '\r', '\n'};
    while(1) {
        int opt_flag = getopt_long(argc, argv, "", long_options,NULL);
        if (opt_flag == -1) {
            break;
        }
        switch (opt_flag) {
            case 1:
                set_input_mode();
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
                    struct pollfd fds[2];
                    fds[0].fd = STDIN_FILENO;
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
                            int kill_flag = 0;
                            int close_flag = 0;
                            char buf[2048];
                            int count = read(STDIN_FILENO, buf, 2048);
                            if (count == -1) {
                                int ernum = errno;
                                fprintf(stderr, "read from StdIn failed: %s\n", strerror(ernum));
                                exit(1);
                            }
                            int i = 0;
                            while (i < count) {
                                if (buf[i] == '\004') {
                                    int wr_flag =write(STDOUT_FILENO, CtrlD, 4);
                                    if (wr_flag == -1) {
                                        int ernum = errno;
                                        fprintf(stderr, "echo ^D on the screen failed: %s\n", strerror(ernum));
                                        exit(1);
                                    }
                                    close_flag = 1;
                                    break;
                                }else if (buf[i] == '\n' || buf[i] == '\r') {
                                    int wr_flag1 = write(to_child_pipe[1], "\n", 1);
                                    int wr_flag2 = write(STDOUT_FILENO, map, 2);
                                    write_check(wr_flag1,wr_flag2);
                                    i++;
                                }else if (buf[i] == '\003') {
                                    int wr_flag = write(STDOUT_FILENO, CtrlC, 4);
                                    if (wr_flag == -1) {
                                        int ernum = errno;
                                        fprintf(stderr, "echo ^C on the screen failed: %s\n", strerror(ernum));
                                        exit(1);
                                    }
                                    kill_flag = 1;
                                    break;
                                }else {
                                    int wr_flag1 = write(to_child_pipe[1], &buf[i], 1);
                                    int wr_flag2 = write(STDOUT_FILENO, &buf[i], 1);
                                    write_check(wr_flag1,wr_flag2);
                                    i++;
                                }
                            }
                            if (kill_flag == 1) {
                                kill(child_pid, SIGINT);
                            }
                            if (close_flag == 1) {
                                close(to_child_pipe[1]);
                            }
                        }
                        if (fds[0].revents & (POLLHUP + POLLERR)) {
                            int ernum = errno;
                            fprintf(stderr, "SdtIn error: %s\n", strerror(ernum));
                            close(to_child_pipe[1]);
                            close(from_child_pipe[0]);
                            kill(child_pid, SIGINT);
                            exit(1);
                        }
                        if (fds[1].revents & POLLIN) {
                            char buff[2048];
                            int count = read(from_child_pipe[0], buff, 2048);
                            if (count == -1) {
                                int ernum = errno;
                                fprintf(stderr, "read from shell failed, error: %s\n", strerror(ernum));
                                exit(1);
                            }
                            int i = 0;
                            while(i < count) {
                                if(buff[i] == '\n' || buff[i] == '\r') {
                                    int wr_flag = write(STDOUT_FILENO, map, 2);
                                    if (wr_flag == -1) {
                                        int ernum = errno;
                                        fprintf(stderr, "echo return from shell failed, error: %s\n", strerror(ernum));
                                        exit(1);
                                    }
                                }else {
                                    int wr_flag = write(STDOUT_FILENO, &buff[i], 1);
                                    if (wr_flag == -1) {
                                        int ernum = errno;
                                        fprintf(stderr, "echo return from shell failed, error: %s\n", strerror(ernum));
                                        exit(1);
                                    }
                                }
                                i++;
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
                        reset_input_mode();
                        exit(1);
                    }
                }else {
                    int ernum = errno;
                    fprintf(stderr, "fork() failed: %s\n", strerror(ernum));
                    exit(1);
                }
            break;
                
            default:
                exit(1);
            break;
        }
    }
    set_input_mode();
    while (1) {
        char buf[2048];
        int count = read(STDIN_FILENO, buf, 2048);
        if (count == -1) {
            int ernum = errno;
            fprintf(stderr, "read failed: %s\n", strerror(ernum));
            exit(1);
        }
        int i = 0;
        while (i < count) {
            if (buf[i] == 4) {
                reset_input_mode();
                exit(0);
            }else if (buf[i] == '\n' || buf[i] == '\r') {
                int wr_flag = write(STDOUT_FILENO, map, 2);
                if (wr_flag == -1) {
                    int ernum = errno;
                    fprintf(stderr, "write failed: %s\n", strerror(ernum));
                    exit(1);
                }
                i++;
            }else {
                int wr_flag = write(STDOUT_FILENO, &buf[i], 1);
                if (wr_flag == -1) {
                    int ernum = errno;
                    fprintf(stderr, "write failed: %s\n", strerror(ernum));
                    exit(1);
                }
                i++;
            }
        }
    }
    return 0;
}
