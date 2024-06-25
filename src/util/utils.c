#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

#define MAX_LINE_LENGTH 32

SimulationParameters leggiVariabili(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Errore nell'apertura del file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    SimulationParameters params = {0};
    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), file))
    {
        char *variabile = strtok(line, "=");
        char *valore = strtok(NULL, "=");

        if (variabile != NULL && valore != NULL)
        {
            valore[strcspn(valore, "\n")] = 0;

            if (strcmp(variabile, "ENERGY_DEMAND") == 0)
            {
                params.energy_demand = atoi(valore);
            }
            else if (strcmp(variabile, "N_ATOM_INIT") == 0)
            {
                params.n_atom_init = atoi(valore);
            }
            else if (strcmp(variabile, "MAX_N_ATOMICO") == 0)
            {
                params.max_n_atomico = atoi(valore);
            }
            else if (strcmp(variabile, "MIN_N_ATOMICO") == 0)
            {
                params.min_n_atomico = atoi(valore);
            }
            else if (strcmp(variabile, "STEP_ATTIVATORE") == 0)
            {
                params.step_attivatore = atoi(valore);
            }
            else if (strcmp(variabile, "STEP_ALIMENTAZIONE") == 0)
            {
                params.step_alimentazione = atoi(valore);
            }
            else if (strcmp(variabile, "N_NUOVI_ATOMI") == 0)
            {
                params.n_nuovi_atomi = atoi(valore);
            }
            else if (strcmp(variabile, "SIM_DURATION") == 0)
            {
                params.sim_duration = atoi(valore);
            }
            else if (strcmp(variabile, "ENERGY_EXPLODE_THRESHOLD") == 0)
            {
                params.energy_explode_threshold = atoi(valore);
            }
        }
    }

    fclose(file);
    return params;
}

void printSimulationParameters(const SimulationParameters *params)
{
    char arr[30];
    for (int i = 0; i < 30; ++i) {
        arr[i] = '-';
    }
    printf("%s\n", arr);
    printf("ENERGY_DEMAND: %d\n", params->energy_demand);
    printf("N_ATOM_INIT: %d\n", params->n_atom_init);
    printf("MAX_N_ATOMICO: %d\n", params->max_n_atomico);
    printf("MIN_N_ATOMICO: %d\n", params->min_n_atomico);
    printf("STEP_ATTIVATORE: %d\n", params->step_attivatore);
    printf("STEP_ALIMENTAZIONE: %d\n", params->step_alimentazione);
    printf("N_NUOVI_ATOMI: %d\n", params->n_nuovi_atomi);
    printf("SIM_DURATION: %d\n", params->sim_duration);
    printf("ENERGY_EXPLODE_THRESHOLD: %d\n", params->energy_explode_threshold);
    printf("%s\n", arr);
}