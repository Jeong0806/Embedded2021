#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/input.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/msg.h>
#include <pthread.h>
#include <dirent.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <errno.h>
#include <getopt.h>
#include "button.h"
#include "led.h"
#include "buzzer.h"
#include "fnd.h"
#include "lcdtext.h"
#include "color_led.h"
#include "temperature.h"
#include "accelMagGyro.h"

int main(void)
{
    Accel();
    Magnet();
    Gyro();

    return 0;
}