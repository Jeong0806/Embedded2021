#include <stdio.h>
#include <stdlib.h>
#include "led.h"

int main(void)
{
    ledLibinit();

    ledLibRaw(0xFF);
    sleep(1);
    ledLibRaw(0xAA);
    sleep(1);
    ledLibRaw(0x55);
    sleep(2);
    ledLibExit();

    return 0;
}