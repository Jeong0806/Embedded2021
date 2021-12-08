CC = arm-linux-gnueabi-gcc
AR = arm-linux-gnueabi-ar

all: libMyPeri.a color_ledtest_kmk

libMyPeri.a: led.o button.o buzzer.o fnd.o lcdtext.o color_led.o
	$(AR) rc libMyPeri.a led.o button.o  buzzer.o fnd.o lcdtext.o color_led.o

led.o: led.h led.c
	$(CC) led.c -o led.o -c

button.o: button.h button.c
	$(CC) button.c -o button.o -c

buzzer.o: buzzer.h buzzer.c
	$(CC) buzzer.c -o buzzer.o -c

fnd.o: fnd.h fnd.c
	$(CC) fnd.c -o fnd.o -c

lcdtext.o: lcdtext.h lcdtext.c
	$(CC) lcdtext.c -o lcdtext.o -c

color_led.o: color_led.h color_led.c
	$(CC) color_led.c -o color_led.o -c

color_ledtest_kmk: led.h button.h buzzer.h fnd.h lcdtext.h color_led.h  libMyPeri.a
	$(CC) --static -c -o color_ledtest.o color_ledtest.c
	$(CC) color_ledtest.o -l MyPeri -L. -lpthread -o color_ledtest_kmk
	scp color_ledtest_kmk ecube@192.168.0.9:/home/ecube

clean:
	rm -rf *.o *.a color_ledtest_kmk

