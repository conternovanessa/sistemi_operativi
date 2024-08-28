#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <time.h>

#include "headers/utils.h"
#include "headers/io.h"
#include "headers/process.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

shared_data *shm_data;
sem_t *sem;

void scissione(int *num_atomico);

void sigterm_handler(int sig){
    cleanup(&sem, &shm_data);
    exit(EXIT_SUCCESS);
}

void scissione(int *num_atomico){
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1){
        perror("pipe creation failed");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));

    pid_t c_pid = fork();
    if (c_pid == -1){
        perror("fork failed for atomo.c");
        printf("MELTDOWN!\n");
        kill(shm_data->master_pid, SIGTERM);
        exit(EXIT_FAILURE);
    }
    if (c_pid == 0){
        // Child process: atomo
        close(pipe_fd[1]); // Close unused write end
        int new_atomico = 0;
        if (read(pipe_fd[0], &new_atomico, sizeof(new_atomico)) != sizeof(new_atomico)){
            perror("read failed in child");
            exit(EXIT_FAILURE);
        }
        close(pipe_fd[0]); // Close read end after reading

        char buffer[20];
        sprintf(buffer, "%d", new_atomico);
        char *atomo_args[] = {"atomo", buffer, NULL};
        if (execve("./atomo", atomo_args, NULL) == -1){
            perror("execve failed for atomo");
            exit(EXIT_FAILURE);
        }
    }
    else{
        // Parent process: update shared memory
        close(pipe_fd[0]); // Close unused read end
        int new_atomico = (rand() % (*num_atomico - 1)) + 1;

        if (write(pipe_fd[1], &new_atomico, sizeof(new_atomico)) != sizeof(new_atomico)){
            perror("write failed in parent");
            exit(EXIT_FAILURE);
        }
        close(pipe_fd[1]); // Close write end after writing

        *num_atomico -= new_atomico;

        add_pid(c_pid, sem, shm_data);
        shm_data->free_energy += (*num_atomico * new_atomico - MAX(*num_atomico, new_atomico));
        shm_data->scissioni++;
    }
}

int main(int argc, char *argv[]){
    if (argc < 2){
        fprintf(stderr, "Usage: %s <num_atomico>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int num_atomico = atoi(argv[1]);
    if (num_atomico <= 0){
        fprintf(stderr, "Invalid value for num_atomico\n");
        exit(EXIT_FAILURE);
    }

    const char *filename = "variabili.txt";
    SimulationParameters params = leggiVariabili(filename);

    connect_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);

    struct sigaction sa_term;
    sa_term.sa_handler = sigterm_handler;
    sigemptyset(&sa_term.sa_mask);
    sa_term.sa_flags = 0;
    if (sigaction(SIGTERM, &sa_term, NULL) == -1){
        perror("sigaction failed for SIGTERM");
        exit(EXIT_FAILURE);
    }

    sigset_t set;
    siginfo_t info;

    // Block SIGUSR1 and set up signal handling
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    if (sigprocmask(SIG_BLOCK, &set, NULL) == -1){
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    while (1){
        // Wait for the signal
        if (sigwaitinfo(&set, &info) == -1){
            perror("sigwaitinfo");
            continue; // Continue waiting for the next signal if interrupted
        }

        if (info.si_signo == SIGUSR1){

            if (num_atomico <= params.min_n_atomico){
                sem_wait(sem);
                shm_data->scorie++;
                pid_t pid_rem = getpid();

                for (int i = 0; i < shm_data->num_processes; i++){
                    if (shm_data->pid_array[i] == pid_rem){
                        shm_data->num_processes--;
                        // Move the last PID to the current index to fill the gap
                        shm_data->pid_array[i] = shm_data->pid_array[shm_data->num_processes];
                        shm_data->pid_array[shm_data->num_processes] = 0; // Clear the last entry
                        break;
                    }
                }
                sem_post(sem);

                kill(pid_rem, SIGTERM);
            }
            else{
                scissione(&num_atomico);
            }
        }
    }

    cleanup(&sem, &shm_data);
    exit(EXIT_SUCCESS);
}
