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

#define MAX_SCALE_STEP 8
#define BUZZER_BASE_SYS_PATH "/sys/bus/platform/devices"
#define BUZZER_FILENAME "peribuzzer"
#define BUZZER_ENABLE_NAME "enable"
#define BUZZER_FREQUENCY_NAME "frequency"
char gBuzzerBaseSysDir[128]; //  /sys/bus/platform/devices/peribuzzer.숫자 가 결정됨

static int fp = 0;

const int musicScale[MAX_SCALE_STEP] = 
{
    /*do*/262, 294, 330, 349, 392, 440, 494, 523/*si*/
};

int findBuzzerSysPath(){ //못찾으면 ifNotFound -> 1을 리턴
    DIR *dir_info = opendir(BUZZER_BASE_SYS_PATH);
    int ifNotFound = 1;
    if(dir_info != NULL)
    {
        while(1)
        {
            struct dirent *dir_entry;
            dir_entry = readdir(dir_info);
            if(dir_entry == NULL) break;
            if(strncasecmp(BUZZER_FILENAME, dir_entry->d_name, strlen(BUZZER_FILENAME)) == 0)
            {
                ifNotFound = 0;
                sprintf(gBuzzerBaseSysDir, "%s/%s/", BUZZER_BASE_SYS_PATH, dir_entry->d_name);
                printf("find %s\n", gBuzzerBaseSysDir);
                return ifNotFound;
            }
        }
    }
    printf("Cannot Found\n");
    return ifNotFound;
}

void doHelp(void)
{
    printf("Usage:\n");
    printf("buzzertest <buzzerN> \n");
    printf("buzzerNo: \n");
    printf("do(1),re(2),mi(3),fa(4),sol(5),ra(6),si(7),do(8) \n");
    printf("off(0)\n");
}

void buzzerEnable(int bEnable)
{
    char path[200];
    sprintf(path, "%s%s", gBuzzerBaseSysDir, BUZZER_ENABLE_NAME);
    int fd = open(path, O_WRONLY);
    printf("%s\n", path);
    if(bEnable)
    {
        write(fd, &"1", 1);
        printf("echo 1 > enable\n");
    }
    else
    {
        write(fd, &"0", 1);
        printf("echo 0 > enable\n");
    }
    close(fd);
}

void setFrequency(int frequency)
{
    char path[200];
    sprintf(path, "%s%s", gBuzzerBaseSysDir, BUZZER_FREQUENCY_NAME);
    int fd = open(path, O_WRONLY);
    dprintf(fd, "%d", frequency);
    close(fd);
}

int buzzerInit(void)
{
    if(findBuzzerSysPath() == 1)
    {
        printf("error!");
        return 0;
    }
    printf("buzzerPath: %s\n", gBuzzerBaseSysDir);
    
    buzzerEnable(1);
    return 1;
}

int buzzerPlaySong(int scale)
{
    char path[200];
    sprintf(path, "%s%s", gBuzzerBaseSysDir, BUZZER_FREQUENCY_NAME);
    fp = open(path, O_WRONLY);
    printf("%s\n", path);
    int i = 0;
    while(i != 10)
    {
        dprintf(fp, "%d", musicScale[scale - 1 + i++]);
        sleep(1);
    }
    
    return 1;
}

int buzzerStopSong(void)
{
    printf("stop song\n");
    close(fp);
    return 1;
}



int buzzerExit(void)
{
    buzzerEnable(0);
    return 1;
}