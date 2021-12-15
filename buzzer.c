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
#include "lcdtext.h"

#define MAX_SCALE_STEP 8
#define BUZZER_BASE_SYS_PATH "/sys/bus/platform/devices"
#define BUZZER_FILENAME "peribuzzer"
#define BUZZER_ENABLE_NAME "enable"
#define BUZZER_FREQUENCY_NAME "frequency"
char gBuzzerBaseSysDir[128]; //  /sys/bus/platform/devices/peribuzzer.숫자 가 결정됨

static int fp = 0;


typedef int element;

typedef struct ListNode {
    element data;
    struct ListNode *link;
}ListNode;

ListNode *head1 = NULL;
ListNode *head2 = NULL;
ListNode *head3 = NULL;
ListNode *head4 = NULL;
ListNode *head5 = NULL;

ListNode *insert_last1(element data)
{
    ListNode *node = (ListNode *)malloc(sizeof(ListNode));
    node->data = data;

    if(head1 == NULL)
    {
        head1 = node;
        node->link = head1;
    }
    else
    {
        node->link = head1->link;
        head1->link = node;
        head1 = node;
    }

    return head1;
}

ListNode *insert_last2(element data)
{
    ListNode *node = (ListNode *)malloc(sizeof(ListNode));
    node->data = data;

    if(head2 == NULL)
    {
        head2 = node;
        node->link = head2;
    }
    else
    {
        node->link = head2->link;
        head2->link = node;
        head2 = node;
    }

    return head2;
}

ListNode *insert_last3(element data)
{
    ListNode *node = (ListNode *)malloc(sizeof(ListNode));
    node->data = data;

    if(head3 == NULL)
    {
        head3 = node;
        node->link = head3;
    }
    else
    {
        node->link = head3->link;
        head3->link = node;
        head3 = node;
    }

    return head3;
}

ListNode *insert_last4(element data)
{
    ListNode *node = (ListNode *)malloc(sizeof(ListNode));
    node->data = data;

    if(head4 == NULL)
    {
        head4 = node;
        node->link = head4;
    }
    else
    {
        node->link = head4->link;
        head4->link = node;
        head4 = node;
    }

    return head4;
}

ListNode *insert_last5(element data)
{
    ListNode *node = (ListNode *)malloc(sizeof(ListNode));
    node->data = data;

    if(head5 == NULL)
    {
        head5 = node;
        node->link = head5;
    }
    else
    {
        node->link = head5->link;
        head5->link = node;
        head5 = node;
    }

    return head5;
}


const int musicScale1[MAX_SCALE_STEP] = 
{
    /*do*/262, 294, 330, 349, 392, 440, 494, 523/*si*/
};

const int musicScale2[MAX_SCALE_STEP] = 
{
    /*do*/523, 294, 330, 349, 392, 440, 494, 523/*si*/
};

const int musicScale3[MAX_SCALE_STEP] = 
{
    /*do*/494, 294, 330, 349, 392, 440, 494, 523/*si*/
};

const int musicScale4[MAX_SCALE_STEP] = 
{
    /*do*/440, 294, 330, 349, 392, 440, 494, 523/*si*/
};

const int musicScale5[MAX_SCALE_STEP] = 
{
    /*do*/392, 294, 330, 349, 392, 440, 494, 523/*si*/
};

int findBuzzerSysPath(){ //못찾으면 ifNotFound -> 1을 리턴
    DIR *dir_info = opendir(BUZZER_BASE_SYS_PATH);
    int ifNotFound = 1;
    if(dir_info != NULL)
    {
        while(1)
        {
            struct dirent *dir_entry;
            dir_entry = readdir(dir_info);
            if(dir_entry == NULL) break;
            if(strncasecmp(BUZZER_FILENAME, dir_entry->d_name, strlen(BUZZER_FILENAME)) == 0)
            {
                ifNotFound = 0;
                sprintf(gBuzzerBaseSysDir, "%s/%s/", BUZZER_BASE_SYS_PATH, dir_entry->d_name);
                printf("find %s\n", gBuzzerBaseSysDir);
                return ifNotFound;
            }
        }
    }
    printf("Cannot Found\n");
    return ifNotFound;
}

