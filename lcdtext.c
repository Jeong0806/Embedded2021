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

int lcdtextwrite(const char *str1, const char *str2)
{
    unsigned int linenum = 0;
    stTextLCD stlcd;
    int fd, len;

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
    fd = open(TEXTLCD_DRIVER_NAME, O_RDWR);
    if(fd < 0)
    {
        printf("/dev/peritextlcd open error\n");
        return -1;
    }
    write(fd, &stlcd, sizeof(stTextLCD));
    close(fd);
    printf("%s\n", stlcd.TextData[linenum -1]);
    return linenum;
}