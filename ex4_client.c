//Ben Eli 319086435
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <ctype.h>
#include <string.h>

#define SIZE 150
void alarm_handler_timeout() {
    printf("Client closed because no response was received from the server for 30 seconds\n");
    signal(SIGALRM, alarm_handler_timeout);
    exit(0);
}
void print_handler(){

}

int main(int argc, char *argv[]) {
    signal(SIGUSR1, print_handler);
    signal(SIGALRM, alarm_handler_timeout);
    if (argc < 5) {
        exit(-1);
    }
    int fileOpen;
    char * first_num, second_num, calc_opreator;
    char file_write[SIZE];
    pid_t pid;
    int i = -1;
    pid = atoi(argv[1]);
    first_num = argv[2];
    calc_opreator = argv[3];
    second_num = argv[4];
    while (i < 10) {
        i++;
        fileOpen = open("to_srv", O_CREAT | O_WRONLY | O_EXCL, 0777);
        if (fileOpen < 0) {
            alarm(rand() % 5 + 1);
            pause();
        } else {
            break;
        }
    }
    if(i==10){
        printf("ERROR_FROM_EX4\n");
    }

    sprintf(file_write, "%d %s %s %s\n", getpid(), first_num, calc_opreator, second_num);
    write(fileOpen, file_write, strlen(fileOpen));
    close(fileOpen);
    pid_t serv_pid = atoi(argv[1]);
    signal(SIGALRM, alarm_handler_timeout);
    kill(serv_pid, SIGUSR1);
    signal(SIGUSR1, print_handler);
    alarm(30);
    pause();
    return 0;
}
