gcc -Wall -std=c11 -c -o config.o config.c 
gcc -Wall -std=c11 -c -o player.o player.c 
gcc -Wall -std=c11 -c -o othello.o othello.c 
gcc -Wall -std=c11 -o othello config.o player.o othello.o
D:\INSTALLER\CUnit-2.1-3\CUnit\Sources\.libs -ID:\INSTALLER\CUnit-2.1-3\CUnit\Headers
gcc -Wall -ID:\INSTALLER\CUnit-2.1-3\CUnit\Headers test_unit.c -LD:\INSTALLER\CUnit-2.1-3\CUnit\Sources\.libs -lcunit ./libhello.so -o executable
