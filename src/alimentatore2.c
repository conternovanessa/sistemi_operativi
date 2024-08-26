#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "headers/utils.h"
#include "headers/io.h"
#include "headers/process.h"

shared_data *shm_data;
sem_t *sem;
SimulationParameters params;

// Signal handler for SIGTERM
void sigterm_handler(int sig) {
    cleanup(&sem, &shm_data);
    exit(EXIT_SUCCESS);
}

// Handler for the timer using SIGALRM
void timer_handler(int sig) {
    // Create N_NUOVI_ATOMI processes
    printf("Calling create atomo\n");
    fflush(stdout);
    print_line();
    create_atomo(&params.max_n_atomico, sem, shm_data);
    printf("Done\n");
    fflush(stdout);
}

int main() {
    const char* filename = "variabili.txt";
    params = leggiVariabili(filename);

    connect_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);

    // Set up the signal handler for SIGTERM
    struct sigaction sa_term;
    sa_term.sa_handler = sigterm_handler;
    sigemptyset(&sa_term.sa_mask);
    sa_term.sa_flags = 0;

    if (sigaction(SIGTERM, &sa_term, NULL) == -1) {
        perror("sigaction failed for SIGTERM");
        cleanup(&sem, &shm_data);
        exit(EXIT_FAILURE);
    }

    // Set up the signal handler for the timer using SIGALRM
    struct sigaction sa_timer;
    sa_timer.sa_handler = timer_handler;
    sigemptyset(&sa_timer.sa_mask);
    sa_timer.sa_flags = 0;

    if (sigaction(SIGALRM, &sa_timer, NULL) == -1) {
        perror("sigaction failed for SIGALRM");
        cleanup(&sem, &shm_data);
        exit(EXIT_FAILURE);
    }

    // Use the create_timer function to create a timer that sends SIGALRM
    create_timer(SIGALRM, 0, params.step_alimentazione, 0, params.step_alimentazione, &sem, &shm_data);

    // Waiting for signals
    while (1) {
        pause();
    }

    // Cleanup is added as a safety measure
    cleanup(&sem, &shm_data);
    exit(EXIT_SUCCESS);
}
