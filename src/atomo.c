#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <semaphore.h>
#include "util/utils.h"

#define SHARED_MEM_NAME "/shared_mem"
#define SEMAPHORE_NAME "/semaphore"

shared_data *shm_data;
sem_t *sem;
pid_t a_pid,c_pid;

void scissione(int numero_atomico){
    int n1 = (rand() % (numero_atomico - 2)) + 2;
    int n2 = numero_atomico- n1;
    int new_energy = n1*n2 - ((n1 > n2) ? n1 : n2);
    printf("n1: %d\n", n1);
    printf("n2: %d\n", n2);

}

int main(int argc, char *argv[]){
    if (argc < 2){
        perror("Necessary at least two inputs");
        exit(EXIT_FAILURE);
    }

    int num_atomico = atoi(argv[1]);

    srand(time(NULL));
    const char* filename = "variabili.txt";
    SimulationParameters params = leggiVariabili(filename);
    print_line();
    printf("PARAMETERS OBTAINED FROM THE FILE: \n");
    printSimulationParameters(&params);

    int minimo = params.min_n_atomico;


    scissione(num_atomico);

    printf("Received num_atomico: %d \n", num_atomico);
    exit(EXIT_SUCCESS);
}
