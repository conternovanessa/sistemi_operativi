
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>

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

shared_data *shared_memory;
sem_t *sem;
int SIM_DURATION;

void create_shared_memory_and_semaphore() {
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shm_fd, sizeof(shared_data)) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    shared_memory = mmap(0, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    memset(shared_memory, 0, sizeof(shared_data));

    sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    close(shm_fd);
}

void leggiVariabili(const char* filename, int *energy_demand, int *n_atom_init, int *n_atom_max, int *min_n_atomico, int *step_attivatore, int *step_alimentazione, int *n_nuovi_atomi, int *sim_duration, int *energy_explode_threshold) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Errore nell'apertura del file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        char* variabile = strtok(line, "=");
        char* valore = strtok(NULL, "=");

        if (variabile != NULL && valore != NULL) {
            valore[strcspn(valore, "\n")] = 0;

            if (strcmp(variabile, "ENERGY_DEMAND") == 0) {
                *energy_demand = atoi(valore);
            } else if (strcmp(variabile, "N_ATOM_INIT") == 0) {
                *n_atom_init = atoi(valore);
            } else if (strcmp(variabile, "N_ATOM_MAX") == 0) {
                *n_atom_max = atoi(valore);
            } else if (strcmp(variabile, "MIN_N_ATOMICO") == 0) {
                *min_n_atomico = atoi(valore);
            } else if (strcmp(variabile, "STEP_ATTIVATORE") == 0) {
                *step_attivatore = atoi(valore);
            } else if (strcmp(variabile, "STEP_ALIMENTAZIONE") == 0) {
                *step_alimentazione = atoi(valore);
            } else if (strcmp(variabile, "N_NUOVI_ATOMI") == 0) {
                *n_nuovi_atomi = atoi(valore);
            } else if (strcmp(variabile, "SIM_DURATION") == 0) {
                *sim_duration = atoi(valore);
            } else if (strcmp(variabile, "ENERGY_EXPLODE_THRESHOLD") == 0) {
                *energy_explode_threshold = atoi(valore);
            }
        }
    }

    fclose(file);
}

void add_pid_to_shared_memory(pid_t pid) {
    sem_wait(sem);
    if (shared_memory->num_processes < MAX_PROCESSES) {
        shared_memory->pid_array[shared_memory->num_processes++] = pid;
    }
    sem_post(sem);
}



pid_t pid_attivatore, pid_alimentatore;

void timeout_handler(int signum) {
    printf("Abbiamo raggiunto %d secondi\n", SIM_DURATION);

    sem_wait(sem);
    for (int i = 0; i < shared_memory->num_processes; i++) {
        kill(shared_memory->pid_array[i], SIGTERM);
    }
    sem_post(sem);

    // Terminazione di attivatore e alimentatore
    kill(pid_attivatore, SIGTERM);
    kill(pid_alimentatore, SIGTERM);

    // Attendi che tutti i processi terminino
    while (wait(NULL) > 0);

    if (munmap(shared_memory, sizeof(shared_data)) == -1) {
        perror("munmap");
    }
    if (shm_unlink(SHM_NAME) == -1) {
        perror("shm_unlink");
    }
    if (sem_close(sem) == -1) {
        perror("sem_close");
    }
    if (sem_unlink(SEM_NAME) == -1) {
        perror("sem_unlink");
    }

    exit(0);
}

int main(int argc, char *argv[]) {
    int ENERGY_DEMAND, N_ATOM_INIT, N_ATOM_MAX, MIN_N_ATOMICO, STEP_ATTIVATORE;
    int STEP_ALIMENTAZIONE, N_NUOVI_ATOMI, ENERGY_EXPLODE_THRESHOLD;

    leggiVariabili("variabili.txt", &ENERGY_DEMAND, &N_ATOM_INIT, &N_ATOM_MAX, &MIN_N_ATOMICO,
                   &STEP_ATTIVATORE, &STEP_ALIMENTAZIONE, &N_NUOVI_ATOMI, &SIM_DURATION, &ENERGY_EXPLODE_THRESHOLD);

    srand(time(NULL));

    create_shared_memory_and_semaphore();

    signal(SIGALRM, timeout_handler);
    alarm(SIM_DURATION);

    // Esecuzione di attivatore.c
    pid_attivatore = fork();
    if (pid_attivatore == -1) {
        perror("fork attivatore");
        exit(EXIT_FAILURE);
    } else if (pid_attivatore == 0) {
        char step_attivatore_str[16];
        snprintf(step_attivatore_str, sizeof(step_attivatore_str), "%d", STEP_ATTIVATORE);
        execl("./attivatore", "attivatore", step_attivatore_str, (char *) NULL);
        perror("execl attivatore");
        exit(EXIT_FAILURE);
    }

    // Esecuzione di alimentatore.c
    pid_alimentatore = fork();
    if (pid_alimentatore == -1) {
        perror("fork alimentatore");
        exit(EXIT_FAILURE);
    } else if (pid_alimentatore == 0) {
        char step_alimentazione_str[16], n_nuovi_atomi_str[16];
        snprintf(step_alimentazione_str, sizeof(step_alimentazione_str), "%d", STEP_ALIMENTAZIONE);
        snprintf(n_nuovi_atomi_str, sizeof(n_nuovi_atomi_str), "%d", N_NUOVI_ATOMI);
        execl("./alimentatore", "alimentatore", step_alimentazione_str, n_nuovi_atomi_str, (char *) NULL);
        perror("execl alimentatore");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < N_ATOM_INIT; i++) {
        int numero_atomico = (rand() % N_ATOM_MAX) + 1;

        pid_t pid = fork();

        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            char pid_str[16], numero_atomico_str[16], min_n_atomico_str[16];
            snprintf(pid_str, sizeof(pid_str), "%d", getpid());
            snprintf(numero_atomico_str, sizeof(numero_atomico_str), "%d", numero_atomico);
            snprintf(min_n_atomico_str, sizeof(min_n_atomico_str), "%d", MIN_N_ATOMICO);

            execl("./atomo", "atomo", pid_str, min_n_atomico_str, numero_atomico_str, (char *) NULL);
            perror("execl");
            exit(EXIT_FAILURE);
        } else {
            add_pid_to_shared_memory(pid);
        }
    }

    printf("PID dei processi atomo creati:\n");
    for (int i = 0; i < shared_memory->num_processes; i++) {
        printf("%d\n", shared_memory->pid_array[i]);
    }

    while (wait(NULL) > 0);

    printf("Tutti i processi sono terminati prima di raggiungere %d secondi\n", SIM_DURATION);

    if (munmap(shared_memory, sizeof(shared_data)) == -1) {
        perror("munmap");
    }
    if (shm_unlink(SHM_NAME) == -1) {
        perror("shm_unlink");
    }
    if (sem_close(sem) == -1) {
        perror("sem_close");
    }
    if (sem_unlink(SEM_NAME) == -1) {
        perror("sem_unlink");
    }

    return 0;
}
