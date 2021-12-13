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
    int change_octave = 0; // Home키를 눌렀을 때 octave 변경
    int change_scale = 0; // back키를 눌렀을 때 음 변경 -> 버튼 수가 7개보다 적어 파트를 나눔
    BUTTON_MSG_T messageRxData;
    int Msgid = msgget(MESSAGE_ID, IPC_CREAT|0666);
    while(1)
    {
        int return_value = 0;
        return_value = msgrcv(Msgid, &messageRxData, sizeof(int) * 2, 0, IPC_NOWAIT);
        if(return_value == -1) break;
    }

    buttonInit();
    buzzerInit();
    buzzerEnable(0);
    while(1)
    {
        int doo = 262; int re = 294; int mi = 330; int pa = 349;
        if((change_scale % 2) == 1) // back를 통해 각 버튼에 해당하는 음을 바꿈
        {
            //doo를 sol로 바꿈, re를 ra로 바꿈, mi를 si로 바꿈, pa를 소리 안나게 바꿈
            doo = 392; re = 440; mi = 493; pa = 0;
        }
        int return_value = 0;
        return_value = msgrcv(Msgid, &messageRxData, sizeof(int) * 2, 0, 0);
        if(messageRxData.pressed)
        {
            printf("EV_KEY(");
            switch(messageRxData.keyInput)
            {
                case KEY_VOLUMEUP:
                printf("volume up key): pressed\r\n");
                buzzerEnable(1);
                if((change_octave % 2) == 0)
                {
                    setFrequency(mi);
                }
                else
                {
                    setFrequency(mi * 2);
                }
                sleep(1);
                buzzerEnable(0);
                break;
                case KEY_HOME:
                printf("Home key): pressed\r\n");
                change_octave++;
                break;
                case KEY_SEARCH: 
                printf("Search key): pressed\r\n");
                buzzerEnable(1);
                if((change_octave % 2) == 0)
                {
                    setFrequency(doo);
                }
                else
                {
                    setFrequency(doo * 2);
                }
                sleep(1);
                buzzerEnable(0);
                break;
                case KEY_BACK:
                printf("Back key): pressed\r\n");
                change_scale++;
                break;
                case KEY_MENU: 
                printf("Menu key): pressed\r\n");
                buzzerEnable(1);
                if((change_octave % 2) == 0)
                {
                    setFrequency(re);
                }
                else
                {
                    setFrequency(re * 2);
                }
                sleep(1);
                buzzerEnable(0);
                break;
                case KEY_VOLUMEDOWN: 
                printf("volume down key): pressed\r\n");
                buzzerEnable(1);
                if(pa == 0)
                {
                    buzzerEnable(0);
                }
                else
                {
                    if((change_octave % 2) == 0) // 처음 home키를 누르면 1옥타브 높은 음으로 이동
                    {                            // 다시 home키를 누르면 원래 옥타브로 이동
                        setFrequency(pa);
                    }
                    else
                    {
                        setFrequency(pa * 2);
                    }
                    sleep(1);
                    buzzerEnable(0);
                    break;
                }
            }
        }
        else
        {
            printf("release\r\n");
        }
    }

    buzzerStopSong();
    buttonExit();
    

    return 0;
}