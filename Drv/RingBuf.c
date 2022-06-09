/**
 ******************************************************************************
 * @file    ringbuf.c
 * @author  jkeyvv
 * @version V1.0.0
 * @date    2021-10-21
 * @brief   Ring buffer management
 ******************************************************************************
 **/


#include "RingBuf.h"
#include "Heap.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define RINGBUF_WRITE_PROTECT

static void Ring_Lock(void);
static void Ring_UnLock(void);

/** 
 * @brief  创建环形缓冲区
 * @param  BufSize：缓冲区大小
 * @return 指向缓冲区结构体的指针（NULL：创建失败）
 */
RingBufTypeDef *RingBuf_Create(size_t BufSize)
{
    RingBufTypeDef *RingBuf = (RingBufTypeDef *)pvPortMalloc(sizeof(RingBufTypeDef));
    if (RingBuf == NULL)
    {
        printf("RingBuf Create Error\r\n");
        return NULL;
    }
    
    uint8_t *Store = (uint8_t *)pvPortMalloc(BufSize);
    if (Store == NULL)
    {
        printf("RingBuf Store Create Error\r\n");
        vPortFree(RingBuf);
        return NULL;
    }
        
    RingBuf->Store = Store;
    RingBuf->Size = BufSize;
    RingBuf->Head = RingBuf->Tail = Store;
    RingBuf->Length = 0;
        
    memset(RingBuf->Store, 0, RingBuf->Size);
    
    return RingBuf;
}

/** 
 * @brief  销毁环形缓冲区
 * @param  指向缓冲区结构体的指针
 * @return void
 */
void RingBuf_Delate(RingBufTypeDef *RingBuf)
{
    if (RingBuf == NULL)
        return;
    
    vPortFree(RingBuf->Store);
    vPortFree(RingBuf);
}

/** 
 * @brief  获取环形缓冲区大小
 * @param  RingBuf 环形缓冲区句柄
 * @return 缓冲区大小
 */
uint32_t RingBuf_GetRemain(RingBufTypeDef *RingBuf)
{
    return RingBuf->Size - RingBuf->Length;
}

/** 
 * @brief  获取环形缓冲区已存储数据量
 * @param  RingBuf 环形缓冲区句柄
 * @return 已存储数据量
 */
uint32_t RingBuf_GetLength(RingBufTypeDef *RingBuf)
{
    return RingBuf->Length;
}

/** 
 * @brief  从环形缓冲区弹出数据
 * @param  RingBuf 环形缓冲区句柄
 * @param  Data 数据要存储的位置
 * @param  Size 要弹出的数据量
 * @return 执行结果
 */
uint8_t RingBuf_Pop(RingBufTypeDef *RingBuf, uint8_t *Data, uint32_t Size)
{
    if (RingBuf == NULL || Data == NULL)
        return FEEDBACK_ERROR;
    
    Ring_Lock();
    
    /* 检查长度 */
    if (Size > RingBuf_GetLength(RingBuf))
    {
        Ring_UnLock();
        return FEEDBACK_FAIL;
    }
    
    /* 计算缓冲区末尾数据量是否足够 */
    if (RingBuf->Head + Size <= RingBuf->Store + RingBuf->Size)
    {
        /* 弹出数据 */
        memcpy(Data, RingBuf->Head, Size);
        
        /* 更新缓冲区 */
        RingBuf->Length -= Size;
        RingBuf->Head += Size;
        if(RingBuf->Head == RingBuf->Store + RingBuf->Size)
            RingBuf->Head = RingBuf->Store;
    }
    else /* 缓冲区末尾数据量不足 */
    {
        /* 计算末尾和开头需要弹出的数据量 */
        uint32_t BytesFirst = RingBuf->Store + RingBuf->Size - RingBuf->Head;
        uint32_t BytesSecond = Size - BytesFirst;
        
        /* 从末尾弹出数据 */
        memcpy(Data, RingBuf->Head, BytesFirst);
        
        /* 从开头弹出数据 */
        memcpy(Data + BytesFirst, RingBuf->Store, BytesSecond);
        
        /* 更新缓冲区 */
        RingBuf->Length -= Size;
        RingBuf->Head = RingBuf->Store + BytesSecond;
    }
    
    Ring_UnLock();
    return FEEDBACK_OK;
}

/** 
 * @brief  将数据压入环形缓冲区
 * @param  RingBuf 环形缓冲区句柄
 * @param  Data 数据存储位置
 * @param  Size 要压入的数据量
 * @return 执行结果
 */
