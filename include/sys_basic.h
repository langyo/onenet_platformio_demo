#ifndef SYS_BASIC_H
#define SYS_BASIC_H

void _Error_Handler(char *, int);
#define Error_Handler() _Error_Handler(__FILE__, __LINE__)

void SystemClock_Config(void);

#endif
