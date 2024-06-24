#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <string.h>
#include <time.h>

#define SHM_NAME "/my_shared_memory"
#define SEM_NAME "/my_semaphore"
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

shared_data *shared_memory;
sem_t *sem;

void cleanup() {
    if (munmap(shared_memory, sizeof(shared_data)) == -1) {
        perror("munmap");
    }
    if (sem_close(sem) == -1) {
        perror("sem_close");
    }
}

void send_signal_to_random_child(int pids_size, pid_t *pids) {
    if (pids_size > 0) {
        size_t random_index = rand() % pids_size;  // Select a random index
        pid_t random_pid = pids[random_index];  // Get the PID at that index
        kill(random_pid, SIGUSR1);  // Send SIGUSR1 to the selected process
        printf("Sent SIGUSR1 to process with PID: %d\n", random_pid);
    }
}



int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <PID>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int STEP_ATTIVATORE = atoi(argv[1]);

    printf("Dentro a attivatore. STEP_ATTIVATORE: %d\n", STEP_ATTIVATORE);

    // Apertura della memoria condivisa
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    // Mappatura della memoria condivisa
    shared_memory = mmap(NULL, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // Apertura del semaforo
    sem = sem_open(SEM_NAME, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    // Impostazione del gestore del segnale SIGUSR1
    signal(SIGUSR1, send_signal_to_random_child);

    // Invia SIGUSR1 a un processo atomo
    sem_wait(sem);
    if (shared_memory->num_processes > 0) {
        int index = rand() % shared_memory->num_processes;
        pid_t pid_atomo = shared_memory->pid_array[index];

        // Invia SIGUSR1 al processo atomo selezionato per la scissione
        kill(pid_atomo, SIGUSR1);
        printf("Attivatore: Inviato segnale SIGUSR1 a processo atomo con PID %d per la scissione\n", pid_atomo);
    } else {
        fprintf(stderr, "Attivatore: Nessun processo atomo disponibile\n");
    }
    sem_post(sem);

    cleanup();

    return 0;
}
