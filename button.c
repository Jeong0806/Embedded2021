#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/input.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/msg.h>
#include <pthread.h>
#include "button.h"

#define INPUT_DEVICE_LIST "/dev/input/event"
#define PROBE_FILE "/proc/bus/input/devices"
#define HAVE_TO_FIND_1 "N: Name=\"ecube-button\""
#define HAVE_TO_FIND_2 "H: Handlers=kbd event"

pthread_t buttonTh_id;
pthread_mutex_t lock;

static int fd = 0;
static int msgID = 0;

int probeButtonPath(char *newPath)
{
    int returnValue = 0;
    int number = 0;
    int i = 0;
    FILE *fp = fopen(PROBE_FILE, "rt");
    while(!feof(fp))
    {
        char temStr[200];
        fgets(temStr, 200, fp);

        if(strncasecmp(temStr, HAVE_TO_FIND_1, 15) == 0)
        {
            printf("YES! I found!: %s\r\n", temStr);
            returnValue = 1;
        }

        if((returnValue == 1) && (strncasecmp(temStr, HAVE_TO_FIND_2, strlen(HAVE_TO_FIND_2)) == 0))
        {
            printf("-->%s", temStr);
            printf("\t%c\r\n", temStr[strlen(temStr) - 3]);
            number = temStr[strlen(temStr) - 3] - '0';
            break;
        }
    }

    fclose(fp);

    
    if(returnValue == 1)
    {
        sprintf(newPath, "%s%d", INPUT_DEVICE_LIST, number);
    }

    return returnValue;
}

void *buttonThFunc(void *arg)
{
    BUTTON_MSG_T messageTxData;
    int readSize, inputIndex;
    struct input_event stEvent;
    
    while(1)
    {
        readSize = read(fd, &stEvent, sizeof(stEvent));
        if(readSize != sizeof(stEvent))
        {
            continue;
        }
    
        if(stEvent.type == EV_KEY)
        {
            switch(stEvent.code)
            {
                case KEY_VOLUMEUP:
                messageTxData.keyInput = stEvent.code;
                messageTxData.messageNum = 1;
                break;
                case KEY_HOME:
                messageTxData.keyInput = stEvent.code;
                messageTxData.messageNum = 1;
                break;
                case KEY_SEARCH:
                messageTxData.keyInput = stEvent.code;
                messageTxData.messageNum = 1;
                break;
                case KEY_BACK:
                messageTxData.keyInput = stEvent.code;
                messageTxData.messageNum = 1;
                break;
                case KEY_MENU:
                messageTxData.keyInput = stEvent.code;
                messageTxData.messageNum = 1;
                break;
                case KEY_VOLUMEDOWN:
                messageTxData.keyInput = stEvent.code;
                messageTxData.messageNum = 1;
                break;
            }
            if(stEvent.value)
            {
                messageTxData.pressed = stEvent.value;
                msgsnd(msgID, &messageTxData, sizeof(int) * 2, 0);
            }
            else
            {
                messageTxData.pressed = stEvent.value;
                msgsnd(msgID, &messageTxData, sizeof(int) * 2, 0);
            }
        }
        else
        {
            
        }
    }
    return NULL;
}

int buttonInit(void)
{
    char buttonPath[200] = {0, };
    if(probeButtonPath(buttonPath) == 0)
    {
        printf("error!");
        return 0;
    }
    printf("buttonPath: %s\r\n", buttonPath);
    
    fd = open(buttonPath, O_RDONLY);
    msgID = msgget(MESSAGE_ID, IPC_CREAT|0666);
    pthread_create(&buttonTh_id, NULL, buttonThFunc, NULL);
    return 1;
}

int buttonExit(void)
{
    pthread_join(buttonTh_id, NULL);
    close(fd);
}
