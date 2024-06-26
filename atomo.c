#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <errno.h>
#include <string.h>

#define SHM_NAME "/my_shared_memory"
#define SEM_NAME "/my_semaphore"
#define MAX_LINE_LENGTH 256
#define MAX_PROCESSES 1000

typedef struct {
    int attivazioni;
    int scissioni;
    int free_energy;
    int consumata;
    int scorie;
    int num_processes;
    pid_t pid_array[MAX_PROCESSES];
} shared_data;
// Dichiarazioni delle funzioni

void scissione(int numero_atomico){
    int n1 = (rand() % (numero_atomico - 2)) + 2;
    int n2 = numero_atomico- n1;
    int new_energy = n1*n2 - ((n1 > n2) ? n1 : n2);
    printf("n1: %d\n", n1);
    printf("n2: %d\n", n2);

}

/// ciao
int main(int argc, char *argv[]) {
    // Converti gli argomenti in interi
    pid_t pid = atoi(argv[1]);
    int min_n_atomico = atoi(argv[2]);
    int numero_atomico = atoi(argv[3]);
    printf("dentro atomo %d\n",pid);
    scissione(numero_atomico);


    return 0;
}
