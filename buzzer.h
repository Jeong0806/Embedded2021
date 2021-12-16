#ifndef _BUZZER_H_
#define _BUZZER_H_

typedef int element;

typedef struct ListNode {
    element data;
    struct ListNode *link;
}ListNode;

int buzzerInit(void);
int buzzerExit(void);

ListNode *insert_last1(element data);
ListNode *insert_last2(element data);
ListNode *insert_last3(element data);
ListNode *insert_last4(element data);
ListNode *insert_last5(element data);

void buzzerEnable(int bEnable);
void setFrequency(int frequency);
void print_list(int num);
int empty_number_of_song(void);
int list_number_of_song(void);
void delete_song(int num);

#endif