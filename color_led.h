#ifndef _COLOR_LED_H_
#define _COLOR_LED_H_

int pwmActiveAll(void);
int pwmInactiveAll(void);
int pwmSetDuty(int dutyCycle, int pwmIndex);
int pwmSetPeriod(int Period, int pwmIndex);
int pwmSetPercent(int percent, int ledColor);
int pwmStartAll(void);
int pwmLedInit(void);
void color_red(void);
void color_green(void);
void color_blue(void);
void color_off(void);

#endif