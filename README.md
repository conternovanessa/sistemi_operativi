# sistemi_operativi
## Installation instruction on UBUNTU

```
sudo apt update && sudo apt upgrade -y
sudo apt install gcc g++ cmake -y
```

## How to build it

Open a terminal on current folder with terminal and run:#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <time.h>

#include "headers/utils.h"
#include "headers/io.h"
#include "headers/process.h"

// Global variables for shared memory and semaphore
shared_data *shm_data;
sem_t *sem;
pid_t a_pid,c_pid;

void timer_handler(int sig) {
    print_shared_data(shm_data);
    // Set the alarm again for the next second
    alarm(1);
}


void terminate_processes(pid_t a_pid, pid_t c_pid) {
    // Terminate attivatore process
    if (kill(a_pid, SIGTERM) == -1) {
        perror("Error terminating attivatore");
    }
    
    for(int i = 0; i < shm_data->num_processes; i++){
        if (kill(shm_data->pid_array[i], SIGTERM) == -1) {#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <time.h>

#include "headers/utils.h"
#include "headers/io.h"
#include "headers/process.h"

// Global variables for shared memory and semaphore
shared_data *shm_data;
sem_t *sem;
pid_t a_pid,c_pid;

void timer_handler(int sig) {
    print_shared_data(shm_data);
    // Set the alarm again for the next second
    alarm(1);
}


void terminate_processes(pid_t a_pid, pid_t c_pid) {
    // Terminate attivatore process
    if (kill(a_pid, SIGTERM) == -1) {
        perror("Error terminating attivatore");
    }
    
    for(int i = 0; i < shm_data->num_processes; i++){
        if (kill(shm_data->pid_array[i], SIGTERM) == -1) {
            perror("Error terminating atomo");
        }
    }
}

int main(int argc, char *argv[]) {
    const char* filename = "variabili.txt";
    SimulationParameters params = leggiVariabili(filename);
    print_line();
    printf("PARAMETERS OBTAINED FROM THE FILE: \n");
    printSimulationParameters(&params);
    
    // Initialize shared memory and semaphore
    init_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);
    print_shared_data(shm_data);
    srand(time(NULL));
    struct sigaction sa;

    // Set up the signal handler
    sa.sa_handler = timer_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("sigaction failed");
        exit(EXIT_FAILURE);
    }#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <time.h>

#include "headers/utils.h"
#include "headers/io.h"
#include "headers/process.h"

// Global variables for shared memory and semaphore
shared_data *shm_data;
sem_t *sem;
pid_t a_pid,c_pid;

void timer_handler(int sig) {
    print_shared_data(shm_data);
    // Set the alarm again for the next second
    alarm(1);
}


void terminate_processes(pid_t a_pid, pid_t c_pid) {
    // Terminate attivatore process
    if (kill(a_pid, SIGTERM) == -1) {
        perror("Error terminating attivatore");
    }
    
    for(int i = 0; i < shm_data->num_processes; i++){
        if (kill(shm_data->pid_array[i], SIGTERM) == -1) {
            perror("Error terminating atomo");
        }
    }
}

int main(int argc, char *argv[]) {
    const char* filename = "variabili.txt";
    SimulationParameters params = leggiVariabili(filename);
    print_line();
    printf("PARAMETERS OBTAINED FROM THE FILE: \n");
    printSimulationParameters(&params);
    
    // Initialize shared memory and semaphore
    init_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);
    print_shared_data(shm_data);
    srand(time(NULL));
    struct sigaction sa;

    // Set up the signal handler
    sa.sa_handler = timer_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("sigaction failed");
        exit(EXIT_FAILURE);
    }

    // Set the initial alarm for 1 second
    if (alarm(1) == -1) {
        perror("alarm failed");
        exit(EXIT_FAILURE);
    }

    // Fork attivatore process
    a_pid = create_attivatore();



    // Fork atomo process
    for (int i = 0; i < params.n_atom_init; i++) {
        c_pid = create_atomo(&params.max_n_atomico, sem, shm_data);
    }

    // Wait for a certain amount of time
    srand(time(NULL));
    int count = 0;
    
    while(count < 8){
        pause(); 
        count++;
    }

    // Print shared data after sleep
    print_shared_data(shm_data);

    printf("Kill all the processes\n");
    // Terminate child processes
    terminate_processes(a_pid, c_pid);

    // Wait for child processes to terminate
    int status;
    waitpid(a_pid, &status, 0);
    waitpid(c_pid, &status, 0);
    printf("All processes terminated\n");
    print_line();

    // Cleanup shared memory and semaphore
    cleanup_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);

    exit(EXIT_SUCCESS);
}


    // Set the initial alarm for 1 second
    if (alarm(1) == -1) {
        perror("alarm failed");
        exit(EXIT_FAILURE);
    }

    // Fork attivatore process
    a_pid = create_attivatore();



    // Fork atomo process
    for (int i = 0; i < params.n_atom_init; i++) {
        c_pid = create_atomo(&params.max_n_atomico, sem, shm_data);
    }

    // Wait for a certain amount of time
    srand(time(NULL));
    int count = 0;
    
    while(count < 8){
        pause(); 
        count++;
    }

    // Print shared data after sleep
    print_shared_data(shm_data);

    printf("Kill all the processes\n");
    // Terminate child processes
    terminate_processes(a_pid, c_pid);

    // Wait for child processes to terminate
    int status;
    waitpid(a_pid, &status, 0);
    waitpid(c_pid, &status, 0);
    printf("All processes terminated\n");
    print_line();

    // Cleanup shared memory and semaphore
    cleanup_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);

    exit(EXIT_SUCCESS);
}

            perror("Error terminating atomo");
        }
    }
}

