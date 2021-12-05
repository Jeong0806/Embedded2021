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
#include "led.h"


int main(void)
{
    BUTTON_MSG_T messageRxData;
    int Msgid = msgget(MESSAGE_ID, IPC_CREAT|0666);
    while(1)
    {
        int return_value = 0;
        return_value = msgrcv(Msgid, &messageRxData, sizeof(int) * 2, 0, IPC_NOWAIT);
        if(return_value == -1) break;
    }
    buttonInit();
    while(1)
    {
        int return_value = 0;
        return_value = msgrcv(Msgid, &messageRxData, sizeof(int) * 2, 0, 0);
        if(messageRxData.pressed)
        {
            printf("EV_KEY(");
            switch(messageRxData.keyInput)
            {
                case KEY_VOLUMEUP: printf("volume up key): pressed\r\n"); break;
                case KEY_HOME: printf("Home key): pressed\r\n"); break;
                case KEY_SEARCH: printf("Search key): pressed\r\n"); break;
                case KEY_BACK: printf("Back key): pressed\r\n"); break;
                case KEY_MENU: printf("Menu key): pressed\r\n"); break;
                case KEY_VOLUMEDOWN: printf("volume down key): pressed\r\n"); break;
            }
        }
        else
        {
            printf("release\r\n");
        }
    }

    buttonExit();

    return 0;
}