#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <time.h>       // Include the header for timer functions

#include "headers/utils.h"
#include "headers/process.h"
#include "headers/io.h"

shared_data *shm_data;
sem_t *sem;

void cleanup_and_exit(int sig) {
    printf("Received signal Terminate for attivatore, cleaning up and exiting.\n");
    fflush(stdout);
    
    // Unmap shared memory
    if (munmap(shm_data, sizeof(shared_data)) == -1) {
        perror("munmap failed");
    }
    
    // Close semaphore
    if (sem_close(sem) == -1) {
        perror("sem_close failed");
    }

    exit(EXIT_SUCCESS);
}

void send_signal(){
    // Seed the random number generator
    srand(time(NULL));

    // Select a random index within the range of num_processes
    int random_index = rand() % shm_data->num_processes;
    pid_t receiver_pid = shm_data->pid_array[random_index];

    // Send SIGUSR1 to the randomly selected receiver process
    if (kill(receiver_pid, SIGUSR1) == -1) {
        perror("kill");
        exit(EXIT_FAILURE);
    }

    printf("Sent SIGUSR1 to process %d\n", receiver_pid);
}

// Timer signal handler
void timer_handler(int sig) {
    send_signal();
    // Set the alarm again for the next second
    alarm(2);
}

int main(int argc, char *argv[]){

    const char* filename = "variabili.txt";
    SimulationParameters params = leggiVariabili(filename);

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

    // Setup signal handler for SIGTERM
    struct sigaction sa2;
    sa2.sa_handler = cleanup_and_exit;
    sigemptyset(&sa2.sa_mask);
    sa2.sa_flags = 0;

    if (sigaction(SIGTERM, &sa2, NULL) == -1) {
        perror("sigaction failed");
        exit(EXIT_FAILURE);
    }

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
    if (alarm(params.step_attivatore) == -1) {
        perror("alarm failed");
        exit(EXIT_FAILURE);
    }

    while(1){
        pause();
    }

    // // Unmap shared memory and close semaphore (unreachable in this example)
    // munmap(shm_data, sizeof(shared_data));
    // sem_close(sem);

    exit(EXIT_SUCCESS);
}
