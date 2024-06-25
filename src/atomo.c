#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]){
    if (argc < 2){
        perror("Necessary at least two inputs");
        exit(EXIT_FAILURE);
    }

    int num_atomico = atoi(argv[1]);

    printf("Received num_atomico: %d \n", num_atomico);

    exit(EXIT_SUCCESS);
}
