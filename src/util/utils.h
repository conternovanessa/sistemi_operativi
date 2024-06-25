#pragma once
#ifndef UTILS_H
#define UTILS_H

#include <sys/types.h>

typedef struct
{
    int attivazioni;
    int scissioni;
    int free_energy;
    int consumata;
    int scorie;
    int num_processes;
    pid_t *pid_array;
} shared_data;

typedef struct
{
    int energy_demand;
    int n_atom_init;
    int n_atom_max;
    int min_n_atomico;
    int step_attivatore;
    int step_alimentazione;
    int n_nuovi_atomi;
    int sim_duration;
    int energy_explode_threshold;
} SimulationParameters;

SimulationParameters leggiVariabili(const char *filename);
void printSimulationParameters(
    const SimulationParameters *params);

#endif