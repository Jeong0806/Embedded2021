all: final.elf

final.elf: main.o libproject.a
	gcc main.o -l project -L. -o final.elf

main.o: main.c myProject.h
	gcc main.c -o main.o

libproject.a: 1.o
	ar rc libproject.a 1.o

1.o: 1.c myProject.h
	gcc --static -c -o 1.o 1.c


clean:
	rm -rf *.elf *.o *.a