int main(int argc, char *argv[]) {
    const char* filename = "variabili.txt";
    SimulationParameters params = leggiVariabili(filename);
    print_line();
    printf("PARAMETERS OBTAINED FROM THE FILE: \n");
    printSimulationParameters(&params);
    
    // Initialize shared memory and semaphore
    init_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);
    print_shared_data(shm_data);
    srand(time(NULL));
    struct sigaction sa;

    // Set up the signal handler
    sa.sa_handler = timer_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("sigaction failed");
        exit(EXIT_FAILURE);
    }

    // Set the initial alarm for 1 second
    if (alarm(1) == -1) {
        perror("alarm failed");
        exit(EXIT_FAILURE);
    }

    // Fork attivatore process
    a_pid = create_attivatore();



    // Fork atomo process
    for (int i = 0; i < params.n_atom_init; i++) {
        c_pid = create_atomo(&params.max_n_atomico, sem, shm_data);
    }

    // Wait for a certain amount of time
    srand(time(NULL));
    int count = 0;
    
    while(count < 8){
        pause(); 
        count++;
    }

    // Print shared data after sleep
    print_shared_data(shm_data);

    printf("Kill all the processes\n");
    // Terminate child processes
    terminate_processes(a_pid, c_pid);

    // Wait for child processes to terminate
    int status;
    waitpid(a_pid, &status, 0);
    waitpid(c_pid, &status, 0);
    printf("All processes terminated\n");
    print_line();

    // Cleanup shared memory and semaphore
    cleanup_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);

    exit(EXIT_SUCCESS);
}


