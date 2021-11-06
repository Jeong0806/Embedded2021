all: final.elf

final.elf: main.o 1.o
        gcc main.o 1.o -o final.elf

main.o: main.c myProject.h
        gcc -c main.c

1.o: 1.c myProject.h
        gcc -c 1.c

clean:
        rm final.elf main.o 1.o
