#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(int argc, char *argv[]){
    if (argc < 2){
        perror("Necessary at least two inputs");
        exit(EXIT_FAILURE);
    }

    int num_atomico = atoi(argv[1]);

    printf("Received num_atomico: %d \n", num_atomico);

    sigset_t set;
    struct sigaction sa;
    siginfo_t info;

    // Block the signal we want to wait for
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    if (sigprocmask(SIG_BLOCK, &set, NULL) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    printf("Receiver process PID: %d\n", getpid());
    printf("Waiting for signal...\n");

    // Wait for the signal
    if (sigwaitinfo(&set, &info) == -1) {
        perror("sigwaitinfo");
        exit(EXIT_FAILURE);
    }

    if (info.si_signo == SIGUSR1) {
        printf("Received SIGUSR1, performing specific action.\n");
        // Perform the specific action here
    }

    exit(EXIT_SUCCESS);
}
