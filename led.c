#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "led.h"

static int fd = 0;
static int ledValue = 0;

int ledLibinit(void)
//이 함수는 led를 켜기 전에 반드시 한 번 불러주어야 한다.
{
    fd = open(LED_DRIVER_NAME, O_RDWR);
    if(fd < 0)
    {
        printf("Device open Error\n");
        exit(1);
    }
}
int ledOnOff(int ledNum, int onOff)
//이 함수는 ledNum에 0~7을 넣으면, 그에 해당하는 led를 켜거나 끄거나 한다.
{
    int i = 1;
    i = i<<ledNum;
    ledValue = ledValue & (~i);
    if(onOff != 0)
    {
        ledValue != i;
    }
    ledLibRaw(ledValue);

    return 1;
}
int ledStatus(void)
//이 함수를 호출하면 현재 led상태가 어떤지 확인해준다.
//만약 모든 led가 켜져 있으면 0xff를 리턴하고,
//모든 led가 꺼져 있으면 0x00을 리턴한다.
//0xAA(10101010)이 리턴되면, led는 4개만 켜져 있는 것이다.
{
    return ledValue;
}
int ledLibExit(void)
//이 함수는 led를 전부 끄고 driver를 close한다.
{
    ledOnOff(0,0);
    close(fd);
}
int ledLibRaw(char led_status)
//이 함수를 호출하면 led_status에 맞게 8개의 led를 전부 제어한다.
//예를 들어 ledLibRaw(0x80);이면 led 1개만 켜진다. 
{
    if(fd > 0)
    {
        write(fd, &led_status, 1);
    }
    else
    {
        printf("Wrong! must open device node\n");
        exit(2);
    }

    ledValue = led_status;
    return 1;
}
int ledLibBlink(int ledNum, int nth, int msec)
//1~8까지의 Led를 하나 골라서 n번 깜빡이는데, 주기가 msec이다.
{
    for(int i = 0; i < nth; i++)
    {
        ledOnOff(ledNum, 1);   
        usleep(1000*msec);
        ledOnOff(ledNum, 0);
        usleep(1000*msec);
    }
}