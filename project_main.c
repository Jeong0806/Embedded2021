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
    int home_for_makeSong = 0; //작곡 모드인지 판단하기 위한 변수
    int back_for_PlaySong = 0; // 저장한 곡을 재생하는 모드인지 판단하기 위한 변수
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
        if(home_for_makeSong == 3 && back_for_PlaySong == 1) // 저장된 곡 재생
        {
            if(messageRxData.pressed)
            {
                switch(messageRxData.keyInput)
                {
                    case KEY_HOME: // home키 누르면 저장된 곡 재생 모드에서 빠져나감
                    home_for_makeSong = 0; back_for_PlaySong = 0;
                    break;
                    case KEY_BACK:
                    buzzerEnable(1);
                    //buzzerPlaySong(1);
                    //buzzerEnable(0);
                    print_list(1);
                    break;
                    case KEY_SEARCH:
                    buzzerEnable(1);
                    //buzzerPlaySong(2);
                    //uzzerEnable(0);
                    print_list(2);
                    break;
                    case KEY_MENU:
                    buzzerEnable(1);
                    //buzzerPlaySong(3);
                    //buzzerEnable(0);
                    print_list(3);
                    break;
                    case KEY_VOLUMEUP:
                    buzzerEnable(1);
                    //buzzerPlaySong(4);
                    //buzzerEnable(0);
                    print_list(4);
                    break;
                    case KEY_VOLUMEDOWN:
                    buzzerEnable(1);
                    //buzzerPlaySong(5);
                    //buzzerEnable(0);
                    print_list(5);
                    break;

                }
            }
        }
        else if(home_for_makeSong == 4) // 작곡 모드
        {
            //delete_song(1); delete_song(2); delete_song(3); delete_song(4); delete_song(5);
            printf("making mode\n");
            lcdtextwrite("1", "making mode");
            number_of_song(); // 몇 번이 비어있는지 lcd에 출력
            int home;
            if(messageRxData.pressed)
            {
                switch(messageRxData.keyInput)
                {
                    case KEY_HOME:
                    home_for_makeSong = 0; //home 키를 누르면 작곡 모드 종료
                    //lcdtextwrite("1", "playing mode"); // 작곡 모드를 나가면 연주 모드
                    break;
                    case KEY_BACK:
                    home = 0; // while문에서 빠져나오기 위한 변수 -> 1번째 곡의 작곡을 멈추기 위한 것.
                    while(1)
                    {
                        doo = 262; re = 294; mi = 330; pa = 349;
                        if((change_scale % 2) == 1) // back를 통해 각 버튼에 해당하는 음을 바꿈
                        {
                            //doo를 sol로 바꿈, re를 ra로 바꿈, mi를 si로 바꿈, pa를 소리 안나게 바꿈
                            doo = 392; re = 440; mi = 493; pa = 0;
                        }
                        if(home == 2) // 다른 키 안누르고 홈 키를 두 번 연속 누르면 종료되도록..
                        {
                            break;
                        }
                        msgrcv(Msgid, &messageRxData, sizeof(int) * 2, 0, 0);
                        if(messageRxData.pressed)
                        {
                            switch(messageRxData.keyInput)
                            {
                                case KEY_HOME: // 다른 키 안누르고 두 번 연속 누르면 종료되도록..
                                change_octave++;
                                home++;
                                break;
                                case KEY_BACK:
                                change_scale++;
                                home = 0;
                                break;
                                case KEY_SEARCH:
                                home = 0;
                                if((change_octave % 2) == 0)
                                {
                                    insert_last1(doo);
                                }
                                else
                                {
                                    insert_last1(doo * 2);
                                }
                                break;
                                case KEY_MENU:
                                home = 0;
                                if((change_octave % 2) == 0)
                                {
                                    insert_last1(re);
                                }
                                else
                                {
                                    insert_last1(re * 2);
                                }
                                break;
                                case KEY_VOLUMEUP:
                                home = 0;
                                if((change_octave % 2) == 0)
                                {
                                    insert_last1(mi);
                                }
                                else
                                {
                                    insert_last1(mi * 2);
                                }
                                break;
                                case KEY_VOLUMEDOWN:
                                home = 0;
                                if(pa != 0)
                                {
                                    if((change_octave % 2) == 0)
                                    {
                                        insert_last1(pa);
                                    }
                                    else
                                    {
                                        insert_last1(pa * 2);
                                    }
                                }
                            
                                break;
                            }
                        }
                    }
                    //buzzerEnable(1);
                    //print_list(1);
                    //printf("playing 1 finish\n");
                    printf("first song maked\n");
                    break;
                    case KEY_SEARCH:
                    home = 0; // while문에서 빠져나오기 위한 변수 -> 2번째 곡의 작곡을 멈추기 위한 것.
                    while(1)
                    {
                        doo = 262; re = 294; mi = 330; pa = 349;
                        if((change_scale % 2) == 1) // back를 통해 각 버튼에 해당하는 음을 바꿈
                        {
                            //doo를 sol로 바꿈, re를 ra로 바꿈, mi를 si로 바꿈, pa를 소리 안나게 바꿈
                            doo = 392; re = 440; mi = 493; pa = 0;
                        }
                        if(home == 2) // 다른 키 안누르고 홈 키를 두 번 연속 누르면 종료되도록..
                        {
                            break;
                        }
                        msgrcv(Msgid, &messageRxData, sizeof(int) * 2, 0, 0);
                        if(messageRxData.pressed)
                        {
                            switch(messageRxData.keyInput)
                            {
                                case KEY_HOME: // 다른 키 안누르고 두 번 연속 누르면 종료되도록..
                                change_octave++;
                                home++;
                                break;
                                case KEY_BACK:
                                change_scale++;
                                home = 0;
                                break;
                                case KEY_SEARCH:
                                home = 0;
                                if((change_octave % 2) == 0)
                                {
                                    insert_last2(doo);
                                }
                                else
                                {
                                    insert_last2(doo * 2);
                                }
                                break;
                                case KEY_MENU:
                                home = 0;
                                if((change_octave % 2) == 0)
                                {
                                    insert_last2(re);
                                }
                                else
                                {
                                    insert_last2(re * 2);
                                }
                                break;
                                case KEY_VOLUMEUP:
                                home = 0;
                                if((change_octave % 2) == 0)
                                {
                                    insert_last2(mi);
                                }
                                else
                                {
                                    insert_last2(mi * 2);
                                }
                                break;
                                case KEY_VOLUMEDOWN:
                                home = 0;
                                if(pa != 0)
                                {
                                    if((change_octave % 2) == 0)
                                    {
                                        insert_last2(pa);
                                    }
                                    else
                                    {
                                        insert_last2(pa * 2);
                                    }
                                }
                            
                                break;
                            }
                        }
                    }
                    //buzzerEnable(1);
                    //print_list(2);
                    //printf("playing 2 finish\n");
                    printf("second song maked\n");
                    break;
                    case KEY_MENU:
                    home = 0; // while문에서 빠져나오기 위한 변수 -> 3번째 곡의 작곡을 멈추기 위한 것.
                    while(1)
                    {
                        doo = 262; re = 294; mi = 330; pa = 349;
                        if((change_scale % 2) == 1) // back를 통해 각 버튼에 해당하는 음을 바꿈
                        {
                            //doo를 sol로 바꿈, re를 ra로 바꿈, mi를 si로 바꿈, pa를 소리 안나게 바꿈
                            doo = 392; re = 440; mi = 493; pa = 0;
                        }
                        if(home == 2) // 다른 키 안누르고 홈 키를 두 번 연속 누르면 종료되도록..
                        {
                            break;
                        }
                        msgrcv(Msgid, &messageRxData, sizeof(int) * 2, 0, 0);
                        if(messageRxData.pressed)
                        {
                            switch(messageRxData.keyInput)
                            {
                                case KEY_HOME: // 다른 키 안누르고 두 번 연속 누르면 종료되도록..
                                change_octave++;
                                home++;
                                break;
                                case KEY_BACK:
                                change_scale++;
                                home = 0;
                                break;
                                case KEY_SEARCH:
                                home = 0;
                                if((change_octave % 2) == 0)
                                {
                                    insert_last3(doo);
                                }
                                else
                                {
                                    insert_last3(doo * 2);
                                }
                                break;
                                case KEY_MENU:
                                home = 0;
                                if((change_octave % 2) == 0)
                                {
                                    insert_last3(re);
                                }
                                else
                                {
                                    insert_last3(re * 2);
                                }
                                break;
                                case KEY_VOLUMEUP:
                                home = 0;
                                if((change_octave % 2) == 0)
                                {
                                    insert_last3(mi);
                                }
                                else
                                {
                                    insert_last3(mi * 2);
                                }
                                break;
                                case KEY_VOLUMEDOWN:
                                home = 0;
                                if(pa != 0)
                                {
                                    if((change_octave % 2) == 0)
                                    {
                                        insert_last3(pa);
                                    }
                                    else
                                    {
                                        insert_last3(pa * 2);
                                    }
                                }
                            
                                break;
                            }
                        }
                    }
                    //buzzerEnable(1);
                    //print_list(3);
                    //printf("playing 3 finish\n");
                    printf("third song maked\n");
                    break;
                    case KEY_VOLUMEUP:
                    home = 0; // while문에서 빠져나오기 위한 변수 -> 4번째 곡의 작곡을 멈추기 위한 것.
                    while(1)
                    {
                        doo = 262; re = 294; mi = 330; pa = 349;
                        if((change_scale % 2) == 1) // back를 통해 각 버튼에 해당하는 음을 바꿈
                        {
                            //doo를 sol로 바꿈, re를 ra로 바꿈, mi를 si로 바꿈, pa를 소리 안나게 바꿈
                            doo = 392; re = 440; mi = 493; pa = 0;
                        }
                        if(home == 2) // 다른 키 안누르고 홈 키를 두 번 연속 누르면 종료되도록..
                        {
                            break;
                        }
                        msgrcv(Msgid, &messageRxData, sizeof(int) * 2, 0, 0);
                        if(messageRxData.pressed)
                        {
                            switch(messageRxData.keyInput)
                            {
                                case KEY_HOME: // 다른 키 안누르고 두 번 연속 누르면 종료되도록..
                                change_octave++;
                                home++;
                                break;
                                case KEY_BACK:
                                change_scale++;
                                home = 0;
                                break;
                                case KEY_SEARCH:
                                home = 0;
                                if((change_octave % 2) == 0)
                                {
                                    insert_last4(doo);
                                }
                                else
                                {
                                    insert_last4(doo * 2);
                                }
                                break;
                                case KEY_MENU:
                                home = 0;
                                if((change_octave % 2) == 0)
                                {
                                    insert_last4(re);
                                }
                                else
                                {
                                    insert_last4(re * 2);
                                }
                                break;
                                case KEY_VOLUMEUP:
                                home = 0;
                                if((change_octave % 2) == 0)
                                {
                                    insert_last4(mi);
                                }
                                else
                                {
                                    insert_last4(mi * 2);
                                }
                                break;
                                case KEY_VOLUMEDOWN:
                                home = 0;
                                if(pa != 0)
                                {
                                    if((change_octave % 2) == 0)
                                    {
                                        insert_last4(pa);
                                    }
                                    else
                                    {
                                        insert_last4(pa * 2);
                                    }
                                }
                            
                                break;
                            }
                        }
                    }
                    //buzzerEnable(1);
                    //print_list(4);
                    //printf("playing 4 finish\n");
                    printf("forth song maked\n");
                    break;
                    case KEY_VOLUMEDOWN:
                    home = 0; // while문에서 빠져나오기 위한 변수 -> 5번째 곡의 작곡을 멈추기 위한 것.
                    while(1)
                    {
                        doo = 262; re = 294; mi = 330; pa = 349;
                        if((change_scale % 2) == 1) // back를 통해 각 버튼에 해당하는 음을 바꿈
                        {
                            //doo를 sol로 바꿈, re를 ra로 바꿈, mi를 si로 바꿈, pa를 소리 안나게 바꿈
                            doo = 392; re = 440; mi = 493; pa = 0;
                        }
                        if(home == 2) // 다른 키 안누르고 홈 키를 두 번 연속 누르면 종료되도록..
                        {
                            break;
                        }
                        msgrcv(Msgid, &messageRxData, sizeof(int) * 2, 0, 0);
                        if(messageRxData.pressed)
                        {
                            switch(messageRxData.keyInput)
                            {
                                case KEY_HOME: // 다른 키 안누르고 두 번 연속 누르면 종료되도록..
                                change_octave++;
                                home++;
                                break;
                                case KEY_BACK:
                                change_scale++;
                                home = 0;
                                break;
                                case KEY_SEARCH:
                                home = 0;
                                if((change_octave % 2) == 0)
                                {
                                    insert_last5(doo);
                                }
                                else
                                {
                                    insert_last5(doo * 2);
                                }
                                break;
                                case KEY_MENU:
                                home = 0;
                                if((change_octave % 2) == 0)
                                {
                                    insert_last5(re);
                                }
                                else
                                {
                                    insert_last5(re * 2);
                                }
                                break;
                                case KEY_VOLUMEUP:
                                home = 0;
                                if((change_octave % 2) == 0)
                                {
                                    insert_last5(mi);
                                }
                                else
                                {
                                    insert_last5(mi * 2);
                                }
                                break;
                                case KEY_VOLUMEDOWN:
                                home = 0;
                                if(pa != 0)
                                {
                                    if((change_octave % 2) == 0)
                                    {
                                        insert_last5(pa);
                                    }
                                    else
                                    {
                                        insert_last5(pa * 2);
                                    }
                                }
                            
                                break;
                            }
                        }
                    }
                    //buzzerEnable(1);
                    //print_list(5);
                    //printf("playing 5 finish\n");
                    printf("fifth song maked\n");
                    break;
                }
            }
            if(home_for_makeSong == 0)
            {
                printf("Exit making mode\n");
            }
        }
        else
        {
            if(messageRxData.pressed)
            {
                printf("EV_KEY(");
                switch(messageRxData.keyInput)
                {
                    case KEY_VOLUMEUP:
                    back_for_PlaySong = 0; // 버튼으로 연주하는 도중에 실수로 저장된 곡을
                    home_for_makeSong = 0; // 재생하는 매커니즘 대로 버튼을 눌러도 실행되지 않게..
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
                    home_for_makeSong++;
                    break;
                    case KEY_SEARCH:
                    back_for_PlaySong = 0; // 버튼으로 연주하는 도중에 실수로 저장된 곡을
                    home_for_makeSong = 0; // 재생하는 매커니즘 대로 버튼을 눌러도 실행되지 않게..
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
                    back_for_PlaySong++;
                    break;
                    case KEY_MENU:
                    back_for_PlaySong = 0; // 버튼으로 연주하는 도중에 실수로 저장된 곡을
                    home_for_makeSong = 0; // 재생하는 매커니즘 대로 버튼을 눌러도 실행되지 않게..
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
                    back_for_PlaySong = 0; // 버튼으로 연주하는 도중에 실수로 저장된 곡을
                    home_for_makeSong = 0; // 재생하는 매커니즘 대로 버튼을 눌러도 실행되지 않게..
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
    }

    buzzerStopSong();
    buttonExit();
    

    return 0;
}