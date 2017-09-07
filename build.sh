#!/bin/bash
mkdir -p obj && cd obj
gcc -c ../src/*.c ../src/linux/*.c && gcc -o ../bin/cipher *.o -lncurses