void doHelp(void)
{
    printf("Usage:\n");
    printf("buzzertest <buzzerN> \n");
    printf("buzzerNo: \n");
    printf("do(1),re(2),mi(3),fa(4),sol(5),ra(6),si(7),do(8) \n");
    printf("off(0)\n");
}

void buzzerEnable(int bEnable)
{
    char path[200];
    sprintf(path, "%s%s", gBuzzerBaseSysDir, BUZZER_ENABLE_NAME);
    int fd = open(path, O_WRONLY);
    printf("%s\n", path);
    if(bEnable)
    {
        write(fd, &"1", 1);
        printf("echo 1 > enable\n");
    }
    else
    {
        write(fd, &"0", 1);
        printf("echo 0 > enable\n");
    }
    close(fd);
}

void setFrequency(int frequency)
{
    char path[200];
    sprintf(path, "%s%s", gBuzzerBaseSysDir, BUZZER_FREQUENCY_NAME);
    int fd = open(path, O_WRONLY);
    dprintf(fd, "%d", frequency);
    close(fd);
}

int buzzerInit(void)
{
    if(findBuzzerSysPath() == 1)
    {
        printf("error!");
        return 0;
    }
    printf("buzzerPath: %s\n", gBuzzerBaseSysDir);
    
    buzzerEnable(1);
    return 1;
}

int buzzerPlaySong(int scale)
{
    char path[200];
    sprintf(path, "%s%s", gBuzzerBaseSysDir, BUZZER_FREQUENCY_NAME);
    fp = open(path, O_WRONLY);
    printf("%s\n", path);
    int i = 0;
    switch(scale)
    {
        case 1:
        while(i != 8)
        {
            
            dprintf(fp, "%d", musicScale1[i++]);
            sleep(1);
        }
        buzzerEnable(0);
        break;
        case 2:
        while(i != 8)
        {
            dprintf(fp, "%d", musicScale2[i++]);
            sleep(1);
        }
        buzzerEnable(0);
        break;
        case 3:
        while(i != 8)
        {
            dprintf(fp, "%d", musicScale3[i++]);
            sleep(1);
        }
        buzzerEnable(0);
        break;
        case 4:
        while(i != 8)
        {
            dprintf(fp, "%d", musicScale4[i++]);
            sleep(1);
        }
        buzzerEnable(0);
        break;
        case 5:
        while(i != 8)
        {
            dprintf(fp, "%d", musicScale5[i++]);
            sleep(1);
        }
        buzzerEnable(0);
        break;
    }
    
    return 1;
}

