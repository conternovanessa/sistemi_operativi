#pragma once
#ifndef UTILS_H
#define UTILS_H

#include <sys/types.h>
#include <semaphore.h>

// STRUCT DEFINITIONS
typedef struct
{
    int attivazioni;
    int scissioni;
    int free_energy;
    int consumata;
    int scorie;
    int num_processes;
    pid_t pid_array[100];
} shared_data;

typedef struct
{
    int energy_demand;
    int n_atom_init;
    int max_n_atomico;
    int min_n_atomico;
    int step_attivatore;
    int step_alimentazione;
    int n_nuovi_atomi;
    int sim_duration;
    int energy_explode_threshold;
} SimulationParameters;

//PROTOTYPE DEFINITION

SimulationParameters leggiVariabili(const char *filename);

pid_t create_attivatore();
pid_t create_atomo(int *num_atomico, char *buffer, sem_t *sem, shared_data *shm_data);

void printSimulationParameters(const SimulationParameters *params);
void add_pid(pid_t pid, sem_t *sem, shared_data *shm_data);
void print_shared_data(sem_t *sem, shared_data *shm_data);
void print_line();

#endif