uint8_t RingBuf_Push(RingBufTypeDef *RingBuf, const uint8_t *Data, uint32_t Size)
{
    if (RingBuf == NULL)
        return FEEDBACK_ERROR;
    
    Ring_Lock();
    
    /* 检查长度 */
    if (Size > RingBuf_GetRemain(RingBuf))
    {
        Ring_UnLock();
        return FEEDBACK_FAIL;
    }
    
    if (RingBuf->Tail + Size <= RingBuf->Store + RingBuf->Size)
    {
        /* 存储数据 */
        memcpy(RingBuf->Tail, Data, Size);
        
        /* 更新缓冲区 */
        RingBuf->Length += Size;
        RingBuf->Tail += Size;
        if (RingBuf->Tail == RingBuf->Store + RingBuf->Size)
            RingBuf->Tail = RingBuf->Store;
    }
    else /* 缓冲区末尾储量不足 */
    {
        /* 计算末尾和开头需要压入的数据量 */
        uint32_t BytesFirst = RingBuf->Store + RingBuf->Size - RingBuf->Tail;
        uint32_t BytesSecond = Size - BytesFirst;
        
        /* 压入数据到末尾 */
        memcpy(RingBuf->Tail, Data, BytesFirst);
        
        /* 压入数据到开头 */
        memcpy(RingBuf->Store, Data + BytesFirst, BytesSecond);
        
        /* 更新缓冲区 */
        RingBuf->Length += Size;
        RingBuf->Tail = RingBuf->Store + BytesSecond;
    }
    
    Ring_UnLock();
    return FEEDBACK_OK;
}

/** 
 * @brief  从环形缓冲区读取数据（数据不弹出）
 * @param  RingBuf 环形缓冲区句柄
 * @param  Data 数据要存储的位置
 * @param  Size 要读取的数据量
 * @param  Offset 相对于队头的偏移量
 * @return 执行结果
 */
uint8_t RingBuf_Read(RingBufTypeDef *RingBuf, uint8_t *Data, uint32_t Size, uint32_t Offset)
{
    if (RingBuf == NULL || Data == NULL)
        return FEEDBACK_ERROR;
    
    Ring_Lock();
    
    /* 检查长度 */
    if (Size + Offset > RingBuf_GetLength(RingBuf))
    {
        Ring_UnLock();
        return FEEDBACK_FAIL;
    }
    
    /* 计算缓冲区末尾数据量是否足够 */
    if (RingBuf->Head + Offset + Size <= RingBuf->Store + RingBuf->Size)
    {
        /* 弹出数据 */
        memcpy(Data, RingBuf->Head + Offset, Size);
    }
    else /* 缓冲区末尾数据量不足 */
    {
        /* 计算末尾和开头需要读取的数据量 */       
        uint32_t BytesFirst = 0;
        uint32_t BytesSecond = 0;
        
        /* 判断队尾是否有数据要读取 */
        if (RingBuf->Head + Offset < RingBuf->Store + RingBuf->Size)
        {
            BytesFirst = RingBuf->Store + RingBuf->Size - RingBuf->Head - Offset;
            BytesSecond = Size - BytesFirst;
            
            /* 从末尾读取数据 */
            memcpy(Data, RingBuf->Head + Offset, BytesFirst);
        
            /* 从开头读取数据 */
            memcpy(Data + BytesFirst, RingBuf->Store, BytesSecond);
        }
        else
        {
            /* 计算队头偏移量 */
            uint32_t OffsetHead =  RingBuf->Head + Offset - (RingBuf->Store + RingBuf->Size);
            
            /* 从队头读数据 */
            memcpy(Data, RingBuf->Store + OffsetHead, Size);
        }
    }
    
    Ring_UnLock();
    return FEEDBACK_OK;
}

/** 
 * @brief  从环形缓冲区中弹出单个字节
 * @param  RingBuf 环形缓冲区句柄
 * @param  Data 数据存储位置
 * @return 执行结果
 */
inline uint8_t RingBuf_PopByte(RingBufTypeDef *RingBuf, uint8_t *Data)
{
    return RingBuf_Pop(RingBuf, Data, 1);
}

/** 
 * @brief  向环形缓冲区中压入单个字节
 * @param  RingBuf 环形缓冲区句柄
 * @param  Data 数据存储位置
 * @return 执行结果
 */
inline uint8_t RingBuf_PushByte(RingBufTypeDef *RingBuf, uint8_t Data)
{
    return RingBuf_Push(RingBuf, &Data, 1);
}

/** 
 * @brief  从环形缓冲区中读取单个字节（数据不弹出）
 * @param  RingBuf 环形缓冲区句柄
 * @param  Data 数据存储位置
 * @return 执行结果
 */
inline uint8_t RingBuf_ReadByte(RingBufTypeDef *RingBuf, uint8_t *Data, uint32_t Offset)
{
    return RingBuf_Read(RingBuf, Data, 1, Offset);
}

/** 
 * @brief  环形缓冲区上锁
 * @param  void
 * @return void
 */
static void Ring_Lock(void)
{
    #ifdef RINGBUF_WRITE_PROTECT
    /* 关闭总中断 */
    __disable_irq();
    #endif
}

/** 
 * @brief  环形缓冲区解锁
 * @param  void
 * @return void
 */
static void Ring_UnLock(void)
{
    #ifdef RINGBUF_WRITE_PROTECT
    /* 打开总中断 */
    __enable_irq();
    #endif
}

