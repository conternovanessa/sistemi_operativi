#include <stdlib.h>
#include <stdio.h>
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

// Signal handler for timer
void printing(int sig) {
    print_shared_data(shm_data);
}

void kill_all_processes(pid_t al_pid, pid_t a_pid, shared_data* shm_data){
    // Terminate alimentatore
    if (kill(al_pid, SIGTERM) == -1) {
        perror("Error terminating alimentatore");
    }

    // Terminate attivatore
    if (kill(a_pid, SIGTERM) == -1) {
        perror("Error terminating attivatore");
    }
    
    // Terminate atomo processes
    for (int i = 0; i < shm_data->num_processes; i++) {
        if (kill(shm_data->pid_array[i], SIGTERM) == -1) {
            perror("Error terminating atomo");
        }
    }    

    // Wait for alimentatore to terminate
    int status;
    waitpid(al_pid, &status, 0);
    waitpid(a_pid, &status, 0);
    for (int i = 0; i < shm_data->num_processes; i++) {
        waitpid(shm_data->pid_array[i], &status, 0);
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
    srand(time(NULL));

    shm_data->num_processes = 0;

    for(int i = 0; i < params.n_atom_init; i++){
        create_atomo(&params.max_n_atomico, sem, shm_data);
    }


    pid_t al_pid = create_alimentatore();
    if (al_pid == -1) {
        perror("Failed to create alimentatore");
        cleanup_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);
        exit(EXIT_FAILURE);
    }

    pid_t a_pid = create_attivatore();
    if (a_pid == -1) {
        perror("Failed to create alimentatore");
        cleanup_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);
        exit(EXIT_FAILURE);
    }


    struct sigaction sa;

    // Set up the signal handler for printing shared data
    sa.sa_handler = printing;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("sigaction failed");
        cleanup_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);
        exit(EXIT_FAILURE);
    }

    create_timer(SIGALRM, 1, 0, 1, 0, &sem, &shm_data);

    // Wait for simulation duration
    for (int count = 1; count <= params.sim_duration; count++) {
        sleep(1);
        printf("count %d\n", count);
        fflush(stdout);
    }

    kill_all_processes(al_pid, a_pid, shm_data);
    
    // Cleanup resources
    cleanup_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);
    exit(EXIT_SUCCESS);
}
