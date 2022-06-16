//Ben Eli 319086435
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>


void client_handler(int pid) {

}


void time_out_60(int num) {
    signal(SIGALRM, time_out_60);
    printf("The server was closed because no service request was received for the last 60 seconds\n");
    while (wait(NULL) != -1);
    exit(0);
}

int main(int argc, char *argv[]) {
    //put the signals in the start in order to add them to the signal table
    signal(SIGALRM, time_out_60);
    signal(SIGUSR1, client_handler);
    remove("to_srv.txt");
    while (1) {
        alarm(60);
        pause();
    }
}