```
mkdir build
cp variabili.txt build
cd build
cmake ..#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <time.h>

#include "headers/utils.h"
#include "headers/io.h"
#include "headers/process.h"

// Global variables for shared memory and semaphore
shared_data *shm_data;
sem_t *sem;
pid_t a_pid,c_pid;

void timer_handler(int sig) {
    print_shared_data(shm_data);#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <time.h>

#include "headers/utils.h"
#include "headers/io.h"
#include "headers/process.h"

// Global variables for shared memory and semaphore
shared_data *shm_data;
sem_t *sem;
pid_t a_pid,c_pid;

void timer_handler(int sig) {
    print_shared_data(shm_data);
    // Set the alarm again for the next second
    alarm(1);
}


void terminate_processes(pid_t a_pid, pid_t c_pid) {
    // Terminate attivatore process
    if (kill(a_pid, SIGTERM) == -1) {
        perror("Error terminating attivatore");
    }
    
    for(int i = 0; i < shm_data->num_processes; i++){
        if (kill(shm_data->pid_array[i], SIGTERM) == -1) {
            perror("Error terminating atomo");
        }
    }
}

int main(int argc, char *argv[]) {
    const char* filename = "variabili.txt";
    SimulationParameters params = leggiVariabili(filename);
    print_line();
    printf("PARAMETERS OBTAINED FROM THE FILE: \n");
    printSimulationParameters(&params);
    
    // Initialize shared memory and semaphore
    init_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);
    print_shared_data(shm_data);
    srand(time(NULL));
    struct sigaction sa;

    // Set up the signal handler
    sa.sa_handler = timer_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("sigaction failed");
        exit(EXIT_FAILURE);
    }

    // Set the initial alarm for 1 second
    if (alarm(1) == -1) {
        perror("alarm failed");
        exit(EXIT_FAILURE);
    }

    // Fork attivatore process
    a_pid = create_attivatore();



    // Fork atomo process
    for (int i = 0; i < params.n_atom_init; i++) {
        c_pid = create_atomo(&params.max_n_atomico, sem, shm_data);
    }

    // Wait for a certain amount of time
    srand(time(NULL));
    int count = 0;
    
    while(count < 8){
        pause(); 
        count++;
    }

    // Print shared data after sleep
    print_shared_data(shm_data);

    printf("Kill all the processes\n");
    // Terminate child processes
    terminate_processes(a_pid, c_pid);

    // Wait for child processes to terminate
    int status;
    waitpid(a_pid, &status, 0);#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <time.h>

#include "headers/utils.h"
#include "headers/io.h"
#include "headers/process.h"

// Global variables for shared memory and semaphore
shared_data *shm_data;
sem_t *sem;
pid_t a_pid,c_pid;

void timer_handler(int sig) {
    print_shared_data(shm_data);
    // Set the alarm again for the next second
    alarm(1);
}


void terminate_processes(pid_t a_pid, pid_t c_pid) {
    // Terminate attivatore process
    if (kill(a_pid, SIGTERM) == -1) {
        perror("Error terminating attivatore");
    }
    
    for(int i = 0; i < shm_data->num_processes; i++){
        if (kill(shm_data->pid_array[i], SIGTERM) == -1) {
            perror("Error terminating atomo");
        }
    }
}

int main(int argc, char *argv[]) {
    const char* filename = "variabili.txt";
    SimulationParameters params = leggiVariabili(filename);
    print_line();
    printf("PARAMETERS OBTAINED FROM THE FILE: \n");
    printSimulationParameters(&params);
    
    // Initialize shared memory and semaphore
    init_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);
    print_shared_data(shm_data);
    srand(time(NULL));
    struct sigaction sa;

    // Set up the signal handler
    sa.sa_handler = timer_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("sigaction failed");
        exit(EXIT_FAILURE);
    }

    // Set the initial alarm for 1 second
    if (alarm(1) == -1) {
        perror("alarm failed");
        exit(EXIT_FAILURE);
    }

    // Fork attivatore process
    a_pid = create_attivatore();



    // Fork atomo process
    for (int i = 0; i < params.n_atom_init; i++) {
        c_pid = create_atomo(&params.max_n_atomico, sem, shm_data);
    }

    // Wait for a certain amount of time
    srand(time(NULL));
    int count = 0;
    
    while(count < 8){
        pause(); 
        count++;
    }
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <time.h>

#include "headers/utils.h"
#include "headers/io.h"
#include "headers/process.h"

// Global variables for shared memory and semaphore
shared_data *shm_data;
sem_t *sem;
pid_t a_pid,c_pid;

void timer_handler(int sig) {
    print_shared_data(shm_data);
    // Set the alarm again for the next second
    alarm(1);
}


void terminate_processes(pid_t a_pid, pid_t c_pid) {
    // Terminate attivatore process
    if (kill(a_pid, SIGTERM) == -1) {
        perror("Error terminating attivatore");
    }
    
    for(int i = 0; i < shm_data->num_processes; i++){
        if (kill(shm_data->pid_array[i], SIGTERM) == -1) {
            perror("Error terminating atomo");
        }
    }
}

int main(int argc, char *argv[]) {
    const char* filename = "variabili.txt";
    SimulationParameters params = leggiVariabili(filename);
    print_line();
    printf("PARAMETERS OBTAINED FROM THE FILE: \n");
    printSimulationParameters(&params);
    
    // Initialize shared memory and semaphore
    init_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);
    print_shared_data(shm_data);
    srand(time(NULL));
    struct sigaction sa;

    // Set up the signal handler
    sa.sa_handler = timer_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("sigaction failed");
        exit(EXIT_FAILURE);
    }

    // Set the initial alarm for 1 second
    if (alarm(1) == -1) {
        perror("alarm failed");
        exit(EXIT_FAILURE);
    }

    // Fork attivatore process
    a_pid = create_attivatore();



    // Fork atomo process
    for (int i = 0; i < params.n_atom_init; i++) {
        c_pid = create_atomo(&params.max_n_atomico, sem, shm_data);
    }

    // Wait for a certain amount of time
    srand(time(NULL));
    int count = 0;
    
    while(count < 8){
        pause(); 
        count++;
    }

    // Print shared data after sleep
    print_shared_data(shm_data);

    printf("Kill all the processes\n");
    // Terminate child processes
    terminate_processes(a_pid, c_pid);

    // Wait for child processes to terminate
    int status;
    waitpid(a_pid, &status, 0);
    waitpid(c_pid, &status, 0);
    printf("All processes terminated\n");
    print_line();

    // Cleanup shared memory and semaphore
    cleanup_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);

    exit(EXIT_SUCCESS);
}

    // Print shared data after sleep
    print_shared_data(shm_data);

    printf("Kill all the processes\n");
    // Terminate child processes
    terminate_processes(a_pid, c_pid);

    // Wait for child processes to terminate
    int status;
    waitpid(a_pid, &status, 0);
    waitpid(c_pid, &status, 0);
    printf("All processes terminated\n");
    print_line();

    // Cleanup shared memory and semaphore
    cleanup_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);

    exit(EXIT_SUCCESS);
}

    waitpid(c_pid, &status, 0);
    printf("All processes terminated\n");
    print_line();

    // Cleanup shared memory and semaphore
    cleanup_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);

    exit(EXIT_SUCCESS);
}

    // Set the alarm again for the next second
    alarm(1);
}


void terminate_processes(pid_t a_pid, pid_t c_pid) {
    // Terminate attivatore process
    if (kill(a_pid, SIGTERM) == -1) {
        perror("Error terminating attivatore");
    }
    
    for(int i = 0; i < shm_data->num_processes; i++){
        if (kill(shm_data->pid_array[i], SIGTERM) == -1) {
            perror("Error terminating atomo");
        }#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <time.h>

#include "headers/utils.h"
#include "headers/io.h"
#include "headers/process.h"

// Global variables for shared memory and semaphore
shared_data *shm_data;
sem_t *sem;
pid_t a_pid,c_pid;

void timer_handler(int sig) {
    print_shared_data(shm_data);
    // Set the alarm again for the next second
    alarm(1);
}


void terminate_processes(pid_t a_pid, pid_t c_pid) {
    // Terminate attivatore process
    if (kill(a_pid, SIGTERM) == -1) {
        perror("Error terminating attivatore");
    }
    
    for(int i = 0; i < shm_data->num_processes; i++){
        if (kill(shm_data->pid_array[i], SIGTERM) == -1) {
            perror("Error terminating atomo");
        }
    }
}

int main(int argc, char *argv[]) {
    const char* filename = "variabili.txt";
    SimulationParameters params = leggiVariabili(filename);
    print_line();
    printf("PARAMETERS OBTAINED FROM THE FILE: \n");
    printSimulationParameters(&params);
    
    // Initialize shared memory and semaphore
    init_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);
    print_shared_data(shm_data);
    srand(time(NULL));
    struct sigaction sa;

    // Set up the signal handler
    sa.sa_handler = timer_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("sigaction failed");
        exit(EXIT_FAILURE);
    }

    // Set the initial alarm for 1 second
    if (alarm(1) == -1) {
        perror("alarm failed");
        exit(EXIT_FAILURE);
    }

    // Fork attivatore process
    a_pid = create_attivatore();



    // Fork atomo process
    for (int i = 0; i < params.n_atom_init; i++) {
        c_pid = create_atomo(&params.max_n_atomico, sem, shm_data);
    }

    // Wait for a certain amount of time
    srand(time(NULL));
    int count = 0;
    
    while(count < 8){
        pause(); 
        count++;
    }

    // Print shared data after sleep
    print_shared_data(shm_data);

    printf("Kill all the processes\n");
    // Terminate child processes
    terminate_processes(a_pid, c_pid);

    // Wait for child processes to terminate
    int status;
    waitpid(a_pid, &status, 0);
    waitpid(c_pid, &status, 0);
    printf("All processes terminated\n");
    print_line();

    // Cleanup shared memory and semaphore
    cleanup_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);

    exit(EXIT_SUCCESS);
}

    }
}

int main(int argc, char *argv[]) {
    const char* filename = "variabili.txt";
    SimulationParameters params = leggiVariabili(filename);
    print_line();#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <time.h>

#include "headers/utils.h"
#include "headers/io.h"
#include "headers/process.h"

// Global variables for shared memory and semaphore
shared_data *shm_data;
sem_t *sem;
pid_t a_pid,c_pid;

void timer_handler(int sig) {
    print_shared_data(shm_data);
    // Set the alarm again for the next second
    alarm(1);
}


void terminate_processes(pid_t a_pid, pid_t c_pid) {
    // Terminate attivatore process
    if (kill(a_pid, SIGTERM) == -1) {
        perror("Error terminating attivatore");
    }
    
    for(int i = 0; i < shm_data->num_processes; i++){
        if (kill(shm_data->pid_array[i], SIGTERM) == -1) {
            perror("Error terminating atomo");
        }
    }
}

int main(int argc, char *argv[]) {
    const char* filename = "variabili.txt";
    SimulationParameters params = leggiVariabili(filename);
    print_line();
    printf("PARAMETERS OBTAINED FROM THE FILE: \n");
    printSimulationParameters(&params);
    
    // Initialize shared memory and semaphore
    init_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);
    print_shared_data(shm_data);
    srand(time(NULL));
    struct sigaction sa;

    // Set up the signal handler
    sa.sa_handler = timer_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("sigaction failed");
        exit(EXIT_FAILURE);
    }

    // Set the initial alarm for 1 second
    if (alarm(1) == -1) {
        perror("alarm failed");
        exit(EXIT_FAILURE);
    }

    // Fork attivatore process
    a_pid = create_attivatore();



    // Fork atomo process
    for (int i = 0; i < params.n_atom_init; i++) {
        c_pid = create_atomo(&params.max_n_atomico, sem, shm_data);
    }

    // Wait for a certain amount of time
    srand(time(NULL));
    int count = 0;
    
    while(count < 8){
        pause(); 
        count++;
    }

    // Print shared data after sleep
    print_shared_data(shm_data);

    printf("Kill all the processes\n");
    // Terminate child processes
    terminate_processes(a_pid, c_pid);

    // Wait for child processes to terminate
    int status;
    waitpid(a_pid, &status, 0);
    waitpid(c_pid, &status, 0);
    printf("All processes terminated\n");
    print_line();

    // Cleanup shared memory and semaphore
    cleanup_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);

    exit(EXIT_SUCCESS);
}

    printf("PARAMETERS OBTAINED FROM THE FILE: \n");
    printSimulationParameters(&params);
    
    // Initialize shared memory and semaphore
    init_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);
    print_shared_data(shm_data);
    srand(time(NULL));
    struct sigaction sa;

    // Set up the signal handler
    sa.sa_handler = timer_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("sigaction failed");
        exit(EXIT_FAILURE);
    }

    // Set the initial alarm for 1 second
    if (alarm(1) == -1) {
        perror("alarm failed");
        exit(EXIT_FAILURE);
    }

    // Fork attivatore process
    a_pid = create_attivatore();



    // Fork atomo process
    for (int i = 0; i < params.n_atom_init; i++) {
        c_pid = create_atomo(&params.max_n_atomico, sem, shm_data);
    }

    // Wait for a certain amount of time
    srand(time(NULL));
    int count = 0;
    
    while(count < 8){
        pause(); 
        count++;
    }

    // Print shared data after sleep
    print_shared_data(shm_data);

    printf("Kill all the processes\n");
    // Terminate child processes
    terminate_processes(a_pid, c_pid);

    // Wait for child processes to terminate
    int status;
    waitpid(a_pid, &status, 0);
    waitpid(c_pid, &status, 0);
    printf("All processes terminated\n");
    print_line();

    // Cleanup shared memory and semaphore
    cleanup_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);

    exit(EXIT_SUCCESS);
}

make
```

