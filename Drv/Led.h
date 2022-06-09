#ifndef __LED_H
#define __LED_H

#include "stm32f4xx.h"

typedef enum
{
    LED_1 = 0,
}LedTypeDef;

void Led_Init(void);
void Led_On(LedTypeDef Led);
void Led_Off(LedTypeDef Led);
void Led_Toggle(LedTypeDef Led);

#endif

