#ifndef __DELAY_H
#define __DELAY_H

/* ----- DWT 延时 ----- */
void Delay_DWT_Init(void);
void Delay_DWT_US(unsigned int US);
void Delay_DWT_MS(unsigned int MS);

/* ----- Tick 延时 ----- 使用 FreeRTOS 不能使用 SysTick定时器 */
//void Delay_SysTick_Init(unsigned char clockMHz);
//void Delay_SysTick_US(unsigned int US);
//void Delay_SysTick_MS(unsigned int MS);

/* ----- FreeRTOS 操作系统延时 ----- */
void Delay_System_Init(void);
void Delay_System_US(unsigned int US);
void Delay_System_MS(unsigned int MS);

#endif /* << _DELAY_H_ */


