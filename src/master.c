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
#include <errno.h>

#include "headers/utils.h"
#include "headers/io.h"
#include "headers/process.h"

// Global variables for shared memory and semaphore
shared_data *shm_data;
sem_t *sem;

pid_t al_pid;
pid_t a_pid;
SimulationParameters params;

// Signal handler for timer
void print_and_consume(int sig) {
    
    print_shared_data(shm_data);


    sem_wait(sem);
    shm_data -> free_energy -= params.energy_demand;
    shm_data -> consumata += params.energy_demand;
    sem_post(sem);

    if (shm_data->scissioni >= 1 && params.energy_demand >= shm_data->free_energy) {
        printf("BLACKOUT!\n");
        kill(shm_data->master_pid, SIGTERM);
    }

    if (shm_data->free_energy >= params.energy_explode_threshold) {
        printf("EXPLODE!\n");
        kill(shm_data->master_pid, SIGTERM);
    }
}

    int safe_terminate_process(pid_t pid) {
    if (kill(pid, 0) == 0) {
        // Process exists, try to terminate it
        if (kill(pid, SIGTERM) == 0) {
            return 0;
        } else {
            perror("Error sending SIGTERM");
            return -1;
        }
    } else {
        if (errno == ESRCH) {
            return 0;  // Not an error, process is already gone
        } else {
            perror("Error checking process existence");
            return -1;
        }
    }
}


void kill_all_processes(pid_t al_pid, pid_t a_pid, shared_data *shm_data) {
    // Terminate alimentatore
    if (safe_terminate_process(al_pid) == -1) {
        fprintf(stderr, "Error terminating alimentatore\n");
    }
    
    // Terminate attivatore
    if (safe_terminate_process(a_pid) == -1) {
        fprintf(stderr, "Error terminating attivatore\n");
    }

    // Terminate atomo processes
    for (int i = 0; i < shm_data->num_processes; i++) {
        if (safe_terminate_process(shm_data->pid_array[i]) == -1) {
            fprintf(stderr, "Error terminating atomo %d\n", i);
        }
    }

    // Wait for all processes to terminate
    int status;
    waitpid(al_pid, &status, 0);
    waitpid(a_pid, &status, 0);
    for (int i = 0; i < shm_data->num_processes; i++) {
        waitpid(shm_data->pid_array[i], &status, 0);
    }
}

    void sigterm_handler(int signum) {
        printf("Master process received SIGTERM. Cleaning up and exiting.\n");
        kill_all_processes(al_pid, a_pid, shm_data);
        cleanup_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);
        exit(EXIT_SUCCESS);
    }

    int main(int argc, char *argv[]) {

        pid_t master_pid = getpid();

        const char *filename = "variabili.txt";
        params = leggiVariabili(filename);
        print_line();
        printf("PARAMETERS OBTAINED FROM THE FILE: \n");
        printSimulationParameters(&params);

        // Initialize shared memory and semaphore
        init_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);
        shm_data -> master_pid = master_pid;
        srand(time(NULL));

        for (int i = 0; i < params.n_atom_init; i++) {
            create_atomo(&params.max_n_atomico, sem, shm_data);
        }

        pid_t al_pid = create_alimentatore(shm_data);
        if (al_pid == -1) {
            perror("Failed to create alimentatore");
            cleanup_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);
            exit(EXIT_FAILURE);
        }
        pid_t a_pid = create_attivatore(shm_data);
        if (a_pid == -1) {
            perror("Failed to create alimentatore");
            cleanup_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);
            exit(EXIT_FAILURE);
        }


        struct sigaction sa_term;
        sa_term.sa_handler = sigterm_handler;
        sigemptyset(&sa_term.sa_mask);
        sa_term.sa_flags = 0;
        if (sigaction(SIGTERM, &sa_term, NULL) == -1) {
            perror("sigaction failed");
            cleanup_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);
            exit(EXIT_FAILURE);
        }

        struct sigaction sa;
        // Set up the signal handler for print_and_consume shared data
        sa.sa_handler = print_and_consume;
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
        }

        printf("TIMEOUT!\n");
        kill(shm_data->master_pid, SIGTERM);

        exit(EXIT_SUCCESS);
    }

