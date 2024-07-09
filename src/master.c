#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <time.h>

#include "headers/utils.h"
#include "headers/io.h"
#include "headers/process.h"

// Global variables for shared memory and semaphore
shared_data *shm_data;
sem_t *sem;
pid_t a_pid,c_pid;

void timer_handler(int sig) {
    print_shared_data(shm_data);
    // Set the alarm again for the next second
    alarm(1);
}


void terminate_processes(pid_t a_pid, pid_t c_pid) {
    // Terminate attivatore process
    if (kill(a_pid, SIGTERM) == -1) {
        perror("Error terminating attivatore");
    }
    
    for(int i = 0; i < shm_data->num_processes; i++){
        if (kill(shm_data->pid_array[i], SIGTERM) == -1) {
            perror("Error terminating atomo");
        }
    }
}

int main(int argc, char *argv[]) {
    const char* filename = "variabili.txt";
    SimulationParameters params = leggiVariabili(filename);
    print_line();
    printf("PARAMETERS OBTAINED FROM THE FILE: \n");
    printSimulationParameters(&params);
    
    // Initialize shared memory and semaphore
    init_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);
    print_shared_data(shm_data);

    
    srand(time(NULL));

    struct sigaction sa;

    // Set up the signal handler
    sa.sa_handler = timer_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("sigaction failed");
        exit(EXIT_FAILURE);
    }

    // Set the initial alarm for 1 second
    if (alarm(1) == -1) {
        perror("alarm failed");
        exit(EXIT_FAILURE);
    }

    // Fork attivatore process
    a_pid = create_attivatore();

    // Fork atomo process
    for (int i = 0; i < params.n_atom_init; i++) {
        c_pid = create_atomo(&params.max_n_atomico, sem, shm_data);
    }

    // Wait for a certain amount of time

    int count = 0;
    
    while(count < 3){
        pause();
        sleep(1); 
        count++;
    }

    // Print shared data after sleep
    print_shared_data(shm_data);

    printf("Kill all the processes\n");
    // Terminate child processes
    terminate_processes(a_pid, c_pid);

    // Wait for child processes to terminate
    int status;
    waitpid(a_pid, &status, 0);
    for(int i = 0; i < shm_data->num_processes; i++){
        waitpid(shm_data->pid_array[i], &status, 0);
    }

    printf("All processes terminated\n");
    print_line();

    // Cleanup shared memory and semaphore
    cleanup_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);

    exit(EXIT_SUCCESS);
}
