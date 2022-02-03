gcc -Wall -std=c11 -c -o config.o config.c 
gcc -Wall -std=c11 -c -o player.o player.c 
//gcc -Wall -std=c11 -c -o othello.o othello.c 
//gcc -Wall -std=c11 -o othello config.o player.o othello.o
gcc -Wall -fPIC -shared -O -g config.o player.o -o libhello.so
gcc -Wall -IC:\Users\geoff\CUnit-2.1-3\CUnit\Headers test_unit.c -LC:\Users\geoff\CUnit-2.1-3\CUnit\Sources\.libs -lcunit ./libhello.so -o executable
gcc -Wall -I..\CUnit-2.1-3\CUnit\Headers test_unit.c -L..\CUnit-2.1-3\CUnit\Sources\.libs -lcunit config.o player.o -o executable
gcc -Wall -I..\CUnit-2.1-3\CUnit\Headers -L..\CUnit-2.1-3\CUnit\Sources\.libs -o executable test_unit.c config.o player.o -lcunit

gcc -Wall -LD:\INSTALLER\CUnit-2.1-3\CUnit\Sources\.libs -ID:\INSTALLER\CUnit-2.1-3\CUnit\Headers -o test_unite test_unit.c player.o config.o -lcunit
C:\MinGW\bin\gcc -Wall -LC:\.libs -IC:Headers -o max_test max_test.c max.o -lcunit


gcc -Wall -std=c11 -c -o config.o config.c 
gcc -Wall -std=c11 -c -o player.o player.c 
gcc -Wall -ID:\INSTALLER\CUnit-2.1-3\CUnit\Headers -LD:\INSTALLER\CUnit-2.1-3\CUnit\Sources\.libs -o test_unite test_unit.c player.o config.o -lcunit