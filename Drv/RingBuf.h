#ifndef __RING_BUF_H
#define __RING_BUF_H

#ifdef __cpluscplus
extern "C"
{
#endif

#include <stdint.h>
#include <stddef.h>

#define FEEDBACK_OK     0
#define FEEDBACK_FAIL   1
#define FEEDBACK_ERROR  2

/* 定义环形缓冲区句柄 */
typedef struct
{
    uint8_t *Head;      //队列头
    uint8_t *Tail;      //队列尾
    uint8_t *Store;     //实际存储位置
    uint32_t Size;      //队列大小
    uint32_t Length;    //已存储数据长度
}RingBufTypeDef;

RingBufTypeDef *RingBuf_Create(size_t BufSize);
void RingBuf_Delate(RingBufTypeDef *RingBuf);
uint32_t RingBuf_GetLength(RingBufTypeDef *RingBuf);
uint32_t RingBuf_GetRemain(RingBufTypeDef *RingBuf);

uint8_t RingBuf_Pop(RingBufTypeDef *RingBuf, uint8_t *Data, uint32_t Size);
uint8_t RingBuf_Push(RingBufTypeDef *RingBuf, const uint8_t *Data, uint32_t Size);
uint8_t RingBuf_Read(RingBufTypeDef *RingBuf, uint8_t *Data, uint32_t Size, uint32_t Offset);

uint8_t RingBuf_PopByte(RingBufTypeDef *RingBuf, uint8_t *Data);
uint8_t RingBuf_PushByte(RingBufTypeDef *RingBuf, uint8_t Data);
uint8_t RingBuf_ReadByte(RingBufTypeDef *RingBuf, uint8_t *Data, uint32_t Offset);

#ifdef __cpluscplus
}
#endif


#endif

