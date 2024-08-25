#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#include "headers/process.h"
#include "headers/utils.h"

pid_t create_alimentatore(){
    pid_t al_pid = fork();
    if (al_pid == -1) {
        perror("fork for alimentatore did not go well");
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

    //srand(time(NULL));

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

void init_shared_memory_and_semaphore(const char* sem_name, sem_t** sem, const char* shared_name, shared_data** shm_data) {
    // Open shared memory
    int shm_fd = shm_open(shared_name, O_CREAT | O_RDWR, 0666);
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
    *shm_data = mmap(0, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (*shm_data == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }

    // Initialize shared memory content
    memset(*shm_data, 0, sizeof(shared_data));

    // Create semaphore
    *sem = sem_open(sem_name, O_CREAT, 0666, 1);
    if (*sem == SEM_FAILED) {
        perror("sem_open failed");
        exit(EXIT_FAILURE);
    }
}

void connect_shared_memory_and_semaphore(const char* sem_name, sem_t** sem, const char* shared_name, shared_data** shm_data) {

    // Open shared memory
    int shm_fd = shm_open(shared_name, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open failed");
        exit(EXIT_FAILURE);
    }

    // Map shared memory
    *shm_data = mmap(0, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (*shm_data == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }

    close(shm_fd);

    // Open semaphore
    *sem = sem_open(sem_name, 0);
    if (*sem == SEM_FAILED) {
        perror("sem_open failed");
        exit(EXIT_FAILURE);
    }

}

void cleanup_shared_memory_and_semaphore(const char* sem_name, sem_t** sem, const char* shared_name, shared_data** shm_data) {
    // Unmap shared memory
    if (munmap(*shm_data, sizeof(shared_data)) == -1) {
        perror("munmap failed");
    }

    // Close shared memory
    if (shm_unlink(shared_name) == -1) {
        perror("shm_unlink failed");
    }

    // Close semaphore
    if (sem_close(*sem) == -1) {
        perror("sem_close failed");
    }

    // Unlink semaphore
    if (sem_unlink(sem_name) == -1) {
        perror("sem_unlink failed");
    }
}

// Cleanup function
void cleanup(sem_t** sem, shared_data** shm_data) {
    // Unmap shared memory
    if (*shm_data != MAP_FAILED) {
        munmap(*shm_data, sizeof(shared_data));
    }

    // Close semaphore
    if (*sem != SEM_FAILED) {
        sem_close(*sem);
    }
}

void create_timer(int signo, int tv_sec, int tv_nsec, int interval_tv_sec, int interval_tv_nsec, sem_t** sem, shared_data** shm_data) {
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