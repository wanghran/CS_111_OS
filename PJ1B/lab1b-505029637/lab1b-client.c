//  Name: Haoran Wang
//  ID: 505029637
//  Email: hwan252@ucla.edu
//  Created by Rex Wang on 10/15/17.
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


struct termios saved_attributes;
/*following reset and set code are from gnu sample code https://www.gnu.org/software/libc/manual/html_node/Noncanon-Example.html#Noncanon-Example    */

char* get_key(char *file);
void set_input_mode (void);
void reset_input_mode (void);
void reverse(char s[]);
void itoa(int n, char s[]);
void write_check(int wr_flag1, int wr_flag2);
char* get_key(char *file);

int log_flag, enc_flag, logfd;
int port;
MCRYPT cryptfd;
MCRYPT decryptfd;

int main(int argc, char *argv[]) {
    
    const char map[2] = {'\r', '\n'};
    struct option long_options[] =
    {
        {"port", required_argument, NULL, 1},
        {"log", required_argument, NULL, 2},
        {"encrypt", required_argument, NULL, 3},
        {0, 0, 0, 0}
    };
    
    while(1) {
        int opt_flag = getopt_long(argc, argv, "", long_options, NULL);
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
                log_flag = 1;
                logfd = open(optarg, O_RDWR | O_CREAT | O_TRUNC, 0666);
                if (logfd == -1) {
                    int ernum = errno;
                    fprintf(stderr, "cannot create a log file with error: %s", strerror(ernum));
                    exit(1);
                }
                
                break;
            }
            case 3:
            {
                enc_flag = 1;
                char *key = get_key(optarg);
                cryptfd = mcrypt_module_open("twofish", NULL, "cfb", NULL);
                if (cryptfd == MCRYPT_FAILED) {
                    fprintf(stderr, "Module failed");
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
    
    set_input_mode();
    
    struct hostent *server;
    struct sockaddr_in serv_addr;
    int socfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socfd == -1) {
        int ernum = errno;
        fprintf(stderr, "socket() failed with error: %s", strerror(ernum));
        exit(1);
    }
    server = gethostbyname("localhost");
    if(server == NULL) {
        int ernum = h_errno;
        fprintf(stderr, "Cannot get host with error: %s", hstrerror(ernum));
        exit(1);
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy(&serv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
    serv_addr.sin_port = htons(port);
    
    if (connect(socfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        int ernum = errno;
        fprintf(stderr, "Connecting failed with error: %s", strerror(ernum));
        exit(1);
    }
    
    
    struct pollfd fds[2];
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;
    fds[1].fd = socfd;
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
        if (fds[0].revents & POLLIN) {
            char buf[2048];
            int count = read(STDIN_FILENO, buf, 2048);
            if (count == -1) {
                int ernum = errno;
                fprintf(stderr, "read from StdIn failed: %s\n", strerror(ernum));
                exit(1);
            }
            int wr_flag2 = write(STDOUT_FILENO, buf, count);
            if (enc_flag == 1) {
                if (mcrypt_generic(cryptfd, buf, count) != 0) {
                    fprintf(stderr, "Encrypt message out failed");
                    exit(1);
                }
            }
            int wr_flag = write(socfd, buf, count);
            write_check(wr_flag, wr_flag2);
            if (log_flag == 1 && count != 0) {
                char str[14] = "SENT # bytes: ";
                str[5] = '0' + count;
                if(write(logfd, str, 14) == -1 ) {
                    int errnum = errno;
                    fprintf(stderr, "log file is unwritable: %s", strerror(errnum));
                    exit(1);
                }
                write(logfd, buf, count);
                write(logfd, "\n", 1);
            }
            memset(buf, 0, sizeof(buf));
        }
        if (fds[0].revents & (POLLHUP & POLLERR)) {
            int ernum = errno;
            fprintf(stderr, "SdtIn error: %s\n", strerror(ernum));
            close(socfd);
            exit(1);
        }
        if (fds[1].revents & POLLIN) {
            char buf[2048];
            int count = read(socfd, buf, 2048);
            if (count == -1) {
                int ernum = errno;
                fprintf(stderr, "read from socket failed: %s\n", strerror(ernum));
                exit(1);
            }
            if (count < 1) {
                close(logfd);
                close(socfd);
                reset_input_mode();
                mcrypt_generic_deinit(cryptfd);
                mcrypt_module_close(cryptfd);
                mcrypt_generic_deinit(decryptfd);
                mcrypt_module_close(decryptfd);
                exit(0);
            }
            if (log_flag == 1 && count != 0) {
                char str1[] = "RECEIVED ";
                char str2[] = " bytes: ";
                char str3[10];
                itoa(count, str3);
                if(write(logfd, "\n", 1) == -1 ) {
                    int errnum = errno;
                    fprintf(stderr, "log file is unwritable: %s", strerror(errnum));
                    exit(1);
                }
                write(logfd, str1, strlen(str1));
                write(logfd, str3, strlen(str3));
                write(logfd, str2, strlen(str2));
                write(logfd, buf, count);
                write(logfd, "\n", 1);
            }
            if (enc_flag == 1) {
                if (mdecrypt_generic(decryptfd, buf, count)) {
                    fprintf(stderr, "Decrypt message in failed");
                    exit(1);
                }
            }
            int i = 0;
            while(i < count) {
                if(buf[i] == '\n' || buf[i] == '\r') {
                    int wr_flag = write(STDOUT_FILENO, map, 2);
                    if (wr_flag == -1) {
                        int ernum = errno;
                        fprintf(stderr, "echo return from socket failed, error: %s\n", strerror(ernum));
                        exit(1);
                    }
                }else {
                    int wr_flag = write(STDOUT_FILENO, &buf[i], 1);
                    if (wr_flag == -1) {
                        int ernum = errno;
                        fprintf(stderr, "echo return from socket failed, error: %s\n", strerror(ernum));
                        exit(1);
                    }
                }
                i++;
            }
            memset(buf, 0, sizeof(buf));
        }
        if(fds[1].revents & (POLLHUP + POLLERR)) {
                close(logfd);
                close(socfd);
                reset_input_mode();
                mcrypt_generic_deinit(cryptfd);
                mcrypt_module_close(cryptfd);
                mcrypt_generic_deinit(decryptfd);
                mcrypt_module_close(decryptfd);
                exit(0);
        }
    }

    
        return 0;
}

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

void reverse(char s[])
{
    int i, j;
    char c;
    
    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

void itoa(int n, char s[])
{
    int i, sign;
    
    if ((sign = n) < 0)  /* record sign */
        n = -n;          /* make n positive */
    i = 0;
    do {       /* generate digits in reverse order */
        s[i++] = n % 10 + '0';   /* get next digit */
    } while ((n /= 10) > 0);     /* delete it */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}

void write_check(int wr_flag1, int wr_flag2) {
    if (wr_flag1 == -1) {
        int ernum = errno;
        fprintf(stderr, "write to socket failed: %s\n", strerror(ernum));
        exit(1);
    }
    if (wr_flag2 == -1) {
        int ernum = errno;
        fprintf(stderr, "echo on screen failed: %s\n", strerror(ernum));
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

