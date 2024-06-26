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
#include <sys/time.h>
#include "util/utils.h"

#define SHARED_MEM_NAME "/shared_mem"
#define SEMAPHORE_NAME "/semaphore"

// Global variables for shared memory and semaphore
shared_data *shm_data;
sem_t *sem;
pid_t a_pid,c_pid;

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
void timer_handler(int signum) {
    print_shared_data(sem, shm_data);
}

int main(int argc, char *argv[]) {

    struct itimerval timer_stampa;

    // Inizializza sem e shm_data qui...

    // Imposta il gestore del segnale
    signal(SIGALRM, timer_handler);

    // Configura il timer
    timer_stampa.it_value.tv_sec = 1;  // Primo timeout dopo 1 secondo
    timer_stampa.it_value.tv_usec = 0;
    timer_stampa.it_interval.tv_sec = 1;  // Ripeti ogni 1 secondo
    timer_stampa.it_interval.tv_usec = 0;

    // Avvia il timer
    setitimer(ITIMER_REAL, &timer_stampa, NULL);

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe failed");
        exit(1);
    }

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

    // Fork attivatore process
    a_pid = create_attivatore();

    // Fork atomo process
    for (int i = 0; i < params.n_atom_init ; i++) {
        pid_t pid = fork();

        if (pid == -1) {
            // Errore nella fork
            perror("fork failed");
            exit(1);
        } else if (pid == 0) {
            // Processo figlio
            int num_atomico;
            if (read(pipefd[0], &num_atomico, sizeof(int)) == -1) {
                perror("read from pipe failed");
                exit(1);
            }
            c_pid = create_atomo(&num_atomico, buffer, sem, shm_data);
            exit(0);  // Il figlio termina dopo aver creato l'atomo
        } else {
            // Processo padre
            num_atomico = rand() % params.max_n_atomico + 1;
            if (write(pipefd[1], &num_atomico, sizeof(int)) == -1) {
                perror("write to pipe failed");
                exit(1);
            }
            // Attendi che il figlio termini
            wait(NULL);
        }
    }

    // Wait for a certain amount of time
    sleep(2);  // Wait for 2 seconds

    // Print shared data after sleep
    //print_shared_data(sem, shm_data);


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
