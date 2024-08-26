#pragma once
#ifndef UTILS_H
#define UTILS_H

#include <sys/types.h>

#define SHARED_MEM_NAME "/shared_mem"
#define SEMAPHORE_NAME "/semaphore"
#define MAX_PROCESSES 1000

// STRUCT DEFINITIONS
typedef struct
{
    int attivazioni;
    int scissioni;
    int free_energy;
    int consumata;
    int scorie;
    int num_processes;
    pid_t pid_array[MAX_PROCESSES];
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

typedef void (*timer_callback)();

#endif