/**
 ******************************************************************************
 * @file    Log.c
 * @author  jkeyvv
 * @version V1.0.0
 * @date    2021-12-2
 * @brief   Log module
 ******************************************************************************
 **/
 

#include "Log.h"
#include "Uart.h"
#include "RingBuf.h"
#include "SysTick.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define MS_ONE_DAY      (86400000) 	                        //1天毫秒数24*60*60*1000 
#define MS_ONE_HOUR     (3600000)	                        //1小时毫秒数1000*60*60
#define MS_ONE_MINUTE   (60000)		                        //1分钟毫秒数1000*60
                            
static uint32_t LocalTimeMs;                                //上位机发送时间保存
static uint32_t LocalTickCount;	                            //上位机发送时间时本地计数
RingBufTypeDef *LogBuf;                                     //Log缓冲区

#ifdef LOG_UART
volatile _Bool Uart_TxOver = true;                          //UART发送完成标志位
#endif


static void GetCurrentTime(uint8_t *Hour, uint8_t *Minute, uint8_t *Second, uint16_t *Millisecond)
{
    uint32_t TickCount = SysTick_GetTickCount();
    
    /* 一天内的时间，不连续开49天（没做溢出处理） */
	uint32_t AllTimeMs = (TickCount - LocalTickCount + LocalTimeMs) % MS_ONE_DAY;
	*Hour = AllTimeMs / MS_ONE_HOUR;
	*Minute = AllTimeMs / MS_ONE_MINUTE % 60;
	*Second = AllTimeMs / 1000 % 60;
	*Millisecond = AllTimeMs % 1000;
}

void SetCurrentTime(u32 TimeMs)
{
    LocalTimeMs = TimeMs;
	LocalTickCount = SysTick_GetTickCount();
}

/** 
 * @brief  日志打印初始化
 * @param  format 
 * @return void
 */
void Log_Init(void)
{
    /* 创建日志缓冲区 */
    LogBuf = RingBuf_Create(LOG_BUF_SIZE);
    if (LogBuf == NULL)
    {
        Log_Error("Log Buf Create Error\r\n");
        return;
    }
#ifdef LOG_UART
    Uart1_Init(UART1_BAUD_RATE);
    Uart1_TxDMA_Init();
#endif
}

/** 
 * @brief  Log底层实现
 * @param  format 
 * @return void
 */
void MyLog(LogLevelTypeDef Level, const char *File, int Line, const char *Format, ...)
{
	char Buf[LOG_MESSAGE_LENGTH];
	char Log[LOG_MAX_LENGTH];
    
    uint8_t LogLen = 0;
    uint8_t DiscardLogLen = 0;
    uint8_t LogDiscard[LOG_MAX_LENGTH];
    
    /* 提取不定长参数到缓冲区 */
	va_list Args;
	va_start(Args, Format);
	vsprintf(Buf, Format, Args);
	va_end(Args);

    /* 获取当前系统时间 */
	uint8_t Hour, Minute, Second = 0;
	uint16_t Millisecond = 0;
	GetCurrentTime(&Hour, &Minute, &Second, &Millisecond);
    
    switch (Level)
    {   
        case LOG_LEVEL_FATAL:
            sprintf(Log, "[%02d:%02d:%02d.%03d][FATAL]: %s",
                     Hour, Minute, Second, Millisecond, Buf);
            break;
        case LOG_LEVEL_ERROR:
            sprintf(Log, "[%02d:%02d:%02d.%03d][ERROR]: %s",
                     Hour, Minute, Second, Millisecond, Buf);
            break;
        case LOG_LEVEL_WARN:
            sprintf(Log, "[%02d:%02d:%02d.%03d][WARN]: %s",
                     Hour, Minute, Second, Millisecond, Buf);
            break;
        case LOG_LEVEL_INFO:
            sprintf(Log, "[%02d:%02d:%02d.%03d][INFO]: %s",
                     Hour, Minute, Second, Millisecond, Buf);
            break;
        case LOG_LEVEL_DEBUG:
            sprintf(Log, "[%02d:%02d:%02d.%03d][DEBUG]: %s",
                     Hour, Minute, Second, Millisecond, Buf);
            break;
        case LOG_LEVEL_NONE:
            sprintf(Log, "%s", Buf);
            break;
        default:
            break;
    }
    
    /* 计算所需的缓冲区空间（1字节长度 + LOG长度）*/
    if (strlen(Log) >= LOG_MAX_LENGTH)
    {
        Log[LOG_MAX_LENGTH - 1] = '\0';
        LogLen = LOG_MAX_LENGTH;
        Log_Error("Log is too leng, and has been truncated\r\n");
    }
    else
        LogLen = strlen(Log) + 1;
    
    /* 当缓冲区空间不足时，为新的LOG腾出空间 */
    while (RingBuf_GetRemain(LogBuf) < LogLen)
    {
        if (RingBuf_PopByte(LogBuf, &DiscardLogLen) != 0 || RingBuf_Pop(LogBuf, LogDiscard, DiscardLogLen) != 0)
            Log_Error("Log Discard Error\r\n");
    }
    
    if (RingBuf_PushByte(LogBuf, LogLen) != 0 || RingBuf_Push(LogBuf, (uint8_t *)Log, LogLen) != 0)
        Log_Error("Log buf push error\r\n");
}

void Log_Print(void)
{
#ifdef LOG_UART
    /* 当没有协议包需要上发时，处理主板Log */
    if(Uart_TxOver == true && RingBuf_GetLength(LogBuf) > 0)
    {
        uint8_t LogLen = 0;
        static uint8_t Log[LOG_MAX_LENGTH];
        if (RingBuf_PopByte(LogBuf, &LogLen) != 0 || RingBuf_Pop(LogBuf, Log, LogLen) != 0)
        {
            Log_Error("Log Buf Pop Error\r\n");
            return;
        }

        if (Uart1_DMA_Send((char*)Log, LogLen) != 0)
        {
            printf("Log Len: %d, Log: %s", LogLen, Log);
        };
        
        Uart_TxOver = false;
    }
#endif
}

/* 串口 1 发送完成处理函数 */
void Uart1_TxOverProcess(void)
{
    Uart_TxOver = true;
}

// 重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR & 0X40) == 0);          //循环发送,直到发送完毕   
	USART1->DR = (uint8_t)ch;      
	return ch;
}
