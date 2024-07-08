//
// Created by vconterno on 08/07/24.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <time.h>

#include "headers/utils.h"
#include "headers/io.h"
#include "headers/process.h"

shared_data *shm_data;
sem_t *sem;

int main(int argc, char *argv[]) {
    const char* filename = "variabili.txt";
    SimulationParameters params = leggiVariabili(filename);

    int tempo = params.step_alimentazione;
    int nuovi = params.n_nuovi_atomi;

    struct timespec ts;
    ts.tv_sec = tempo;     // Secondi
    ts.tv_nsec = 0;        // Nanosecondi

    while (1) {
        printf("Il timer durerà %d secondi\n", tempo);
        nanosleep(&ts, NULL);  // Attende per l'intervallo specificato

        //chiamare create_atomo

    }

    return 0;
}