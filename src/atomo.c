#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "util/utils.h"

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
    int minimo = params.min_n_atomico;
    scissione(num_atomico);

    printf("Received num_atomico: %d \n", num_atomico);
    exit(EXIT_SUCCESS);
}
