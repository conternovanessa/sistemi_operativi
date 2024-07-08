#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <time.h>

#include "headers/utils.h"
#include "headers/io.h"
#include "headers/process.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

shared_data *shm_data;
sem_t *sem;

void scissione(int minimo, int *num_atomico) {
    printf("scissione called\n");
    fflush(stdout);
    
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        perror("pipe creation failed");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));

    // Fork atomo process
    pid_t c_pid = fork();
    if (c_pid == -1) {
        perror("fork to create atomo did not go well");
        exit(EXIT_FAILURE);
    }
    if (c_pid == 0) {
        // Child process: atomo
        close(pipe_fd[1]); // Close unused write end
        int new_atomico = 0;
        if (read(pipe_fd[0], &new_atomico, sizeof(new_atomico)) != sizeof(new_atomico)) {
            perror("read failed in child");
            exit(EXIT_FAILURE);
        }
        close(pipe_fd[0]); // Close read end after reading
        printf("Child process: read new_atomico = %d\n", new_atomico);
        fflush(stdout);

        char buffer[20];
        sprintf(buffer, "%d", new_atomico);
        char *atomo_args[] = {"atomo", buffer, NULL};
        if (execve("./atomo", atomo_args, NULL) == -1) {
            perror("execve failed for atomo");
            exit(EXIT_FAILURE);
        }
    } else {
        // Parent process: update shared memory
        close(pipe_fd[0]); // Close unused read end
        int new_atomico = rand() % *num_atomico + 1;
        printf("Parent process: new_atomico = %d\n", new_atomico);
        fflush(stdout);

        if (write(pipe_fd[1], &new_atomico, sizeof(new_atomico)) != sizeof(new_atomico)) {
            perror("write failed in parent");
            exit(EXIT_FAILURE);
        }
        *num_atomico = *num_atomico - new_atomico;
        close(pipe_fd[1]); // Close write end after writing
    
        // Parent process: update shared memory
        sem_wait(sem);
        shm_data->pid_array[shm_data->num_processes++] = c_pid;
        shm_data->free_energy += (*num_atomico * new_atomico - MAX(*num_atomico, new_atomico));
        sem_post(sem);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        perror("Necessary at least two inputs");
        exit(EXIT_FAILURE);
    }
    const char* filename = "variabili.txt";
    SimulationParameters params = leggiVariabili(filename);

    int minimo = params.min_n_atomico;
    int num_atomico = atoi(argv[1]);

    printf("Received num_atomico: %d \n", num_atomico);
    fflush(stdout);

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

    sigset_t set;
    struct sigaction sa;
    siginfo_t info;

    // Block the signal we want to wait for
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    if (sigprocmask(SIG_BLOCK, &set, NULL) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    printf("Receiver process PID: %d\n", getpid());
    printf("Waiting for signal...\n");
    fflush(stdout);

    // Wait for the signal
    if (sigwaitinfo(&set, &info) == -1) {
        perror("sigwaitinfo");
        exit(EXIT_FAILURE);
    }

    if (info.si_signo == SIGUSR1) {
        printf("Received SIGUSR1, performing specific action.\n");
        fflush(stdout);

        printf("Calling scissione\n");
        fflush(stdout);

        scissione(minimo, &num_atomico);
    }

    // Unmap shared memory and close semaphore (unreachable in this example)
    munmap(shm_data, sizeof(shared_data));
    sem_close(sem);

    exit(EXIT_SUCCESS);
}
