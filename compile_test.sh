#!/bin/bash

make

gcc -Wall -fPIC -shared -O -g config.o player.o -o libhello.so

gcc -Wall -I/home/secret/CUnitHome/include/CUnit test_unit.c -L/home/secret/CUnitHome/lib -lcunit ./libhello.so -o executable

LD_LIBRARY_PATH=/home/secret/CUnitHome/lib ./executable
