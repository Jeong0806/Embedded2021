CC = arm-linux-gnueabi-gcc
AR = arm-linux-gnueabi-ar

all: libMyPeri.a ledtest

libMyPeri.a: led.o
	$(AR) rc libMyPeri.a led.o

led.o: led.h led.c
	$(CC) led.c -o led.o -c

ledtest: led.h libMyPeri.a
	$(CC) --static -c -o ledtest.o ledtest.c
	$(CC) ledtest.o -l MyPeri -L. -o ledtest
	scp ledtest ecube@192.168.0.9:/home/ecube

clean:
	rm -rf *.o *.a ledtest

