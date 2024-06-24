#include <stdio.h>
#include <stdlib.h>



#define SHM_NAME "/my_shared_memory"
#define SEM_NAME "/my_semaphore"
#define MAX_LINE_LENGTH 256
#define MAX_PROCESSES 1000

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <STEP_ALIMENTAZIONE> <N_NUOVI_ATOMI>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int STEP_ALIMENTAZIONE = atoi(argv[1]);
    int N_NUOVI_ATOMI = atoi(argv[2]);

    printf("Dentro a alimentatore. STEP_ALIMENTAZIONE: %d, N_NUOVI_ATOMI: %d\n", STEP_ALIMENTAZIONE, N_NUOVI_ATOMI);

    // Il resto del codice per l'alimentatore andrà qui

    return 0;
}