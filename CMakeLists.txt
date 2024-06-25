cmake_minimum_required(VERSION 3.10)
project(sistemi_operativi C)

set(CMAKE_C_STANDARD 23)

add_executable(atomo src/atomo.c)
add_executable(master src/master.c)
add_executable(attivatore src/attivatore.c)
add_executable(alimentatore src/alimentatore.c)
