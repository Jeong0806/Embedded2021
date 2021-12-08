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

int main(int argc, char *argv[])
{
    int lineFlag = 0;
    lineFlag = lcdtextwrite(argv[1], argv[2]);
    if(lineFlag == -1)
    {
        printf("failed! try again!\n");
        return -1;
    }

    

    return 0;
}