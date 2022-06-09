#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f4xx.h"

uint8_t SysTick_Init(uint32_t PreemptPriority, uint32_t SubPriority);
uint32_t SysTick_GetTickCount(void);
void SysTick_IRQ_Callback(void);

#endif
