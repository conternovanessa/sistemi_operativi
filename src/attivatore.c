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

    //printf("Sent SIGUSR1 to process %d\n", receiver_pid);
}

// Timer signal handler
void timer_handler(int sig) {
    send_signal();
    // Set the alarm again for the next second
    alarm(2);
}

// Signal handler for SIGTERM
void sigterm_handler(int sig) {
    cleanup(&sem, &shm_data);
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]){

    const char* filename = "variabili.txt";
    SimulationParameters params = leggiVariabili(filename);

    // Open shared memory
    connect_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);

    // Setup signal handler for SIGTERM
    struct sigaction sa2;
    sa2.sa_handler = sigterm_handler;
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

    cleanup(&sem, &shm_data);
    exit(EXIT_SUCCESS);
}
