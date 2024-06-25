#include "util/utils.h"

int main(int argc, char *argv[]){
    const char* filename = "variabili.txt";
    SimulationParameters params = leggiVariabili(filename);
    printSimulationParameters(&params);

    return 0;
}