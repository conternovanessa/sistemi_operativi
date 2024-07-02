#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>

#include "headers/utils.h"

int main(int argc, char *argv[]){
    // Open shared memory
    int shm_fd = shm_open(SHARED_MEM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open failed");
        exit(EXIT_FAILURE);
    }

    // Map shared memory
    shared_data *shm_data = mmap(0, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_data == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }

    // Open semaphore
    sem_t *sem = sem_open(SEMAPHORE_NAME, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open failed");
        exit(EXIT_FAILURE);
    }

    sleep(4);

    pid_t receiver_pid = shm_data->pid_array[0];

    // Send SIGUSR1 to the receiver process
    if (kill(receiver_pid, SIGUSR1) == -1) {
        perror("kill");
        exit(EXIT_FAILURE);
    }

    printf("Sent SIGUSR1 to process %d\n", receiver_pid);

    exit(EXIT_SUCCESS);
}
