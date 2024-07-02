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

void timer_handler(union sigval sv) {
    print_shared_data(shm_data);
}

void create_timer() {
    struct sigevent sev;
    struct itimerspec its;
    timer_t timerid;

    // Set up the signal event
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_value.sival_ptr = &timerid;
    sev.sigev_notify_function = timer_handler;
    sev.sigev_notify_attributes = NULL;

    // Create the timer
    if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1) {
        perror("timer_create failed");
        exit(EXIT_FAILURE);
    }

    // Set the timer to expire after 1 second, and every 1 second thereafter
    its.it_value.tv_sec = 1;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 1;
    its.it_interval.tv_nsec = 0;

    if (timer_settime(timerid, 0, &its, NULL) == -1) {
        perror("timer_settime failed");
        exit(EXIT_FAILURE);
    }
}


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

int main(int argc, char *argv[]) {
    const char* filename = "variabili.txt";
    SimulationParameters params = leggiVariabili(filename);
    print_line();
    printf("PARAMETERS OBTAINED FROM THE FILE: \n");
    printSimulationParameters(&params);
    
    // Initialize shared memory and semaphore
    init_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);
    print_shared_data(shm_data);

    // Create and start the timer
    create_timer();

    // Fork attivatore process
    a_pid = create_attivatore();

    // Fork atomo process
    for (int i = 0; i < 1; i++) {
        c_pid = create_atomo(&params.max_n_atomico, sem, shm_data);
    }

    // Wait for a certain amount of time
    sleep(6);  // Wait for 2 seconds

    // Print shared data after sleep
    print_shared_data(shm_data);

    printf("Kill all the processes\n");
    // Terminate child processes
    terminate_processes(a_pid, c_pid);

    // Wait for child processes to terminate
    int status;
    waitpid(a_pid, &status, 0);
    waitpid(c_pid, &status, 0);
    printf("All processes terminated\n");
    print_line();

    // Cleanup shared memory and semaphore
    cleanup_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);

    exit(EXIT_SUCCESS);
}
