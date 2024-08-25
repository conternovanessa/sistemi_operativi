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
pid_t a_pid; // PID of the attivatore process
pid_t al_pid;

int ENERGY_DEMAND;

// Signal handler for timer
void timer_handler(int sig) {
    if (sig == SIGALRM) {
        // Decrease ENERGY_DEMAND by 2
        ENERGY_DEMAND -= 2;

        // Synchronize access to shared memory
        sem_wait(sem);

        // Increase consumata in shared memory by 2
        shm_data->consumata += 2;

        // Release the semaphore
        sem_post(sem);

        // Print updated shared data and energy demand
        print_shared_data(shm_data);
        //printf("ENERGY_DEMAND updated: %d\n", ENERGY_DEMAND);
        //printf("consumata in shared memory updated: %d\n", shm_data->consumata);

        // Set the alarm again for the next second
        alarm(1);
    }
}

// Function to terminate processes
void terminate_processes(pid_t a_pid, pid_t *atomo_pids, int num_atoms, pid_t al_pid) {
    // Terminate attivatore process
    if (kill(a_pid, SIGTERM) == -1) {
        perror("Error terminating attivatore");
    }

    // Terminate atomo processes
    for (int i = 0; i < num_atoms; i++) {
        if (kill(atomo_pids[i], SIGTERM) == -1) {
            perror("Error terminating atomo");
        }
    }

    if (kill(al_pid, SIGTERM) == -1) {
        perror("Error terminating alimentatore");
    }
}

int main(int argc, char *argv[]) {
    const char* filename = "variabili.txt";
    SimulationParameters params = leggiVariabili(filename);
    print_line();
    printf("PARAMETERS OBTAINED FROM THE FILE: \n");
    printSimulationParameters(&params);

    ENERGY_DEMAND = params.energy_demand;

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

    // Fork atomo processes
    pid_t *atomo_pids = malloc(params.n_atom_init * sizeof(pid_t));
    if (atomo_pids == NULL) {
        perror("Failed to allocate memory for atomo PIDs");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < params.n_atom_init; i++) {
        atomo_pids[i] = create_atomo(&params.max_n_atomico, sem, shm_data);
    }

    al_pid = create_alimentatore();

    // Print start time
    time_t start_time = time(NULL);
    printf("sim duration %d\n", params.sim_duration );

    // Wait for simulation duration using a for loop
    int count =1;
    while ( count <= params.sim_duration) {
        sleep(1);
        printf("sim_duration %d\n", params.sim_duration );
        fflush(stdout);
        params.sim_duration--;
        if(params.sim_duration == 0){
            terminate_processes(a_pid, atomo_pids, params.n_atom_init, al_pid);
            printf("TIMEOUT \n");
            break;
        }
        if (shm_data -> free_energy > params.energy_explode_threshold) {
            printf("EXPLODE! \n");
            terminate_processes(a_pid, atomo_pids, params.n_atom_init, al_pid);
            break;  // Esce dal ciclo principale
        }
        if (ENERGY_DEMAND <=0) {
            printf("BLACKOUT! \n");
            terminate_processes(a_pid, atomo_pids, params.n_atom_init, al_pid);
            break;  // Esce dal ciclo principale
        }
    }

    // Print shared data after simulation time ends
    print_shared_data(shm_data);

    // Calculate elapsed time for debugging
    time_t end_time = time(NULL);

    // Wait for child processes to terminate
    int status;
    waitpid(a_pid, &status, 0);
    for (int i = 0; i < params.n_atom_init; i++) {
        waitpid(atomo_pids[i], &status, 0);
    }
    printf("All processes terminated\n");
    print_line();

    // Cleanup shared memory and semaphore
    cleanup_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);

    // Free allocated memory
    free(atomo_pids);

    exit(EXIT_SUCCESS);
}