#pragma once
#ifndef IO_H
#define IO_H

#include "utils.h"

SimulationParameters leggiVariabili(const char *filename);

void printSimulationParameters(const SimulationParameters *params);
void print_shared_data(shared_data *shm_data);
void print_line();

#endif