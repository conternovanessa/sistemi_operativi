#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/wait.h>

#include "headers/utils.h"
#include "headers/io.h"
#include "headers/process.h"

#define READ_END 0
#define WRITE_END 1

shared_data *shm_data;
sem_t *sem;

void scissione(int numero_atomico, int write_fd);

void processa_atomo(int minimo, int *atomico) {
    printf("in processa atomo\n");
    if(*atomico <= minimo){
        shm_data->scorie++;
        printf("minimo\n");
    }else{
        printf("cercando di fare una scissione \n");
        int pipe_pf[2];
        if(pipe(pipe_pf) == -1){
            perror("pipe failed");
            exit(EXIT_FAILURE);
        }
        pid_t pid = fork();
        if(pid == -1) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        } else if(pid == 0) {
            printf("nuovo atomo\n");
            close(pipe_pf[WRITE_END]);
            int n1, n2;
            read(pipe_pf[READ_END], &n1, sizeof(n1));
            read(pipe_pf[READ_END], &n2, sizeof(n2));
            if (n1 != n2) {
                int massimo = (n1 > n2) ? n1 : n2;
                shm_data->free_energy *= n1 * n2 - massimo;
            }
            char buffer[20];
            sprintf(buffer, "%d", n1);
            char *atomo_args[] = {"./atomo", buffer, NULL};
            if (execve("./atomo", atomo_args, NULL) == -1) {
                perror("execve failed for atomo");
                exit(EXIT_FAILURE);
            }
            *atomico = n2;
            shm_data->num_processes++;
        } else {
            close(pipe_pf[READ_END]);
            shm_data->scissioni++;
            scissione(*atomico, pipe_pf[WRITE_END]);
            close(pipe_pf[WRITE_END]);
            wait(NULL);
        }
    }
    // Unmap shared memory and close semaphore (unreachable in this example)
    munmap(shm_data, sizeof(shared_data));
    sem_close(sem);
}

void scissione(int numero_atomico, int write_fd) {
    int n1 = (rand() % (numero_atomico - 2)) + 1;
    int n2 = numero_atomico - n1;
    write(write_fd, &n1, sizeof(n1));
    write(write_fd, &n2, sizeof(n2));
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        perror("Necessary at least two inputs");
        exit(EXIT_FAILURE);
    }
    const char* filename = "variabili.txt";
    SimulationParameters params = leggiVariabili(filename);

    int num_atomico = atoi(argv[1]);

    printf("Received num_atomico: %d \n", num_atomico);

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

    // Wait for the signal
    if (sigwaitinfo(&set, &info) == -1) {
        perror("sigwaitinfo");
        exit(EXIT_FAILURE);
    }

    if (info.si_signo == SIGUSR1) {
        printf("Received SIGUSR1, performing specific action.\n");
        processa_atomo(minimo,&num_atomico);
    }

    exit(EXIT_SUCCESS);
}
