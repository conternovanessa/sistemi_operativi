#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>

#include "utils.h"

#define MAX_LINE_LENGTH 32



SimulationParameters leggiVariabili(const char *filename){
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

pid_t create_atomo(int *num_atomico, char *buffer, sem_t *sem, shared_data *shm_data) {
    // Fork atomo process
    pid_t c_pid = fork();
    if (c_pid == -1) {
        perror("fork to create atomo did not go well");
        exit(EXIT_FAILURE);
    }
    if (c_pid == 0) {
        // Child process: atomo
        sprintf(buffer, "%d", *num_atomico);
        char *atomo_args[] = {"atomo", buffer, NULL};
        if (execve("./atomo", atomo_args, NULL) == -1) {
            perror("execve failed for atomo");
            exit(EXIT_FAILURE);
        }
    } else {
        // Parent process: update shared memory
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

void print_shared_data(sem_t *sem, shared_data *shm_data) {
    print_line();
    // sem_wait(sem); // Lock the semaphore before accessing shared memory
    printf("\t SHARED DATA:\n");
    printf("attivazioni: %d\n", shm_data->attivazioni);
    printf("scissioni: %d\n", shm_data->scissioni);
    printf("free_energy: %d\n", shm_data->free_energy);
    printf("consumata: %d\n", shm_data->consumata);
    printf("scorie: %d\n", shm_data->scorie);
    printf("num_processes: %d\n", shm_data->num_processes);
    // sem_post(sem); // Unlock the semaphore after accessing shared memory
    print_line();
}


void print_line(){
    printf("--------------------------------------\n");
}

void printSimulationParameters(const SimulationParameters *params){
    print_line();
    printf("ENERGY_DEMAND: %d\n", params->energy_demand);
    printf("N_ATOM_INIT: %d\n", params->n_atom_init);
    printf("MAX_N_ATOMICO: %d\n", params->max_n_atomico);
    printf("MIN_N_ATOMICO: %d\n", params->min_n_atomico);
    printf("STEP_ATTIVATORE: %d\n", params->step_attivatore);
    printf("STEP_ALIMENTAZIONE: %d\n", params->step_alimentazione);
    printf("N_NUOVI_ATOMI: %d\n", params->n_nuovi_atomi);
    printf("SIM_DURATION: %d\n", params->sim_duration);
    printf("ENERGY_EXPLODE_THRESHOLD: %d\n", params->energy_explode_threshold);
    print_line();
}