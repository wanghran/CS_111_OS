#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <math.h>
#include <fcntl.h>
#include <pthread.h>
#include <mraa.h>

struct option long_options[] =
        {
                {"id", required_argument, NULL, 1},
                {"host", required_argument, NULL, 2},
                {"log", required_argument, NULL, 3},
                {"period", required_argument, NULL, 4},
                {"scale", required_argument, NULL, 5},
                {0, 0, 0, 0}
        };
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
FILE* fp = NULL;
int p_flag = 0;
int s_flag = 0;
int id = 0;
char* host_info = NULL;
int port_num = 0;
int sockfd;
SSL *ssl;

void but_handler();
float t_convert(float temp);
void command_handler(char *str);
void ppt(char *str);
void* threads_opt(void *tid);
SSL_CTX *init_client_CTX();
int open_port(const char* server_name, int server_port);


int main(int argc, char **argv) {

    if (argc < 5) {
        fprintf(stderr, "did not define mandatory arguments\n");
        exit(1);
    }

    while (1) {
        int opt_flag = getopt_long(argc, argv, "", long_options, NULL);
        if (opt_flag == -1) {
            break;
        }
        switch (opt_flag) {
            case 1:
            {
                if (strlen(optarg) != 9) {
                    fprintf(stderr, "id should be a 9-digit number\n");
                    exit(1);
                }
                id = atoi(optarg);
                break;
            }
            case 2:
            {
                host_info = malloc(strlen(optarg) + 1);
                host_info = optarg;
                break;
            }
            case 3:
            {
                log_flag = 1;
                if (optarg == NULL) {
                    logfd = open("log.txt", O_RDWR | O_CREAT | O_TRUNC, 0666);
                    if (logfd == -1) {
                        int errnum = errno;
                        fprintf(stderr, "cannot open a log file with error: %s\n", strerror(errnum));
                        exit(2);
                    }
                    fp = fdopen(logfd, "w");
                }else {
                    logfd = open(optarg, O_RDWR | O_CREAT | O_TRUNC, 0666);
                    if (logfd == -1) {
                        int errnum = errno;
                        fprintf(stderr, "cannot open a log file with error: %s\n", strerror(errnum));
                        exit(2);
                    }
                    fp = fdopen(logfd, "w");
                }
                break;
            }
            case 4:
            {
                period = atoi(optarg);
                p_flag = 1;
                break;
            }
            case 5:
            {
                s_flag = 1;
                if (*optarg == 'F') {
                    f_flag = 1;
                    c_flag = 0;
                }else if (*optarg == 'C') {
                    f_flag = 0;
                    c_flag = 1;
                }else {
                    fprintf(stderr, "invalid temperature argument\n");
                    exit(1);
                }
                break;
            }
            default:
            {
                fprintf(stderr, "bad long option\n");
                exit(1);
                break;
            }
        }
    }

    port_num = atoi(argv[argc - 1]);

    if (port_num == 0 || fp == NULL || host_info == NULL || id == 0) {
        fprintf(stderr, "Did not fill the mandatory arguments\n");
        exit(1);
    }

    SSL_library_init();
    SSL_CTX *ctx = init_client_CTX();
    sockfd = open_port(host_info, port_num);
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sockfd);

    if (SSL_connect(ssl) == -1) {
        fprintf(stderr, "Cannot connect to TLS server\n");
        exit(2);
    }

    char ID[20];
    memset(&ID, 0, 20);
    sprintf(ID, "ID=%d", id);
    if (SSL_write(ssl, ID, sizeof(ID)) < 0 ) {
        fprintf(stderr, "Cannot write to the server\n");
        exit(2);
    }

    fprintf(fp, "ID=%d\n", id);

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
        int pid = i;
        int ret = pthread_create(&threads[i], NULL, threads_opt,(void *) (intptr_t)pid);
        if (ret) {
            int ernum = errno;
            fprintf(stderr, "creating threads failed with error%s\n", strerror(ernum));
            exit(2);
        }
    }
    int j = 0;
    for (; j < 2; ++j) {
        int returnVal = 0;
        int ret = pthread_join(threads[j], (void**)&returnVal);
        if (ret) {
            fprintf(stderr, "joining threads %d failed with error%s\n", j,strerror(returnVal));
            exit(2);
        }
    }
    return 0;
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
        dprintf(sockfd, "%s SHUTDOWN\n", timebuf);
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

void ppt(char *str){
    if (log_flag == 1) {
        fprintf(fp, "%s\n", str);
    }
    fflush(fp);
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
        if (substring[0] == 'L' && substring[1] == 'O' && substring[2] == 'G') {
            ppt(substring);
        }else if (i - start == 3) {
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

void* threads_opt(void *pid){
    int t_id = (intptr_t)pid;
    if (t_id == 0) {
        while (1) {
            if (off_sig == 1) {
                break;
            }
            int count = SSL_read(ssl, buffer, 2048);
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
                char outBuffer[50];
                memset(&outBuffer, 0, 50);
                sprintf(outBuffer, "%s %.1f\n", timebuf, t_convert(value));
                if (log_flag == 1) {
                    fprintf(fp ,"%s %.1f\n", timebuf, t_convert(value));
                    SSL_write(ssl, outBuffer, strlen(outBuffer));
                }else {
                    SSL_write(ssl, outBuffer, strlen(outBuffer));
                }
                usleep(period * 1000000);
            }
        }

    }

    pthread_exit(NULL);
}

SSL_CTX *init_client_CTX() {
    const SSL_METHOD *client_method;
    SSL_CTX *ctx_method;

    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    client_method = SSLv23_client_method();
    ctx_method = SSL_CTX_new(client_method);
    if (ctx_method == NULL) {
        fprintf(stderr, "Failed to initialize SSL client\n");
        exit(2);
    }
    return ctx_method;
}

int open_port(const char* server_name, int server_port) {
    int server = 0;
    struct hostent *server_ent;
    struct sockaddr_in socket_address;

    server_ent = gethostbyname(server_name);
    if (server_ent == NULL) {
        fprintf(stderr, "Cannot get server\n");
        exit(1);
    }

    server = socket(PF_INET, SOCK_STREAM, 0);
    memset(&socket_address, 0, sizeof(socket_address));
    socket_address.sin_family = AF_INET;
    socket_address.sin_port = htons(server_port);
    socket_address.sin_addr.s_addr = *(long *) (server_ent->h_addr);
    if (connect(server, (struct sockaddr *)&socket_address, sizeof(socket_address)) != 0) {
        close(server);
        fprintf(stderr, "Cannot connect to server\n");
        exit(2);
    }
    return server;

}