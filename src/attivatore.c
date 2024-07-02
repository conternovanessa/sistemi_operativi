#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <time.h>       // Include the header for timer functions

#include "headers/utils.h"

shared_data *shm_data;
sem_t *sem;

void send_signal(){
    pid_t receiver_pid = shm_data->pid_array[0];

    // Send SIGUSR1 to the receiver process
    if (kill(receiver_pid, SIGUSR1) == -1) {
        perror("kill");
        exit(EXIT_FAILURE);
    }

    printf("Sent SIGUSR1 to process %d\n", receiver_pid);
}

// Timer signal handler
void timer_handler(union sigval sv) {
    send_signal();
}

void create_timer() {
    struct sigevent sev;
    struct itimerspec its;
    timer_t timerid;

    // Set up the signal event
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_value.sival_ptr = &timerid;
    sev.sigev_notify_function = timer_handler;
    sev.sigev_notify_attributes = NULL;

    // Create the timer
    if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1) {
        perror("timer_create failed");
        exit(EXIT_FAILURE);
    }

    // Set the timer to expire after 2 seconds, and every 2 seconds thereafter
    its.it_value.tv_sec = 2;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 2;
    its.it_interval.tv_nsec = 0;

    if (timer_settime(timerid, 0, &its, NULL) == -1) {
        perror("timer_settime failed");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]){
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

    // Create the timer
    create_timer();

    pause();

    // Unmap shared memory and close semaphore (unreachable in this example)
    munmap(shm_data, sizeof(shared_data));
    sem_close(sem);

    exit(EXIT_SUCCESS);
}
