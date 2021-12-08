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

#define FND_DRIVER_NAME "/dev/perifnd"
#define MAX_FND_NUM 6
#define FND_DATA_BUFF_LEN (MAX_FND_NUM + 2)

typedef struct FNDWriteDataForm_tag
{
    char DataNumeric[FND_DATA_BUFF_LEN];
    char DataDot[FND_DATA_BUFF_LEN];
    char DataVaild[FND_DATA_BUFF_LEN];
}stFndWriteForm, *pStFndWriteForm;

int fndDisp(int num, int dotflag)
{
    int fd;
    int temp, i;
    stFndWriteForm stWriteData;
    for(i = 0; i < MAX_FND_NUM; i++)
    {
        stWriteData.DataDot[i] = (dotflag & (0x1 << i)) ? 1 : 0;
        stWriteData.DataVaild[i] = 1;
    }

    temp = num % 1000000; stWriteData.DataNumeric[0] = temp / 100000;
    temp = num % 100000; stWriteData.DataNumeric[1] = temp / 10000;
    temp = num % 10000; stWriteData.DataNumeric[2] = temp / 1000;
    temp = num % 1000; stWriteData.DataNumeric[3] = temp / 100;
    temp = num % 100; stWriteData.DataNumeric[4] = temp / 10;
    stWriteData.DataNumeric[5] = num % 10;

    fd = open(FND_DRIVER_NAME, O_RDWR);
    if(fd < 0)
    {
        printf("driver open failed\n");
        return 0;
    }

    write(fd, &stWriteData, sizeof(stFndWriteForm));
    close(fd);
    return 1;
}
