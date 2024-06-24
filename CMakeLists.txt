cmake_minimum_required(VERSION 3.10)
project(sistemi_operativi C)

set(CMAKE_C_STANDARD 23)

add_executable(atomo atomo.c)
add_executable(master master.c)
add_executable(attivatore attivatore.c)
add_executable(alimentatore alimentatore.c)
