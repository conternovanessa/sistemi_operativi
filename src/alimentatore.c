#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "headers/io.h"
#include "headers/process.h"

shared_data *shm_data;
sem_t *sem;
SimulationParameters params;

// Handler per il timer
void timer_handler(int sig) {
    printf("sei in alimentatore\n");

    // Crea N_NUOVI_ATOMI processi atomo
    for (int i = 0; i < params.n_nuovi_atomi; i++) {
        printf("create new atoms \n");
        pid_t pid = create_atomo(&params.max_n_atomico, sem, shm_data);
        if (pid == -1) {
            perror("Failed to create atomo");
            exit(EXIT_FAILURE);
        }
        printf("Created atomo process with PID: %d\n", pid);
    }
}

int main() {
    const char* filename = "variabili.txt";
    params = leggiVariabili(filename);

    // Open shared memory
    int shm_fd = shm_open(SHARED_MEM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open failed");
        exit(EXIT_FAILURE);
    }

    // Map shared memory
    shm_data = mmap(0, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_data == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }

    // Open semaphore
    sem = sem_open(SEMAPHORE_NAME, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open failed");
        exit(EXIT_FAILURE);
    }

    // Set up the signal handler
    struct sigaction sa;
    sa.sa_handler = timer_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGRTMIN, &sa, NULL) == -1) {
        perror("sigaction failed");
        exit(EXIT_FAILURE);
    }

    // Create the timer
    timer_t timerid;
    struct sigevent sev;
    struct itimerspec its;

    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGRTMIN;
    sev.sigev_value.sival_ptr = &timerid;

    if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1) {
        perror("timer_create failed");
        exit(EXIT_FAILURE);
    }

    // Set the timer to expire every second
    its.it_value.tv_sec = 0;     // Initial expiration after 1 second
    its.it_value.tv_nsec = params.step_alimentazione;   // Initial expiration after 1 second
    its.it_interval.tv_sec = 0; // Interval for repeating expiration (1 second)
    its.it_interval.tv_nsec = params.step_alimentazione; // Interval for repeating expiration (1 second)

    if (timer_settime(timerid, 0, &its, NULL) == -1) {
        perror("timer_settime failed");
        exit(EXIT_FAILURE);
    }
    // Main loop, waiting for timer events
    while (1) {
        pause();  // Wait for a signal to be delivered
    }


    munmap(shm_data, sizeof(shared_data));
    sem_close(sem);

    exit(EXIT_SUCCESS);
}
