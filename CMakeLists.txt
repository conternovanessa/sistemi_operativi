cmake_minimum_required(VERSION 3.10)
project(sistemi_operativi C)

set(CMAKE_C_STANDARD 23)

set(-D_GNU_SOURCE -Wvla -Wextra -Werror)

add_executable(master src/master.c src/io.c src/process.c)
add_executable(atomo src/atomo.c src/process.c src/io.c)
add_executable(attivatore src/attivatore.c src/process.c src/io.c)
add_executable(alimentatore src/alimentatore.c src/process.c src/io.c)