## Checklist:

- [ ] processo master
    - [ ] terminatore
      - [ ] Timeout  --> raggiungimento di SIM_DURATION
      - [ ] Explode  --> FREE_ENERGY > ENERGU_EXPLODE_THRESHOLD
      - [ ] Blackout --> prelievo di quantità superiore di quella possibile da prelevare
      - [ ] meltdown  --> fallimento di una fork
    - [x] far partire N_ATOM_INIT processi *atomo* in cui si scelga il numero atomico da 0 a MAX_N_ATOMICO
    - [x] far partire *attivatore*
    - [ ] far partire *alimentatore*
    - [x] STAMPA stato corrente ogni secondo
    - [x] preleva una quantità di energia da ENERGY_DEMAND
  
- [ ] processo atomo       (mantenere privato il numero atomico di ciascun processo)
    - [x] scissione, tenere conto di quante scissioni avvengono
    - [x] calcolo dei nuovi numeri atomici n1 e n2
    - [x] se n1 o n2 sono uguali allora l'energia sarà massima altrimenti vale zero se uno dei due vale 1.
    - [x] scorie, tenerne conto in base a quali numeri atomici sono minori o uguali a MIN_N_ATOMICO
    - [x] aggiornamento della free_energy  N1*N2 -MAX(N1,N2)
    - [x] la scissione viene comunicata dal processo attivatore e se ne tiene conto

