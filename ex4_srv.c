//Ben Eli 319086435

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>

//repeatable defines.
#define SERVER "to_srv.txt"
#define CLIENT "to_client_"
#define BUF_SIZE 150
#define ERROR "ERROR_FROM_EX4\n"
#define TIMEOUT60 "The server was closed because no service request was received for the last 60 seconds\n"
#define DIVIDE_BY_ZERO "CANNOT_DIVIDE_BY_ZERO\n"

void time_out_60() {
    signal(SIGALRM, time_out_60);
    printf(TIMEOUT60);
    //kill zombie process
    while (wait(NULL) != -1);
    exit(0);
}

void client_handler() {
    signal(SIGUSR1, client_handler);
    int fileOpenServer, fileReadServer, fileOpenClient, fileWriteClient;
    pid_t child_pid;
    char buffer[BUF_SIZE] = {};
    char result_string[BUF_SIZE];
    //open to_srv file and try to read from it.
    fileOpenServer = open(SERVER, O_RDONLY);
    if (fileOpenServer < 0) {
        printf(ERROR);
        exit(-1);
    }
    fileReadServer = read(fileOpenServer, buffer, BUF_SIZE);
    if (fileReadServer < 0) {
        printf(ERROR);
        exit(-1);
    }
    close(fileOpenServer);
    remove(SERVER);
    child_pid = fork();
    if (child_pid < 0) {
        printf(ERROR);
        exit(-1);
    }
    //that means we are in the child process. here we do the calculation.
    if (child_pid == 0) {

        int first_num, second_num, operation, i = 0, pid, result;
        char *split_buffer = strtok(buffer, " ");
        pid = atoi(split_buffer);
        int args[3]; // 3 args - first num,operation and second num
        char to_client_pid[BUF_SIZE];
        strcpy(to_client_pid, CLIENT);
        strcat(to_client_pid, split_buffer);
        strcat(to_client_pid, ".txt");
        split_buffer = strtok(NULL, " ");
        while (split_buffer != NULL && i < 3) {
            args[i] = atoi(split_buffer);
            i++;
            split_buffer = strtok(NULL, " ");
        }
        first_num = args[0];
        operation = args[1];
        second_num = args[2];
        fileOpenClient = open(to_client_pid, O_CREAT | O_WRONLY | O_SYNC, 0777);
        if (fileOpenClient < 0) {
            printf(ERROR);
            exit(-1);
        }
        //switch case for the proper operation
        switch (operation) {
            case 1:
                result = first_num + second_num;
                break;
            case 2:
                result = first_num - second_num;
                break;
            case 3:
                result = first_num * second_num;
                break;
            case 4:
                if (second_num == 0) {
                    //write
                    fileWriteClient = write(fileOpenClient, DIVIDE_BY_ZERO, strlen(DIVIDE_BY_ZERO));
                    if (fileWriteClient < 0) {
                        printf(ERROR);
                        exit(-1);
                    }
                    //close
                    close(fileWriteClient);
                    kill(pid, SIGUSR1);
                    exit(0);
                }
                result = first_num / second_num;
                break;
            default:
                printf(ERROR);
                exit(-1);
        }
        sprintf(result_string, "%d", result);
        fileWriteClient = write(fileOpenClient, result_string, strlen(result_string));
        if (fileWriteClient < 0) {
            printf(ERROR);
            exit(-1);
        }
        //close
        close(fileOpenClient);
        kill(pid, SIGUSR1);
        exit(0);
    } else {
        return;
    }
}


int main() {
    //put the signals in the start in order to add them to the signal table
    signal(SIGALRM, time_out_60);
    signal(SIGUSR1, client_handler);
    remove(SERVER);
    while (1) {
        alarm(60);
        pause();
    }
}

