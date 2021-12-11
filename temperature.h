#ifndef _TEM_H_
#define _TEM_H_

char *spi_read_lm74(int file);
void print_temp(void);
int spi_init(char filename[40]);

#endif