void print_list(int num)
{
    ListNode *p;
    if(num == 1)
    {
        if(head1 == NULL)
        {
            printf("1번에 저장된 곡이 없습니다.\n");
        }
        else
        {
            char path[200];
            sprintf(path, "%s%s", gBuzzerBaseSysDir, BUZZER_FREQUENCY_NAME);
            fp = open(path, O_WRONLY);

            for(p = head1->link; p != head1; p = p->link)
            {
                dprintf(fp, "%d", p->data);
                sleep(1);
            }
            buzzerEnable(0);
        }
    }
    else if(num == 2)
    {
        if(head2 == NULL)
        {
            printf("2번에 저장된 곡이 없습니다.\n");
        }
        else
        {
            char path[200];
            sprintf(path, "%s%s", gBuzzerBaseSysDir, BUZZER_FREQUENCY_NAME);
            fp = open(path, O_WRONLY);

            for(p = head2->link; p != head2; p = p->link)
            {
                dprintf(fp, "%d", p->data);
                sleep(1);
            }
            buzzerEnable(0);
        }
    }
    else if(num == 3)
    {
        if(head3 == NULL)
        {
            printf("3번에 저장된 곡이 없습니다.\n");
        }
        else
        {
            char path[200];
            sprintf(path, "%s%s", gBuzzerBaseSysDir, BUZZER_FREQUENCY_NAME);
            fp = open(path, O_WRONLY);

            for(p = head3->link; p != head3; p = p->link)
            {
                dprintf(fp, "%d", p->data);
                sleep(1);
            }
            buzzerEnable(0);
        }
    }
    else if(num == 4)
    {
        if(head4 == NULL)
        {
            printf("4번에 저장된 곡이 없습니다.\n");
        }
        else
        {
            char path[200];
            sprintf(path, "%s%s", gBuzzerBaseSysDir, BUZZER_FREQUENCY_NAME);
            fp = open(path, O_WRONLY);

            for(p = head4->link; p != head4; p = p->link)
            {
                dprintf(fp, "%d", p->data);
                sleep(1);
            }
            buzzerEnable(0);
        }
    }
    else if(num == 5)
    {
        if(head5 == NULL)
        {
            printf("5번에 저장된 곡이 없습니다.\n");
        }
        else
        {
            char path[200];
            sprintf(path, "%s%s", gBuzzerBaseSysDir, BUZZER_FREQUENCY_NAME);
            fp = open(path, O_WRONLY);

            for(p = head5->link; p != head5; p = p->link)
            {
                dprintf(fp, "%d", p->data);
                sleep(1);
            }
            buzzerEnable(0);
        }
    }
    buzzerEnable(0);
}

int empty_number_of_song(void)
{
    lcd_set();
    lcdtextoverwrite(0, "2", "empty:         "); // 15칸 띄운 이유 -> 이렇게 하지 않으면 \0 에 의해 문장이 끝났다고 판단되어 lcd에 출력이 안됨
    int num = 0;
    if(head1 == NULL)
    {
        lcdtextoverwrite(7, "2", "1");
    }

    if(head2 == NULL)
    {
        lcdtextoverwrite(9, "2", "2");
    }

    if(head3 == NULL)
    {
        lcdtextoverwrite(11, "2", "3");
    }

    if(head4 == NULL)
    {
        lcdtextoverwrite(13, "2", "4");
    }

    if(head5 == NULL)
    {
        lcdtextoverwrite(15, "2", "5");
    }


    lcd_close();

    return num;
}

int list_number_of_song(void)
{
    lcd_set();
    lcdtextoverwrite(0, "2", "list:          "); // 15칸 띄운 이유 -> 이렇게 하지 않으면 \0 에 의해 문장이 끝났다고 판단되어 lcd에 출력이 안됨
    int num = 0;
    if(head1 != NULL)
    {
        lcdtextoverwrite(7, "2", "1");
        num++;
    }

    if(head2 != NULL)
    {
        lcdtextoverwrite(9, "2", "2");
        num++;
    }

    if(head3 != NULL)
    {
        lcdtextoverwrite(11, "2", "3");
        num++;
    }

    if(head4 != NULL)
    {
        lcdtextoverwrite(13, "2", "4");
        num++;
    }

    if(head5 != NULL)
    {
        lcdtextoverwrite(15, "2", "5");
        num++;
    }

    //num 만큼 fnd에 띄우기.

    lcd_close();

    return num;
}

int buzzerStopSong(void)
{
    printf("stop song\n");
    close(fp);
    return 1;
}

void delete_song(int num)
{
    if(num == 1)
    {
        head1 = NULL;
    }
    else if(num == 2)
    {
        head2 = NULL;
    }
    else if(num == 3)
    {
        head3 = NULL;
    }
    else if(num == 4)
    {
        head4 = NULL;
    }
    else if(num == 5)
    {
        head5 = NULL;
    }
}

int buzzerExit(void)
{
    buzzerEnable(0);
    return 1;
}