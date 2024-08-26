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

    // Terminate alimentatore
    if (kill(al_pid, SIGTERM) == -1) {
        perror("Error terminating alimentatore");
    }

    // Terminate attivatore
    if (kill(a_pid, SIGTERM) == -1) {
        perror("Error terminating attivatore");
    }
    
    // Terminate atomo processes
    int termination_counter = 0 ;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (shm_data->pid_array[i] != 0){
            if (kill(shm_data->pid_array[i], SIGTERM) == -1) {
                perror("Error terminating atomo");
            }
            // printf("Killing: %d\n", shm_data->pid_array[i]);
            // fflush(stdout);
            termination_counter++;
        }
        if (termination_counter == shm_data->num_processes){
            printf("Termination counter is: %d\n", termination_counter);
            fflush(stdout);
            break;
        } 
    }    

    // Wait for alimentatore to terminate
    int status;
    int warning_counter = 0 ;
    waitpid(al_pid, &status, 0);
    waitpid(a_pid, &status, 0);
    for (int i = 0; i < shm_data->num_processes; i++) {
        if (shm_data->pid_array[i] != 0){
            waitpid(shm_data->pid_array[i], &status, 0);
            warning_counter++;
        }
        if (warning_counter == shm_data->num_processes) break;
    }
    
    // Cleanup resources
    cleanup_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);
    exit(EXIT_SUCCESS);
}
