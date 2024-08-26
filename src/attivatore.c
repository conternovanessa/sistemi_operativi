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
SimulationParameters params;

void send_signal() {
    // Seed the random number generator
    srand(time(NULL));
    
    pid_t receiver_pid = 0;
    int random_index = 0;

    int new_atom_random = (rand() % params.n_atom_init) + 1;

    sem_wait(sem);
    for(int m = 0; m < new_atom_random; m++){

        random_index = rand() % shm_data->num_processes;
        receiver_pid = shm_data->pid_array[random_index];
        
        
        shm_data->attivazioni++;
        
        
        // Send SIGUSR1 to the randomly selected receiver process
        if (kill(receiver_pid, SIGUSR1) == -1) {
            perror("kill");
            exit(EXIT_FAILURE);
        }
    }
    sem_post(sem);

}

// Timer signal handler
void timer_handler(int sig) {
    send_signal();
}

// Signal handler for SIGTERM
void sigterm_handler(int sig) {
    cleanup(&sem, &shm_data);
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]){

    const char* filename = "variabili.txt";
    params = leggiVariabili(filename);

    // Open shared memory
    connect_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);

    // Setup signal handler for SIGTERM
    struct sigaction sig_term;
    sig_term.sa_handler = sigterm_handler;
    sigemptyset(&sig_term.sa_mask);
    sig_term.sa_flags = 0;

    if (sigaction(SIGTERM, &sig_term, NULL) == -1) {
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

    create_timer(SIGALRM, params.step_attivatore, 0, params.step_attivatore, 0, &sem, &shm_data);

    while(1){
        pause();
    }

    cleanup(&sem, &shm_data);
    exit(EXIT_SUCCESS);
}
