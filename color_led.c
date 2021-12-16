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
#include "color_led.h"

#define COLOR_LED_DEN_R_ "/sys/class/pwm/pwmchip0/"
#define COLOR_LED_DEN_G_ "/sys/class/pwm/pwmchip1/"
#define COLOR_LED_DEN_B_ "/sys/class/pwm/pwmchip2/"

#define PWM_EXPORT "export"
#define PWM_UNEXPORT "unexport"
#define PWM_DUTY "pwm0/duty_cycle"
#define PWM_PERIOD "pwm0/period"
#define PWM_ENABLE "pwm0/enable"

#define PWM_COLOR_R 0
#define PWM_COLOR_G 1
#define PWM_COLOR_B 2

#define PWM_PERIOD_NS 1000000 // 1000ns -> 1kHz

static int f2 = 0;

int pwmActiveAll(void)
{
    f2 = 0;
    f2 = open(COLOR_LED_DEN_R_ PWM_EXPORT, O_WRONLY);
    write(f2, &"0", 1);
    close(f2);

    f2 = open(COLOR_LED_DEN_G_ PWM_EXPORT, O_WRONLY);
    write(f2, &"0", 1);
    close(f2);

    f2 = open(COLOR_LED_DEN_B_ PWM_EXPORT, O_WRONLY);
    write(f2, &"0", 1);
    close(f2);

    return 1;
}

int pwmInactiveAll(void)
{
    f2 = 0;
    f2 = open(COLOR_LED_DEN_R_ PWM_UNEXPORT, O_WRONLY);
    write(f2, &"0", 1);
    close(f2);

    f2 = open(COLOR_LED_DEN_G_ PWM_UNEXPORT, O_WRONLY);
    write(f2, &"0", 1);
    close(f2);

    f2 = open(COLOR_LED_DEN_B_ PWM_UNEXPORT, O_WRONLY);
    write(f2, &"0", 1);
    close(f2);

    return 1;
}

int pwmSetDuty(int dutyCycle, int pwmIndex)
{
    f2 = 0;
    switch(pwmIndex)
    {
        case 0:
        f2 = open(COLOR_LED_DEN_R_ PWM_DUTY, O_WRONLY);
        break;
        case 1:
        f2 = open(COLOR_LED_DEN_G_ PWM_DUTY, O_WRONLY);
        break;
        case 2:
        f2 = open(COLOR_LED_DEN_B_ PWM_DUTY, O_WRONLY);
        break;
    }
    dprintf(f2, "%d", dutyCycle);
    close(f2);
    return 1;
}

int pwmSetPeriod(int Period, int pwmIndex)
{
    f2 = 0;
    switch(pwmIndex)
    {
        case 0:
        f2 = open(COLOR_LED_DEN_R_ PWM_PERIOD, O_WRONLY);
        break;
        case 1:
        f2 = open(COLOR_LED_DEN_G_ PWM_PERIOD, O_WRONLY);
        break;
        case 2:
        f2 = open(COLOR_LED_DEN_B_ PWM_PERIOD, O_WRONLY);
        break;
    }
    dprintf(f2, "%d", Period);
    close(f2);
    return 1;
}

int pwmSetPercent(int percent, int ledColor)
{
    if((percent < 0) || (percent > 100))
    {
        printf("Wrong percent: %d\r\n", percent);
        return 0;
    }

    int duty = (100 - percent) * PWM_PERIOD_NS / 100;
    pwmSetDuty(duty, ledColor);
    return 0;
}

void color_red(void)
{

    int duty = (100 - 0) * PWM_PERIOD_NS / 100;
    pwmSetDuty(0, 0); pwmSetDuty(duty, 1); pwmSetDuty(duty, 2);  
    
}

void color_green(void)
{

    int duty = (100 - 0) * PWM_PERIOD_NS / 100;
    pwmSetDuty(duty, 0); pwmSetDuty(0, 1); pwmSetDuty(duty, 2);  
    
}

void color_blue(void)
{

    int duty = (100 - 0) * PWM_PERIOD_NS / 100;
    pwmSetDuty(duty, 0); pwmSetDuty(duty, 1); pwmSetDuty(0, 2);  
    
}

void color_off(void)
{
    int duty = (100 - 0) * PWM_PERIOD_NS / 100;
    pwmSetDuty(duty, 0); pwmSetDuty(duty, 1); pwmSetDuty(duty, 2);
}



int pwmStartAll(void)
{
    f2 = 0;
    f2 = open(COLOR_LED_DEN_R_ PWM_ENABLE, O_WRONLY);
    write(f2, &"1", 1);
    close(f2);

    f2 = open(COLOR_LED_DEN_G_ PWM_ENABLE, O_WRONLY);
    write(f2, &"1", 1);
    close(f2);

    f2 = open(COLOR_LED_DEN_B_ PWM_ENABLE, O_WRONLY);
    write(f2, &"1", 1);
    close(f2);

    return 1;
}

int pwmLedInit(void)
{
    pwmActiveAll();
    pwmSetDuty(0, 0); pwmSetDuty(0, 1); pwmSetDuty(0, 2);
    pwmSetPeriod(PWM_PERIOD_NS, 0); pwmSetPeriod(PWM_PERIOD_NS, 1); pwmSetPeriod(PWM_PERIOD_NS, 2);
    pwmStartAll(); 
}