- [x] processo attivatore
    - [ ] timer di STEP_ATTIVATORE
    - [x] comunicazione ad atomo di scindere
  
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <time.h>

#include "headers/utils.h"
#include "headers/io.h"
#include "headers/process.h"

// Global variables for shared memory and semaphore
shared_data *shm_data;
sem_t *sem;
pid_t a_pid,c_pid;

void timer_handler(int sig) {
    print_shared_data(shm_data);
    // Set the alarm again for the next second
    alarm(1);
}


void terminate_processes(pid_t a_pid, pid_t c_pid) {
    // Terminate attivatore process
    if (kill(a_pid, SIGTERM) == -1) {
        perror("Error terminating attivatore");
    }
    
    for(int i = 0; i < shm_data->num_processes; i++){
        if (kill(shm_data->pid_array[i], SIGTERM) == -1) {
            perror("Error terminating atomo");
        }
    }
}

int main(int argc, char *argv[]) {
    const char* filename = "variabili.txt";
    SimulationParameters params = leggiVariabili(filename);
    print_line();
    printf("PARAMETERS OBTAINED FROM THE FILE: \n");
    printSimulationParameters(&params);
    
    // Initialize shared memory and semaphore
    init_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);
    print_shared_data(shm_data);
    srand(time(NULL));
    struct sigaction sa;

    // Set up the signal handler
    sa.sa_handler = timer_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("sigaction failed");
        exit(EXIT_FAILURE);
    }

    // Set the initial alarm for 1 second
    if (alarm(1) == -1) {
        perror("alarm failed");
        exit(EXIT_FAILURE);
    }

    // Fork attivatore process
    a_pid = create_attivatore();



    // Fork atomo process
    for (int i = 0; i < params.n_atom_init; i++) {
        c_pid = create_atomo(&params.max_n_atomico, sem, shm_data);
    }

    // Wait for a certain amount of time
    srand(time(NULL));
    int count = 0;
    
    while(count < 8){
        pause(); 
        count++;
    }

    // Print shared data after sleep
    print_shared_data(shm_data);

    printf("Kill all the processes\n");
    // Terminate child processes
    terminate_processes(a_pid, c_pid);

    // Wait for child processes to terminate
    int status;
    waitpid(a_pid, &status, 0);
    waitpid(c_pid, &status, 0);
    printf("All processes terminated\n");
    print_line();

    // Cleanup shared memory and semaphore
    cleanup_shared_memory_and_semaphore(SEMAPHORE_NAME, &sem, SHARED_MEM_NAME, &shm_data);

    exit(EXIT_SUCCESS);
}

- [ ] processo alimentatore
    - [ ] timer di STEP_ALIMENTAZIONE nanosecondi 
    - [ ] creazione di N_NUOVI_ATOMI

