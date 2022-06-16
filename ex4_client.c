//Ben Eli 319086435
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <ctype.h>
#include <string.h>
//repeatable defines.
#define SERVER "to_srv"
#define BUF_SIZE 150
#define ERROR "ERROR_FROM_EX4\n"
#define TIMEOUT30 "Client closed because no response was received from the server for 30 seconds\n"

//handler of 30 sec timeout using alarm signal
void alarm_handler_timeout() {
    printf(TIMEOUT30);
    signal(SIGALRM, alarm_handler_timeout);
    exit(0);
}
//overriding SIGUSR1
void answer_from_server_handler() {

}

int main(int argc, char *argv[]) {
    //signal override first
    signal(SIGUSR1, answer_from_server_handler);
    signal(SIGALRM, alarm_handler_timeout);

    //we receive 4 arguments - PID of Server,num1,+|-|*|/,num2
    if (argc != 5) {
        exit(-1);
    }

    int fileOpen, i = -1;
    char *first_num, second_num, operation;
    char file_write[BUF_SIZE];
    pid_t serv_pid = atoi(argv[1]);
    first_num = argv[2];
    operation = argv[3];
    second_num = argv[4];

    while (i < 10) {
        i++;
        fileOpen = open(SERVER, O_CREAT | O_WRONLY | O_EXCL, 0777);
        if (fileOpen < 0) {
            //using rand in order for 2 clients not to access the server at the same time.
            alarm(rand() % 5 + 1);
            pause();
        } else {
            break;
        }
    }
    if (i == 10) {
        printf(ERROR);
    }
    //write to file,first your pid so the server knows to which client to respond,then the args for the server.
    sprintf(file_write, "%d %s %s %s\n", getpid(), first_num, operation, second_num);
    write(fileOpen, file_write, strlen(fileOpen));
    close(fileOpen);

    signal(SIGALRM, alarm_handler_timeout);
    kill(serv_pid, SIGUSR1);
    signal(SIGUSR1, answer_from_server_handler);
    alarm(30);
    pause();
    return 0;
}
