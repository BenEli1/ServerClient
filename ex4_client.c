//Ben Eli 319086435

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <ctype.h>
#include <string.h>

//repeatable defines.
#define SERVER "to_srv.txt"
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
    int openFile, readFile;
    char client_file_path[BUF_SIZE] = {};
    char read_from_file[BUF_SIZE] = {};
    sprintf(client_file_path, "to_client_%d.txt", getpid());
    //open
    openFile = open(client_file_path, O_RDONLY);
    if (openFile == -1) {
        printf(ERROR);
        exit(-1);
    }
    //read
    readFile = read(openFile, read_from_file, BUF_SIZE);
    if (readFile == -1) {
        printf(ERROR);
        exit(-1);
    }
    //close
    close(openFile);
    printf("%s\n", read_from_file);
    remove(client_file_path);
    exit(0);
}

int main(int argc, char *argv[]) {
    //signal override first
    signal(SIGUSR1, answer_from_server_handler);
    signal(SIGALRM, alarm_handler_timeout);

    //we receive 4 arguments - PID of Server,num1,+|-|*|/,num2
    if (argc != 5) {
        printf(ERROR);
        exit(-1);
    }

    int fileOpen, i = 0;
    char *first_num = "", *second_num = "", *operation = "";
    char file_write[BUF_SIZE];
    pid_t pid = atoi(argv[1]);
    first_num = argv[2];
    operation = argv[3];
    second_num = argv[4];
    while (i < 10) {
        i++;
        //open
        fileOpen = open(SERVER, O_CREAT | O_WRONLY | O_EXCL, 0777);
        if (fileOpen < 0) {
            //using rand in order for 2 clients not to access the server at the same time.
            alarm(rand() % 5 + 1);
            pause();
        } else {
            break;
        }
    }
    if (i >= 10) {
        printf(ERROR);
        exit(-1);
    }
    //write to file,first your pid so the server knows to which client to respond,then the args for the server.
    sprintf(file_write, "%d %s %s %s\n", getpid(), first_num, operation, second_num);
    //write
    write(fileOpen, file_write, strlen(file_write));
    //close
    close(fileOpen);
    //signal the server
    kill(pid, SIGUSR1);
    signal(SIGUSR1, answer_from_server_handler);
    //30 sec timeout alarm signal
    signal(SIGALRM, alarm_handler_timeout);
    alarm(30);
    pause();
    return 0;
}
