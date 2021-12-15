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

#define TEXTLCD_DRIVER_NAME "/dev/peritextlcd"

#define LINE_NUM 2
#define COLUMN_NUM 16
#define LINE_BUFF_NUM (COLUMN_NUM + 4)
#define MODE_CMD 0
#define MODE_DATA 1

typedef struct TextLCD_tag{
    unsigned char cmd;
    unsigned char cmdData;
    unsigned char reserved[2];
    char TextData[LINE_NUM][COLUMN_NUM + 4];
}stTextLCD, *pStTextLCD;

#define CMD_DISPLAY_MODE 0x10
#define BIT_DISPLAY_MODE_CURSOR_DISP 0x01
#define BIT_DISPLAY_MODE_CURSOR_BLINK 0x02
#define BIT_DISPLAY_MODE_DISP_ENABLE 0x04

#define CMD_CURSOR_MOVE_MODE 0x11
#define CURSOR_MOVE_ENABLE 0x01
#define CURSOR_MOVE_MODE_RIGHT_DIR 0x02

#define CMD_CURSOR_MOVE_POSITION 0x12
#define CURSOR_MOVE_X_MASK 0x3F
#define CURSOR_MOVE_Y_MASK 0xC0
    
#define CMD_WRITE_STRING 0x20
    #define CMD_DATA_WIRTE_BOTH_LINE 0
    #define CMD_DATA_WRITE_LINE_1 0x01
    #define CMD_DATA_WRITE_LINE_2 0x02

#define CMD_TEST_GPIO_HIGH 0x30
#define CMD_TEST_GPIO_LOW 0x31

static int fd1 = 0;

int lcdtextwrite(const char *str1, const char *str2)
{
    unsigned int linenum = 0;
    stTextLCD stlcd;
    int len;

    memset(&stlcd, 0, sizeof(stTextLCD));
    
    linenum = strtol(str1, NULL, 10);

    if(linenum == 1)
    {
        printf("1th line\n");
        stlcd.cmdData = CMD_DATA_WRITE_LINE_1;
    }
    else if(linenum == 2)
    {
        printf("2th line\n");
        stlcd.cmdData = CMD_DATA_WRITE_LINE_2;
    }
    else
    {
        printf("plz write 1~2\n");
        return 1;
    }

    len = strlen(str2);
    if(len > COLUMN_NUM)
    {
        memcpy(stlcd.TextData[stlcd.cmdData - 1], str2, COLUMN_NUM);
    }
    else
    {
        memcpy(stlcd.TextData[stlcd.cmdData - 1], str2, len);
    }

    stlcd.cmd = CMD_WRITE_STRING;
    fd1 = open(TEXTLCD_DRIVER_NAME, O_RDWR);
    if(fd1 < 0)
    {
        printf("/dev/peritextlcd open error\n");
        return -1;
    }
    write(fd1, &stlcd, sizeof(stTextLCD));
    close(fd1);
    printf("%s\n", stlcd.TextData[linenum -1]);
    return linenum;
}

int check_open = 0; // 비어있는 곡을 lcd 화면에 출력하고 지워지지 않게 하기 위해  파일을 한 번만 열기 위한 변수.
stTextLCD stlcd;

void lcd_set(void)
{
    memset(&stlcd, 0, sizeof(stTextLCD));
}

int lcdtextoverwrite(int num, const char *str1, const char *str2) //작곡할 때 몇 번이 비어있는지 알려주기 위해..
{                                                                 //비어있는 곡이 lcd에서 num칸 만큼 이동하여 표시되게 함.
    unsigned int linenum = 0;
    stTextLCD stlcd;
    int len;

    
    
    linenum = strtol(str1, NULL, 10);

    if(linenum == 1)
    {
        printf("1th line\n");
        stlcd.cmdData = CMD_DATA_WRITE_LINE_1;
    }
    else if(linenum == 2)
    {
        printf("2th line\n");
        stlcd.cmdData = CMD_DATA_WRITE_LINE_2;
    }
    else
    {
        printf("plz write 1~2\n");
        return 1;
    }

    len = strlen(str2);
    if(len > COLUMN_NUM)
    {
        memcpy(&stlcd.TextData[stlcd.cmdData - 1][num], str2, COLUMN_NUM);
    }
    else
    {
        memcpy(&stlcd.TextData[stlcd.cmdData - 1][num], str2, len);
    }

    stlcd.cmd = CMD_WRITE_STRING;
    if(check_open == 0)
    {
        fd1 = open(TEXTLCD_DRIVER_NAME, O_RDWR);
    }
    check_open++;
    if(fd1 < 0)
    {
        printf("/dev/peritextlcd open error\n");
        return -1;
    }
    write(fd1, &stlcd, sizeof(stTextLCD));
    printf("%s\n", stlcd.TextData[linenum -1]);
    return linenum;
}

void lcd_close(void) // buzzer.c에서 lcd 파일을 닫기 위해..
{
    check_open = 0;
    close(fd1);
}