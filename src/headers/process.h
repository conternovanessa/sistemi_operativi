
#ifndef PROCESS_H
#define PROCESS_H

#include <semaphore.h>

#include "utils.h"

pid_t create_attivatore();
pid_t create_atomo(int *max_n_atomico, sem_t *sem, shared_data *shm_data);
void add_pid(pid_t pid, sem_t *sem, shared_data *shm_data);

#endif