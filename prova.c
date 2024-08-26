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
#include <string.h>

#include "src/headers/utils.h"
#include "src/headers/io.h"
#include "src/headers/process.h"

// Global variables for shared memory and semaphore
shared_data *shm_data;
sem_t *sem;


int main(int argc, char *argv[]) {
    const char *filename = "variabili.txt";
    SimulationParameters params = leggiVariabili(filename);
    print_line();
    printf("PARAMETERS OBTAINED FROM THE FILE: \n");
    printSimulationParameters(&params);

    // Initialize shared memory and semaphore
    init_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);
    print_shared_data(shm_data);
    srand(time(NULL));

    // Fork atomo processes
    shm_data->pid_array = NULL;  // Start with no allocation
    shm_data->num_processes = 0;

    for (int i = 0; i < 2; i++) {
        create_atomo(&params.max_n_atomico, &sem, &shm_data);
    }

    print_shared_data(shm_data);

    // Terminate atomo processes
    for (int i = 0; i < shm_data->num_processes; i++) {
        if (kill(shm_data->pid_array[i], SIGTERM) == -1) {
            perror("Error terminating atomo");
        }
    }

    // Free allocated memory
    free(shm_data->pid_array);
    
    printf("Cleaning up\n");
    // Cleanup resources before exiting
    cleanup_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);
    printf("Done\n");

    exit(EXIT_SUCCESS);
}


