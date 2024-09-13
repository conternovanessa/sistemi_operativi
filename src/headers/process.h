#ifndef PROCESS_H
#define PROCESS_H

#include <semaphore.h>
#include "utils.h"

pid_t create_alimentatore(shared_data *shm_data);
pid_t create_attivatore(shared_data *shm_data);
pid_t create_atomo(int *max_n_atomico, sem_t *sem, shared_data *shm_data);

void add_pid(pid_t pid, sem_t *sem, shared_data *shm_data);
void init_shared_memory_and_semaphore(const char* sem_name, sem_t** sem, int* shmid, shared_data** shm_data);
void connect_shared_memory_and_semaphore(const char* sem_name, sem_t** sem, int* shmid, shared_data** shm_data);
void cleanup_shared_memory_and_semaphore(const char* sem_name, sem_t** sem, int shmid, shared_data** shm_data);
void cleanup(sem_t** sem, shared_data** shm_data);
void create_timer(int signo, int tv_sec, int tv_nsec, int interval_tv_sec, int interval_tv_nsec, sem_t** sem, int* shmid, shared_data** shm_data);

int create_shared_memory();
shared_data* attach_shared_memory(int shmid);
void detach_shared_memory(shared_data* shm_data);
void remove_shared_memory(int shmid);

#endif
