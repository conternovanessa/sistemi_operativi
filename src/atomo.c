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
pid_t pid_rem;

void cleanup_and_exit(int sig) {
    
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

void scissione(int *num_atomico) {
    //printf("scissione called\n");
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
        //printf("Child process: read new_atomico = %d\n", new_atomico);
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
        int new_atomico = (rand() % (*num_atomico-1)) + 1;

        printf("Parent process: new_atomico = %d\n", new_atomico);
        fflush(stdout);

        if (write(pipe_fd[1], &new_atomico, sizeof(new_atomico)) != sizeof(new_atomico)) {
            perror("write failed in parent");
            exit(EXIT_FAILURE);
        }
        *num_atomico = *num_atomico - new_atomico;
        close(pipe_fd[1]); // Close write end after writing

        printf("Child process: atomico = %d\n", *num_atomico);
        // Parent process: update shared memory
        sem_wait(sem);
        shm_data->pid_array[shm_data->num_processes++] = c_pid;
        shm_data->free_energy += (*num_atomico * new_atomico - MAX(*num_atomico, new_atomico));
        sem_post(sem);
    }
}

int main(int argc
,
char *argv[]) {
    if (argc < 2) {
        perror("Necessary at least two inputs");
        exit(EXIT_FAILURE);
    }
    const char* filename = "variabili.txt";
    SimulationParameters params = leggiVariabili(filename);

    int num_atomico = atoi(argv[1]);

    connect_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);

    // Setup signal handler for SIGTERM
    struct sigaction sa;
    sa.sa_handler = cleanup_and_exit;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        perror("sigaction failed");
        exit(EXIT_FAILURE);
    }

    sigset_t set;
    siginfo_t info;

    // Block the signal we want to wait for
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    if (sigprocmask(SIG_BLOCK, &set, NULL) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    //printf("Receiver process PID: %d\n", getpid());
    //printf("Waiting for signal...\n");
    fflush(stdout);
    while(1) {


        // Wait for the signal
        if (sigwaitinfo(&set, &info) == -1) {
            perror("sigwaitinfo");
            exit(EXIT_FAILURE);
        }

        if (info.si_signo == SIGUSR1) {
            //printf("Received SIGUSR1, performing specific action.\n");
            fflush(stdout);

            //printf("Calling scissione\n");
            fflush(stdout);
            sem_wait(sem);
            shm_data->attivazioni++;
            sem_post(sem);

            if (num_atomico <= params.min_n_atomico) {
                sem_wait(sem);
                shm_data->scorie++;
                pid_rem = getpid();
                for (int i = 0; i < shm_data->num_processes; i++) {
                    if (shm_data->pid_array[i] == pid_rem) {
                        shm_data->pid_array[i] = 0;  // Set the PID to zero
                        shm_data->num_processes--;
                        break;
                    }
                }
                sem_post(sem);
                printf("killo il processo scoria\n");
                kill(getpid(), SIGTERM);
                printf("io non devo esserci \n");
            } else {
                scissione(&num_atomico);
                sem_wait(sem);
                shm_data->scissioni++;
                sem_post(sem);

            }
        }
    }

    exit(EXIT_SUCCESS);
}
