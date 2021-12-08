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

#define MODE_STATIC_DIS 1
#define MODE_TIME_DIS 2
#define MODE_COUNT_DIS 3

int main(int argc, char *argv[])
{
    int mode = atoi(argv[1]);
    int number = atoi(argv[2]);

    if(mode == MODE_STATIC_DIS)
    {
        printf("input display mode\n");
        fndDisp(number, 0);
    }
    else if(mode == MODE_TIME_DIS)
    {
        printf("current time display mode\n");
        struct tm *ptmcur;
        time_t tTime;
        if( -1 == time(&tTime))
        {
            return -1;
        }

        ptmcur = localtime(&tTime);

        number = ptmcur->tm_hour * 10000;
        number += ptmcur->tm_min * 100;
        number += ptmcur->tm_sec;

        fndDisp(number, 0b1010);
    }
    else if(mode == MODE_COUNT_DIS)
    {
        printf("count up mode\n");
        int count = 0;

        while(1)
        {
            if(!fndDisp(count, 0)) break;
            printf("%d->", count);
            count++;
            sleep(1);
            if(count > number) break;
        }
        printf("\n");
    }
    return 0;
}