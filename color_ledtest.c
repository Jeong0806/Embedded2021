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
#include "button.h"
#include "led.h"
#include "buzzer.h"
#include "fnd.h"
#include "lcdtext.h"
#include "color_led.h"

int main(int argc, char *argv[])
{
    pwmLedInit();
    sleep(5);
    pwmSetPercent(atoi(argv[1]), 0);
    sleep(5);
    pwmSetPercent(atoi(argv[2]), 1);
    sleep(5);
    pwmSetPercent(atoi(argv[3]), 2);
    sleep(5);
    pwmInactiveAll();
    
    return 0;
}