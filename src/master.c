#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include "util/utils.h"

void terminate_processes(pid_t a_pid, pid_t c_pid) {
    // Terminate attivatore process
    if (kill(a_pid, SIGTERM) == -1) {
        perror("Error terminating attivatore");
    }

    // Terminate atomo process
    if (kill(c_pid, SIGTERM) == -1) {
        perror("Error terminating atomo");
    }
}

pid_t process_attivatore(){
    // Fork attivatore process
    pid_t a_pid = fork();
    if (a_pid == -1) {
        perror("fork for attivatore did not go well");
        exit(EXIT_FAILURE);
    }
    if (a_pid == 0) {
        // Child process: attivatore
        char *attivatore_args[] = {"attivatore", NULL};
        if (execve("./attivatore", attivatore_args, NULL) == -1) {
            perror("execve failed for attivatore");
            exit(EXIT_FAILURE);
        }
    }
    return a_pid;
}

int main(int argc, char *argv[]){
    const char* filename = "variabili.txt";
    SimulationParameters params = leggiVariabili(filename);
    printf("PARAMETERS OBTAINED FROM THE FILE: \n");
    printSimulationParameters(&params);
    int num_atomico = 10;
    char buffer[20];

    // Fork attivatore process
    pid_t a_pid = process_attivatore();

    // Fork atomo process
    pid_t c_pid = fork();
    if (c_pid == -1) {
        perror("fork for atomo did not go well");
        terminate_processes(a_pid, c_pid); // Clean up child processes
        exit(EXIT_FAILURE);
    }
    if (c_pid == 0) {
        // Child process: atomo
        sprintf(buffer, "%d", num_atomico);
        char *atomo_args[] = {"atomo", buffer, NULL};
        if (execve("./atomo", atomo_args, NULL) == -1) {
            perror("execve failed for atomo");
            exit(EXIT_FAILURE);
        }
    } 

    // Wait for a certain amount of time
    sleep(2);  // Wait for 2 seconds

    printf("Kill all the process\n");
    // Terminate child processes
    terminate_processes(a_pid, c_pid);

    // Wait for child processes to terminate
    int status;
    waitpid(a_pid, &status, 0);
    waitpid(c_pid, &status, 0);
    printf("All process terminated\n");

    exit(EXIT_SUCCESS);
}
