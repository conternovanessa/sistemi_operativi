
#ifndef PROCESS_H
#define PROCESS_H

#include <semaphore.h>

#include "utils.h"

pid_t create_attivatore();
pid_t create_atomo(int *max_n_atomico, sem_t *sem, shared_data *shm_data);

void add_pid(pid_t pid, sem_t *sem, shared_data *shm_data);
void init_shared_memory_and_semaphore(const char* sem_name, sem_t** sem, const char* shared_name, shared_data** shm_data);
void cleanup_shared_memory_and_semaphore(const char* sem_name, sem_t** sem, const char* shared_name, shared_data** shm_data);
void create_timer(timer_callback callback, int tv_sec, int tv_nsec, int interval_tv_sec, int interval_tv_nsec);

#endif