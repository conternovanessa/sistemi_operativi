#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "headers/process.h"
#include "headers/utils.h"

pid_t create_attivatore() {
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

pid_t create_atomo(int *max_n_atomico, sem_t *sem, shared_data *shm_data) {
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
        int num_atomico;
        if (read(pipe_fd[0], &num_atomico, sizeof(num_atomico)) != sizeof(num_atomico)) {
            perror("read failed in child");
            exit(EXIT_FAILURE);
        }
        close(pipe_fd[0]); // Close read end after reading

        char buffer[20];
        sprintf(buffer, "%d", num_atomico);
        char *atomo_args[] = {"atomo", buffer, NULL};
        if (execve("./atomo", atomo_args, NULL) == -1) {
            perror("execve failed for atomo");
            exit(EXIT_FAILURE);
        }
    } else {
        // Parent process: update shared memory
        close(pipe_fd[0]); // Close unused read end
        int num_atomico = rand() % *max_n_atomico + 1;
        if (write(pipe_fd[1], &num_atomico, sizeof(num_atomico)) != sizeof(num_atomico)) {
            perror("write failed in parent");
            exit(EXIT_FAILURE);
        }
        close(pipe_fd[1]); // Close write end after writing

        add_pid(c_pid, sem, shm_data);
    }

    return c_pid;
}

void add_pid(pid_t pid, sem_t *sem, shared_data *shm_data){
    // Parent process: update shared memory
    sem_wait(sem);
    shm_data->pid_array[shm_data->num_processes++] = pid;
    sem_post(sem);
}