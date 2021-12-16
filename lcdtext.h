#ifndef _LCD_TEXT_H_
#define _LCD_TEXT_H_


int lcdtextwrite(const char *str1, const char *str2);
void lcd_set(void);
int lcdtextoverwrite(int num, const char *str1, const char *str2); //작곡할 때 몇 번이 비어있는지 알려주기 위해.. 
                                                                   //비어있는 곡이 lcd에서 num칸 만큼 이동하여 표시되게 함.
void lcd_close(void); // buzzer.c에서 lcd 파일을 닫기 위해..
void lcd_exit(void);

#endif