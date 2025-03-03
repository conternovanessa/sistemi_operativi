#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "headers/process.h"
#include "headers/utils.h"

#define SHM_KEY 1234  

pid_t create_alimentatore(shared_data *shm_data) {
    pid_t al_pid = fork();
    if (al_pid == -1) {
        perror("fork failed for create_alimentatore");
        printf("MELTDOWN!");
        kill(shm_data->master_pid, SIGTERM);
        exit(EXIT_FAILURE);
    }
    if (al_pid == 0) {
        // Child process: alimentatore
        char *alimentatore_args[] = {"alimentatore", NULL};
        if (execve("./alimentatore", alimentatore_args, NULL) == -1) {
            perror("execve failed for alimentatore");
            exit(EXIT_FAILURE);
        }
    }
    return al_pid;
}

pid_t create_attivatore(shared_data *shm_data) {
    // Fork attivatore process
    pid_t a_pid = fork();
    if (a_pid == -1) {
        perror ("fork failed for create_attivatore");
        printf("MELTDOWN!\n");
        kill(shm_data->master_pid, SIGTERM);
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

    // Fork atomo process
    pid_t c_pid = fork();
    if (c_pid == -1) {
        printf("MELTDOWN!\n");
        kill(shm_data->master_pid, SIGTERM);
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


void add_pid(pid_t pid, sem_t *sem, shared_data *shm_data) {

    if (shm_data->num_processes < MAX_PROCESSES) {
        // Add the new PID to the array
        sem_wait(sem);  // Wait for semaphore access
        shm_data->pid_array[shm_data->num_processes++] = pid;
        sem_post(sem);  // Release semaphore access
    } else {
        // Handle the case where the array is full
        fprintf(stderr, "Error: PID array is full. Cannot add more PIDs.\n");
        fprintf(stderr, "MEMORY FULL!\n");
        
        kill(shm_data->master_pid, SIGTERM);
    }
}


void connect_shared_memory_and_semaphore(const char* sem_name, sem_t** sem, int* shmid, shared_data** shm_data) {
    // Obtain ID of shared memory
    *shmid = shmget(SHM_KEY, sizeof(shared_data), 0666);
    if (*shmid == -1) {
        perror("shmget failed in connect_shared_memory_and_semaphore");
        exit(EXIT_FAILURE);
    }

    // Connect shared memory
    *shm_data = attach_shared_memory(*shmid);

    // Open semaphore
    *sem = sem_open(sem_name, 0);
    if (*sem == SEM_FAILED) {
        perror("sem_open failed in connect_shared_memory_and_semaphore");
        detach_shared_memory(*shm_data);
        exit(EXIT_FAILURE);
    }
}


int create_shared_memory() {
    int shmid = shmget(SHM_KEY, sizeof(shared_data), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }
    return shmid;
}

shared_data* attach_shared_memory(int shmid) {
    void* shm_ptr = shmat(shmid, NULL, 0);
    if (shm_ptr == (void*)-1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }
    return (shared_data*)shm_ptr;
}

void detach_shared_memory(shared_data* shm_data) {
    if (shmdt(shm_data) == -1) {
        perror("shmdt failed");
        exit(EXIT_FAILURE);
    }
}

void remove_shared_memory(int shmid) {
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl failed");
        exit(EXIT_FAILURE);
    }
}

void init_shared_memory_and_semaphore(const char* sem_name, sem_t** sem, int* shmid, shared_data** shm_data) {
    // create shared memory
    *shmid = create_shared_memory();

    // Connect to shared memory
    *shm_data = attach_shared_memory(*shmid);

    // Initializes the shared memory content
    memset(*shm_data, 0, sizeof(shared_data));

    // Create semaphore
    *sem = sem_open(sem_name, O_CREAT, 0666, 1);
    if (*sem == SEM_FAILED) {
        perror("sem_open failed");
        exit(EXIT_FAILURE);
    }
}

void cleanup_shared_memory_and_semaphore(const char* sem_name, sem_t** sem, int shmid, shared_data** shm_data) {
    // detach shared memory
    detach_shared_memory(*shm_data);

    //remove shared memory
    remove_shared_memory(shmid);

    // Close semaphore
    if (sem_close(*sem) == -1) {
        perror("sem_close failed");
    }

    // Remove semaphore
    if (sem_unlink(sem_name) == -1) {
        perror("sem_unlink failed");
    }
}

void cleanup(sem_t** sem, shared_data** shm_data) {
    //detach shared memory
    if (*shm_data != NULL) {
        detach_shared_memory(*shm_data);
    }

    // Close semaphore
    if (*sem != SEM_FAILED) {
        sem_close(*sem);
    }
}

void create_timer(int signo, int tv_sec, int tv_nsec, int interval_tv_sec, int interval_tv_nsec, sem_t** sem, int* shmid, shared_data** shm_data) {
    struct sigevent sev;
    struct itimerspec its;
    timer_t timerid;

    // Set up the signal event
         
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = signo;          
    sev.sigev_value.sival_ptr = &timerid;

    // Create the timer
    if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1) {
        perror("timer_create failed");
        cleanup(sem, shm_data);
        exit(EXIT_FAILURE);
    }


    // Set the timer to expire at the given intervals
    its.it_value.tv_sec = tv_sec;
    its.it_value.tv_nsec = tv_nsec;
    its.it_interval.tv_sec = interval_tv_sec;
    its.it_interval.tv_nsec = interval_tv_nsec;

    if (timer_settime(timerid, 0, &its, NULL) == -1) {
        perror("timer_settime failed");
        cleanup(sem, shm_data);
        exit(EXIT_FAILURE);
    }
}
