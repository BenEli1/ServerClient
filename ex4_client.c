
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
    int open_file, read_file;
    char client_file_path[BUF_SIZE] = {};
    char read_from_file[BUF_SIZE] = {};
    sprintf(client_file_path, "to_client_%d.txt", getpid());
    //open
    open_file = open(client_file_path, O_RDONLY);
    if (open_file < 0) {
        printf(ERROR);
        exit(-1);
    }
    //read
    read_file = read(open_file, read_from_file, BUF_SIZE);
    if (read_file < 0) {
        close(open_file);
        printf(ERROR);
        exit(-1);
    }
    //close
    close(open_file);
    printf("%s", read_from_file);
    remove(client_file_path);
    exit(0);
}

int main(int argc, char *argv[]) {
    //we receive 4 arguments - PID of Server,num1,+|-|*|/,num2
    if (argc != 5) {
        printf(ERROR);
        exit(-1);
    }

    int file_open, i = 0;
    char *first_num = "", *second_num = "", *operation = "";
    char file_write[BUF_SIZE];
    pid_t pid = atoi(argv[1]);
    first_num = argv[2];
    operation = argv[3];
    second_num = argv[4];
    while (i < 10) {
        i++;
        //open
        file_open = open(SERVER, O_CREAT | O_WRONLY | O_EXCL, 0777);
        if (file_open < 0) {
            //using rand in order for 2 clients not to access the server at the same time.
            sleep(rand() % 5 + 1);
        } else {
            break;
        }
    }
    if (i >= 10) {
        printf(ERROR);
        close(file_open);
        exit(-1);
    }
    //write to file,first your pid so the server knows to which client to respond,then the args for the server.
    sprintf(file_write, "%d %s %s %s\n", getpid(), first_num, operation, second_num);
    //write
    write(file_open, file_write, strlen(file_write));
    //close
    close(file_open);
    //override signals
    signal(SIGUSR1, answer_from_server_handler);
    //signal the server
    int ret_kill= kill(pid, SIGUSR1);
    if(ret_kill<0){
        remove(SERVER);
        printf(ERROR);
        return 0;
    }
    //30 sec timeout alarm signal
    signal(SIGALRM, alarm_handler_timeout);
    alarm(30);
    pause();
    return 0;
}
