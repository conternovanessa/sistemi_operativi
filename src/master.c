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

pid_t process_attivatore() {
    // Fork attivatore process
    pid_t a_pid = fork();
    if (a_pid == -1) {
        perror("fork for attivatore did not go well");
        exit(EXIT_FAILURE);
    }
    if (a_pid == 0) {
        // Child process: attivatore
        char *attivatore_args[] = {"attivatore", NULL};
        if (execve("./attivatore", attivatore_args, NULL) == -1) {
            perror("execve failed for attivatore");
            exit(EXIT_FAILURE);
        }
    }
    return a_pid;
}

pid_t create_atomo(int num_atomico, char *buffer) {
    // Fork atomo process
    pid_t c_pid = fork();
    if (c_pid == -1) {
        perror("fork to create atomo did not go well");
        terminate_processes(a_pid, c_pid); // Clean up child processes
        exit(EXIT_FAILURE);
    }
    if (c_pid == 0) {
        // Child process: atomo
        sprintf(buffer, "%d", num_atomico);
        char *atomo_args[] = {"atomo", buffer, NULL};
        if (execve("./atomo", atomo_args, NULL) == -1) {
            perror("execve failed for atomo");
            exit(EXIT_FAILURE);
        }
    } else {
        // Parent process: update shared memory
        sem_wait(sem);
        shm_data->pid_array[shm_data->num_processes++] = c_pid;
        sem_post(sem);
    }
    return c_pid;
}

void print_shared_data() {
    sem_wait(sem); // Lock the semaphore before accessing shared memory
    printf("Shared Data:\n");
    printf("attivazioni: %d\n", shm_data->attivazioni);
    printf("scissioni: %d\n", shm_data->scissioni);
    printf("free_energy: %d\n", shm_data->free_energy);
    printf("consumata: %d\n", shm_data->consumata);
    printf("scorie: %d\n", shm_data->scorie);
    printf("num_processes: %d\n", shm_data->num_processes);
    for (int i = 0; i < shm_data->num_processes; ++i) {
        printf("pid_array[%d]: %d\n", i, shm_data->pid_array[i]);
    }
    sem_post(sem); // Unlock the semaphore after accessing shared memory
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    const char* filename = "variabili.txt";
    SimulationParameters params = leggiVariabili(filename);
    printf("PARAMETERS OBTAINED FROM THE FILE: \n");
    printSimulationParameters(&params);

    int num_atomico = 0;
    char buffer[20];
    print_line();
    // Initialize shared memory and semaphore
    init_shared_memory_and_semaphore();
    print_shared_data();
    print_line();

    // Fork attivatore process
    a_pid = process_attivatore();

    // Fork atomo process
    for (int i = 0; i < 1; i++) {
        num_atomico = rand() % params.max_n_atomico + 1;
        c_pid = create_atomo(num_atomico, buffer);
    }

    // Wait for a certain amount of time
    sleep(2);  // Wait for 2 seconds

    print_line();
    // Print shared data after sleep
    print_shared_data();

    print_line();
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
