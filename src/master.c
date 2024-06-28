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
#include "util/utils.h"

#define SHARED_MEM_NAME "/shared_mem"
#define SEMAPHORE_NAME "/semaphore"

// Global variables for shared memory and semaphore
shared_data *shm_data;
sem_t *sem;
pid_t a_pid, c_pid;
timer_t timerid;
volatile sig_atomic_t keep_running = 1;

// Function to handle timer signal
void print_shared_data_handler(int sig, siginfo_t *si, void *uc) {
    print_shared_data(sem, shm_data);
}

// Function to handle program termination signals
void handle_termination_signal(int sig) {
    keep_running = 0;
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

void init_shared_memory_and_semaphore() {
    // Open shared memory
    int shm_fd = shm_open(SHARED_MEM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open failed");
        exit(EXIT_FAILURE);
    }

    // Set size of shared memory
    if (ftruncate(shm_fd, sizeof(shared_data)) == -1) {
        perror("ftruncate failed");
        exit(EXIT_FAILURE);
    }

    // Map shared memory
    shm_data = mmap(0, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_data == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }

    // Initialize shared memory content
    memset(shm_data, 0, sizeof(shared_data));

    // Create semaphore
    sem = sem_open(SEMAPHORE_NAME, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open failed");
        exit(EXIT_FAILURE);
    }
}

void cleanup_shared_memory_and_semaphore() {
    // Unmap shared memory
    if (munmap(shm_data, sizeof(shared_data)) == -1) {
        perror("munmap failed");
    }

    // Close shared memory
    if (shm_unlink(SHARED_MEM_NAME) == -1) {
        perror("shm_unlink failed");
    }

    // Close semaphore
    if (sem_close(sem) == -1) {
        perror("sem_close failed");
    }

    // Unlink semaphore
    if (sem_unlink(SEMAPHORE_NAME) == -1) {
        perror("sem_unlink failed");
    }
}

void setup_timer() {
    struct sigaction sa;
    struct sigevent sev;
    struct itimerspec its;

    // Setup signal action
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = print_shared_data_handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGRTMIN, &sa, NULL) == -1) {
        perror("sigaction failed");
        exit(EXIT_FAILURE);
    }

    // Setup timer event
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGRTMIN;
    sev.sigev_value.sival_ptr = &timerid;
    if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1) {
        perror("timer_create failed");
        exit(EXIT_FAILURE);
    }

    // Start the timer
    its.it_value.tv_sec = 1;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 1;
    its.it_interval.tv_nsec = 0;
    if (timer_settime(timerid, 0, &its, NULL) == -1) {
        perror("timer_settime failed");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    const char* filename = "variabili.txt";
    SimulationParameters params = leggiVariabili(filename);
    print_line();
    printf("PARAMETERS OBTAINED FROM THE FILE: \n");
    printSimulationParameters(&params);

    int num_atomico = 0;
    char buffer[20];
    
    // Initialize shared memory and semaphore
    init_shared_memory_and_semaphore();
    print_shared_data(sem, shm_data);

    // Setup timer to call print_shared_data every second
    setup_timer();

    // Setup signal handler for termination
    struct sigaction sa_term;
    sa_term.sa_handler = handle_termination_signal;
    sigemptyset(&sa_term.sa_mask);
    sa_term.sa_flags = 0;
    if (sigaction(SIGINT, &sa_term, NULL) == -1) {
        perror("sigaction failed");
        exit(EXIT_FAILURE);
    }

    // Fork attivatore process
    a_pid = create_attivatore();

    // Fork atomo process
    for (int i = 0; i < 1; i++) {
        num_atomico = rand() % params.max_n_atomico + 1;
        c_pid = create_atomo(&num_atomico, buffer, sem, shm_data);
    }

    // Main loop to keep the program running for a specific time
    struct timespec req = {1, 0}; // 1 second sleep time
    for (int i = 0; i < 8 && keep_running; i++) {
        nanosleep(&req, NULL);
    }

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
    cleanup_shared_memory_and_semaphore();

    exit(EXIT_SUCCESS);
}
