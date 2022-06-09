#ifndef __LOG_H
#define __LOG_H

#include "RingBuf.h"

#define LOG_UART
#define LOG_BUF_SIZE    4096
#define LOG_LEVEL 		LOG_LEVEL_DEBUG

#define LOG_MESSAGE_LENGTH 	    (200)                           //�û��������Ϣ����
#define LOG_MAX_LENGTH   	    (LOG_MESSAGE_LENGTH + 50)       //������ӡ��Ϣ����

#define Log(Format, ...)  MyLog(LOG_LEVEL_NONE, __FILE__, __LINE__, Format, ##__VA_ARGS__)

extern RingBufTypeDef *LogBuf;                                     //Log������

typedef enum
{
    LOG_LEVEL_FATAL = 0,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_NONE
}LogLevelTypeDef;

#if (LOG_LEVEL == LOG_LEVEL_DEBUG)
    #define Log_Fatal(Format, ...)   MyLog(LOG_LEVEL_FATAL, __FILE__, __LINE__, Format, ##__VA_ARGS__)
    #define Log_Error(Format, ...)   MyLog(LOG_LEVEL_ERROR, __FILE__, __LINE__, Format, ##__VA_ARGS__)
    #define Log_Warn(Format, ...)    MyLog(LOG_LEVEL_WARN, __FILE__, __LINE__, Format, ##__VA_ARGS__)
    #define Log_Info(Format, ...)    MyLog(LOG_LEVEL_INFO, __FILE__, __LINE__, Format, ##__VA_ARGS__)
    #define Log_Debug(Format, ...)   MyLog(LOG_LEVEL_DEBUG, __FILE__, __LINE__, Format, ##__VA_ARGS__)

#elif (LOG_LEVEL == LOG_LEVEL_INFO) 
    #define Log_Fatal(Format, ...)   MyLog(LOG_LEVEL_FATAL, __FILE__, __LINE__, Format, ##__VA_ARGS__)
    #define Log_Error(Format, ...)   MyLog(LOG_LEVEL_ERROR, __FILE__, __LINE__, Format, ##__VA_ARGS__)
    #define Log_Warn(Format,  ...)   MyLog(LOG_LEVEL_WARN, __FILE__, __LINE__, Format ,##__VA_ARGS__)
    #define Log_Info(Format, ...)    MyLog(LOG_LEVEL_INFO, __FILE__, __LINE__, Format, ##__VA_ARGS__)
    #define Log_Debug(Format, ...)

#elif (LOG_LEVEL == LOG_LEVEL_WARN)
    #define Log_Fatal(Format, ...)   MyLog(LOG_LEVEL_FATAL, __FILE__, __LINE__, Format, ##__VA_ARGS__)
    #define Log_Error(Format, ...)   MyLog(LOG_LEVEL_ERROR, __FILE__, __LINE__, Format, ##__VA_ARGS__)
    #define Log_Warn(Format, ...)    MyLog(LOG_LEVEL_WARN, __FILE__, __LINE__, Format, ##__VA_ARGS__)
    #define Log_Info(Format, ...)
    #define Log_Debug(Format, ...)

#elif (LOG_LEVEL == LOG_LEVEL_ERROR)
    #define Log_Fatal(Format, ...)   MyLog(LOG_LEVEL_FATAL, __FILE__, __LINE__, Format, ##__VA_ARGS__)
    #define Log_Error(Format, ...)   MyLog(LOG_LEVEL_ERROR, __FILE__, __LINE__, Format, ##__VA_ARGS__)
    #define Log_Warn(Format,  ...)
    #define Log_Info(Format, ...)
    #define Log_Debug(Format, ...)

#elif (LOG_LEVEL == LOG_LEVEL_FATAL)
    #define Log_Fatal(Format, ...)   MyLog(LOG_LEVEL_FATAL, __FILE__, __LINE__, Format, ##__VA_ARGS__)
    #define Log_Error(Format, ...)
    #define Log_Warn(Format, ...)
    #define Log_Info(Format, ...)
    #define Log_Debug(Format, ...)
#endif

void Log_Init(void);
void MyLog(LogLevelTypeDef Level, const char *File, int Line, const char *Format, ...);
void Log_Print(void);

#endif
