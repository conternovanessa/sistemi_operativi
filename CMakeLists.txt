cmake_minimum_required(VERSION 3.10)
project(sistemi_operativi C)

set(CMAKE_C_STANDARD 17)

set(-D_GNU_SOURCE -Wvla -Wextra -Werror)

add_executable(master src/master.c src/util/utils.c)
add_executable(atomo src/atomo.c)
add_executable(attivatore src/attivatore.c)
# add_executable(alimentatore src/alimentatore.c)
