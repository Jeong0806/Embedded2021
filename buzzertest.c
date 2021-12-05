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



int main(int argc, char *argv[])
{
    int freIndex;
    if(argc < 2)
    {
        printf("Error!\n");
        doHelp();
        return 1;
    }

    buzzerInit();

    freIndex = atoi(argv[1]);

    buzzerPlaySong(freIndex);
    buzzerStopSong();
    buzzerExit();
    

    return 0;
}