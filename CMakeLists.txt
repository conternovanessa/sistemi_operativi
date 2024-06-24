cmake_minimum_required(VERSION 3.28)
project(untitled C)

set(CMAKE_C_STANDARD 23)

add_executable(untitled
        atomo.c
        master.c
        attivatore.c
        alimentatore.c)
