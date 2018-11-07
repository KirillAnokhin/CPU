mkdir compile
gcc -o ./compile/cpu_t.o -c -g cpu_t.c
gcc -o ./compile/main.o -c -g main.c
gcc -o ./compile/cpu -g ./compile/cpu_t.o ./compile/main.o
