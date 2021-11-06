all: final.elf

final.elf: main.o libproject.a
        gcc one.o -lproject -L. -o final.elf

main.o: main.c myProject.h
        gcc -c main.c

libproject.a: 1.o
        ar rc libproject.a 1.o

1.o:1.c myProject.h
        gcc --static -c -o 1.o 1.c

clean:
        rm final.elf libproject.a main.o 1.o
