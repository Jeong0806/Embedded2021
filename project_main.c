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
    int fnd_play1_time = 0; // 해당 곡의 재생시간을 fnd에 표시하기 위한 변수.
    int fnd_play2_time = 0;
    int fnd_play3_time = 0;
    int fnd_play4_time = 0;
    int fnd_play5_time = 0;
    int count; // 재생시간을 카운트 다운할 변수
    pid_t pid; //곡 재생과 카운트 다운을 같이 하기 위해..
    int status;

    BUTTON_MSG_T messageRxData;
    int Msgid = msgget(MESSAGE_ID, IPC_CREAT|0666);
    while(1)
    {
        int return_value = 0;
        return_value = msgrcv(Msgid, &messageRxData, sizeof(int) * 2, 0, IPC_NOWAIT);
        if(return_value == -1) break;
    }

    ledLibinit();
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
            ledOnOff(1, 1); // '솔라시'인지 '도레미파'인지 구분하기 편하게 하기 위해 led on -> 솔라시
        }
        else
        {
            ledOnOff(1, 0); // '솔라시'인지 '도레미파'인지 구분하기 편하게 하기 위해 led off -> 도레미파
        }

        if((change_octave % 2) == 1)
        {
            ledOnOff(0, 1); // 한단계 위 옥타브
        }
        else
        {
            ledOnOff(0, 0); // 기본 옥타브
        }

        if(back_for_PlaySong == 4) // delete mode로 진입 -> 저장된 곡 삭제
        {
            while(1)
            {
                msgrcv(Msgid, &messageRxData, sizeof(int) * 2, 0, 0);
                lcdtextwrite("1", "delete mode");
                list_number_of_song();
                if(messageRxData.pressed)
                {
                    switch(messageRxData.keyInput)
                    {
                        case KEY_HOME:
                        home_for_makeSong = 0;
                        back_for_PlaySong = 0; // delete mode 에서 빠져나오기 위해..
                        lcdtextwrite("1", "Playing mode");
                        break;
                        case KEY_BACK:
                        delete_song(1); // 1번 곡 삭제
                        lcdtextwrite("1", "1th song deleted");
                        fnd_play1_time = 0;
                        sleep(1);
                        break;
                        case KEY_SEARCH:
                        delete_song(2); // 2번 곡 삭제
                        lcdtextwrite("1", "2th song deleted");
                        fnd_play2_time = 0;
                        sleep(1);
                        break;
                        case KEY_MENU:
                        delete_song(3); // 3번 곡 삭제
                        lcdtextwrite("1", "3th song deleted");
                        fnd_play3_time = 0;
                        sleep(1);
                        break;
                        case KEY_VOLUMEUP:
                        delete_song(4); // 4번 곡 삭제
                        lcdtextwrite("1", "4th song deleted");
                        fnd_play4_time = 0;
                        sleep(1);
                        break;
                        case KEY_VOLUMEDOWN:
                        delete_song(5); // 5번 곡 삭제
                        lcdtextwrite("1", "5th song deleted");
                        fnd_play5_time = 0;
                        sleep(1);
                        break;
                    }
                    
                }

                if(back_for_PlaySong == 0)
                {
                    break;
                }
            }
        }

        int return_value = 0;
        return_value = msgrcv(Msgid, &messageRxData, sizeof(int) * 2, 0, 0);
        if(home_for_makeSong == 3 && back_for_PlaySong == 1) // 저장된 곡 재생
        {
            lcdtextwrite("1", "Stored music mode");
            list_number_of_song();
            if(messageRxData.pressed)
            {
                switch(messageRxData.keyInput)
                {
                    case KEY_HOME: // home키 누르면 저장된 곡 재생 모드에서 빠져나감
                    home_for_makeSong = 0; back_for_PlaySong = 0;
                    lcdtextwrite("1", "playing mode");
                    break;
                    case KEY_BACK:
                    buzzerEnable(1);
                    lcdtextwrite("1", "playing 1");
                    pid = fork();
                    if(pid > 0)
                    {
                        print_list(1); // 저장된 곡 재생
                        wait(&status);
                    }
                    else if(pid == 0)
                    {
                        count = fnd_play1_time;
                        while(1)
                        {
                            if(count == 0) break;
                            fndDisp(count, 0);
                            count--;
                            sleep(1);
                        }
                        fndDisp(count, 0);
                        exit(1234);
                    }
                    break;
                    case KEY_SEARCH:
                    buzzerEnable(1);
                    lcdtextwrite("1", "playing 2");
                    pid = fork();
                    if(pid > 0)
                    {
                        print_list(2); // 저장된 곡 재생
                        wait(&status);
                    }
                    else if(pid == 0)
                    {
                        count = fnd_play2_time;
                        while(1)
                        {
                            if(count == 0) break;
                            fndDisp(count, 0);
                            count--;
                            sleep(1);
                        }
                        fndDisp(count, 0);
                        exit(1234);
                    }
                    break;
                    case KEY_MENU:
                    buzzerEnable(1);
                    lcdtextwrite("1", "playing 3");
                    pid = fork();
                    if(pid > 0)
                    {
                        print_list(3); // 저장된 곡 재생
                        wait(&status);
                    }
                    else if(pid == 0)
                    {
                        count = fnd_play3_time;
                        while(1)
                        {
                            if(count == 0) break;
                            fndDisp(count, 0);
                            count--;
                            sleep(1);
                        }
                        fndDisp(count, 0);
                        exit(1234);
                    }
                    break;
                    case KEY_VOLUMEUP:
                    buzzerEnable(1);
                    lcdtextwrite("1", "playing 4");
                    pid = fork();
                    if(pid > 0)
                    {
                        print_list(4); // 저장된 곡 재생
                        wait(&status);
                    }
                    else if(pid == 0)
                    {
                        count = fnd_play4_time;
                        while(1)
                        {
                            if(count == 0) break;
                            fndDisp(count, 0);
                            count--;
                            sleep(1);
                        }
                        fndDisp(count, 0);
                        exit(1234);
                    }
                    break;
                    case KEY_VOLUMEDOWN:
                    buzzerEnable(1);
                    lcdtextwrite("1", "playing 5");
                    pid = fork();
                    if(pid > 0)
                    {
                        print_list(5); // 저장된 곡 재생
                        wait(&status);
                    }
                    else if(pid == 0)
                    {
                        count = fnd_play5_time;
                        while(1)
                        {
                            if(count == 0) break;
                            fndDisp(count, 0);
                            count--;
                            sleep(1);
                        }
                        fndDisp(count, 0);
                        exit(1234);
                    }
                    break;

                }
            }
        }
        else if(home_for_makeSong == 4) // 작곡 모드
        {
            //delete_song(1); delete_song(2); delete_song(3); delete_song(4); delete_song(5);
            printf("making mode\n");
            lcdtextwrite("1", "making mode");
            empty_number_of_song(); // 몇 번이 비어있는지 lcd에 출력
            int home;
            if(messageRxData.pressed)
            {
                switch(messageRxData.keyInput)
                {
                    case KEY_HOME:
                    home_for_makeSong = 0; //home 키를 누르면 작곡 모드 종료
                    lcdtextwrite("1", "playing mode"); // 작곡 모드를 나가면 연주 모드
                    break;
                    case KEY_BACK:
                    home = 0; // while문에서 빠져나오기 위한 변수 -> 1번째 곡의 작곡을 멈추기 위한 것.
                    lcdtextwrite("2", "making 1th music");
                    while(1)
                    {
                        doo = 262; re = 294; mi = 330; pa = 349;
                        if((change_scale % 2) == 1) // back를 통해 각 버튼에 해당하는 음을 바꿈
                        {
                            //doo를 sol로 바꿈, re를 ra로 바꿈, mi를 si로 바꿈, pa를 소리 안나게 바꿈
                            doo = 392; re = 440; mi = 493; pa = 0;
                            ledOnOff(1, 1);
                        }
                        else
                        {
                            ledOnOff(1, 0);
                        }
                        if((change_octave % 2) == 1)
                        {
                            ledOnOff(0, 1); // 한단계 위 옥타브
                        }
                        else
                        {
                            ledOnOff(0, 0); // 기본 옥타브
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
                                    insert_last1(doo); fnd_play1_time++;
                                }
                                else
                                {
                                    insert_last1(doo * 2); fnd_play1_time++;
                                }
                                break;
                                case KEY_MENU:
                                home = 0;
                                if((change_octave % 2) == 0)
                                {
                                    insert_last1(re); fnd_play1_time++;
                                }
                                else
                                {
                                    insert_last1(re * 2); fnd_play1_time++;
                                }
                                break;
                                case KEY_VOLUMEUP:
                                home = 0;
                                if((change_octave % 2) == 0)
                                {
                                    insert_last1(mi); fnd_play1_time++;
                                }
                                else
                                {
                                    insert_last1(mi * 2); fnd_play1_time++;
                                }
                                break;
                                case KEY_VOLUMEDOWN:
                                home = 0;
                                if(pa != 0)
                                {
                                    if((change_octave % 2) == 0)
                                    {
                                        insert_last1(pa); fnd_play1_time++;
                                    }
                                    else
                                    {
                                        insert_last1(pa * 2); fnd_play1_time++;
                                    }
                                }
                            
                                break;
                            }
                        }
                    }
                    lcdtextwrite("2", "maked 1th music");
                    break;
                    case KEY_SEARCH:
                    home = 0; // while문에서 빠져나오기 위한 변수 -> 2번째 곡의 작곡을 멈추기 위한 것.
                    lcdtextwrite("2", "making 2th music");
                    while(1)
                    {
                        doo = 262; re = 294; mi = 330; pa = 349;
                        if((change_scale % 2) == 1) // back를 통해 각 버튼에 해당하는 음을 바꿈
                        {
                            //doo를 sol로 바꿈, re를 ra로 바꿈, mi를 si로 바꿈, pa를 소리 안나게 바꿈
                            doo = 392; re = 440; mi = 493; pa = 0;
                            ledOnOff(1,1);
                        }
                        else{
                            ledOnOff(1, 0);
                        }
                        if((change_octave % 2) == 1)
                        {
                            ledOnOff(0, 1); // 한단계 위 옥타브
                        }
                        else
                        {
                            ledOnOff(0, 0); // 기본 옥타브
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
                                    insert_last2(doo); fnd_play2_time++;
                                }
                                else
                                {
                                    insert_last2(doo * 2); fnd_play2_time++;
                                }
                                break;
                                case KEY_MENU:
                                home = 0;
                                if((change_octave % 2) == 0)
                                {
                                    insert_last2(re); fnd_play2_time++;
                                }
                                else
                                {
                                    insert_last2(re * 2); fnd_play2_time++;
                                }
                                break;
                                case KEY_VOLUMEUP:
                                home = 0;
                                if((change_octave % 2) == 0)
                                {
                                    insert_last2(mi); fnd_play2_time++;
                                }
                                else
                                {
                                    insert_last2(mi * 2); fnd_play2_time++;
                                }
                                break;
                                case KEY_VOLUMEDOWN:
                                home = 0;
                                if(pa != 0)
                                {
                                    if((change_octave % 2) == 0)
                                    {
                                        insert_last2(pa); fnd_play2_time++;
                                    }
                                    else
                                    {
                                        insert_last2(pa * 2); fnd_play2_time++;
                                    }
                                }
                            
                                break;
                            }
                        }
                    }
                    lcdtextwrite("2", "maked 2th music");
                    break;
                    case KEY_MENU:
                    home = 0; // while문에서 빠져나오기 위한 변수 -> 3번째 곡의 작곡을 멈추기 위한 것.
                    lcdtextwrite("2", "making 3th music");
                    while(1)
                    {
                        doo = 262; re = 294; mi = 330; pa = 349;
                        if((change_scale % 2) == 1) // back를 통해 각 버튼에 해당하는 음을 바꿈
                        {
                            //doo를 sol로 바꿈, re를 ra로 바꿈, mi를 si로 바꿈, pa를 소리 안나게 바꿈
                            doo = 392; re = 440; mi = 493; pa = 0;
                            ledOnOff(1, 1);
                        }
                        else
                        {
                            ledOnOff(1,0);
                        }
                        if((change_octave % 2) == 1)
                        {
                            ledOnOff(0, 1); // 한단계 위 옥타브
                        }
                        else
                        {
                            ledOnOff(0, 0); // 기본 옥타브
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
                                    insert_last3(doo); fnd_play3_time++;
                                }
                                else
                                {
                                    insert_last3(doo * 2); fnd_play3_time++;
                                }
                                break;
                                case KEY_MENU:
                                home = 0;
                                if((change_octave % 2) == 0)
                                {
                                    insert_last3(re); fnd_play3_time++;
                                }
                                else
                                {
                                    insert_last3(re * 2); fnd_play3_time++;
                                }
                                break;
                                case KEY_VOLUMEUP:
                                home = 0;
                                if((change_octave % 2) == 0)
                                {
                                    insert_last3(mi); fnd_play3_time++;
                                }
                                else
                                {
                                    insert_last3(mi * 2); fnd_play3_time++;
                                }
                                break;
                                case KEY_VOLUMEDOWN:
                                home = 0;
                                if(pa != 0)
                                {
                                    if((change_octave % 2) == 0)
                                    {
                                        insert_last3(pa); fnd_play3_time++;
                                    }
                                    else
                                    {
                                        insert_last3(pa * 2); fnd_play3_time++;
                                    }
                                }
                            
                                break;
                            }
                        }
                    }
                    lcdtextwrite("2", "maked 3th music");
                    break;
                    case KEY_VOLUMEUP:
                    home = 0; // while문에서 빠져나오기 위한 변수 -> 4번째 곡의 작곡을 멈추기 위한 것.
                    lcdtextwrite("2", "making 4th music");
                    while(1)
                    {
                        doo = 262; re = 294; mi = 330; pa = 349;
                        if((change_scale % 2) == 1) // back를 통해 각 버튼에 해당하는 음을 바꿈
                        {
                            //doo를 sol로 바꿈, re를 ra로 바꿈, mi를 si로 바꿈, pa를 소리 안나게 바꿈
                            doo = 392; re = 440; mi = 493; pa = 0;
                            ledOnOff(1,1);
                        }
                        else
                        {
                            ledOnOff(1, 0);
                        }
                        if((change_octave % 2) == 1)
                        {
                            ledOnOff(0, 1); // 한단계 위 옥타브
                        }
                        else
                        {
                            ledOnOff(0, 0); // 기본 옥타브
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
                                    insert_last4(doo); fnd_play4_time++;
                                }
                                else
                                {
                                    insert_last4(doo * 2); fnd_play4_time++;
                                }
                                break;
                                case KEY_MENU:
                                home = 0;
                                if((change_octave % 2) == 0)
                                {
                                    insert_last4(re); fnd_play4_time++;
                                }
                                else
                                {
                                    insert_last4(re * 2); fnd_play4_time++;
                                }
                                break;
                                case KEY_VOLUMEUP:
                                home = 0;
                                if((change_octave % 2) == 0)
                                {
                                    insert_last4(mi); fnd_play4_time++;
                                }
                                else
                                {
                                    insert_last4(mi * 2); fnd_play4_time++;
                                }
                                break;
                                case KEY_VOLUMEDOWN:
                                home = 0;
                                if(pa != 0)
                                {
                                    if((change_octave % 2) == 0)
                                    {
                                        insert_last4(pa); fnd_play4_time++;
                                    }
                                    else
                                    {
                                        insert_last4(pa * 2); fnd_play4_time++;
                                    }
                                }
                            
                                break;
                            }
                        }
                    }
                    lcdtextwrite("2", "maked 4th music");
                    break;
                    case KEY_VOLUMEDOWN:
                    home = 0; // while문에서 빠져나오기 위한 변수 -> 5번째 곡의 작곡을 멈추기 위한 것.
                    lcdtextwrite("2", "making 5th music");
                    while(1)
                    {
                        doo = 262; re = 294; mi = 330; pa = 349;
                        if((change_scale % 2) == 1) // back를 통해 각 버튼에 해당하는 음을 바꿈
                        {
                            //doo를 sol로 바꿈, re를 ra로 바꿈, mi를 si로 바꿈, pa를 소리 안나게 바꿈
                            doo = 392; re = 440; mi = 493; pa = 0;
                            ledOnOff(1,1);
                        }
                        else
                        {
                            ledOnOff(1, 0);
                        }
                        if((change_octave % 2) == 1)
                        {
                            ledOnOff(0, 1); // 한단계 위 옥타브
                        }
                        else
                        {
                            ledOnOff(0, 0); // 기본 옥타브
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
                                    insert_last5(doo); fnd_play5_time++;
                                }
                                else
                                {
                                    insert_last5(doo * 2); fnd_play5_time++;
                                }
                                break;
                                case KEY_MENU:
                                home = 0;
                                if((change_octave % 2) == 0)
                                {
                                    insert_last5(re); fnd_play5_time++;
                                }
                                else
                                {
                                    insert_last5(re * 2); fnd_play5_time++;
                                }
                                break;
                                case KEY_VOLUMEUP:
                                home = 0;
                                if((change_octave % 2) == 0)
                                {
                                    insert_last5(mi); fnd_play5_time++;
                                }
                                else
                                {
                                    insert_last5(mi * 2); fnd_play5_time++;
                                }
                                break;
                                case KEY_VOLUMEDOWN:
                                home = 0;
                                if(pa != 0)
                                {
                                    if((change_octave % 2) == 0)
                                    {
                                        insert_last5(pa); fnd_play5_time++;
                                    }
                                    else
                                    {
                                        insert_last5(pa * 2); fnd_play5_time++;
                                    }
                                }
                            
                                break;
                            }
                        }
                    }
                    lcdtextwrite("2", "maked 5th music");
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

        if(home_for_makeSong == 2 && back_for_PlaySong == 2) //프로그램 종료
        {
            break;
        }
    }

    
    lcdtextwrite("1", "The End");
    sleep(3);
    lcd_exit();
    ledLibExit();
    buzzerExit();
    buttonExit();

    return 0;
}