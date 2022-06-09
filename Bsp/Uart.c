#include "Uart.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "stm32f4xx.h" 

#define UART1_RX_BUF_LEN            (128)

typedef struct
{
    char uart1RxBuf[UART1_RX_BUF_LEN];

}UartCommonTypeDef;


static UartCommonTypeDef s_UartCommon = {0};

/** 
 * @brief  配置串口1 接收 DMA 使用的是 DMA2 的数据流 5 通道 4
 * @param  void
 * @return void
 */
void Uart1_RxDMA_Init(void)
{
    /* DMA 初始化 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
    DMA_InitTypeDef   DMA_InitStructure = {0};
    DMA_InitStructure.DMA_BufferSize            = UART1_RX_BUF_LEN;
    DMA_InitStructure.DMA_Channel               = DMA_Channel_4;
    DMA_InitStructure.DMA_DIR                   = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_FIFOMode              = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold         = DMA_FIFOThreshold_1QuarterFull;
    DMA_InitStructure.DMA_Memory0BaseAddr       = (unsigned int)(s_UartCommon.uart1RxBuf);
    DMA_InitStructure.DMA_MemoryBurst           = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_MemoryDataSize        = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_MemoryInc             = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_Mode                  = DMA_Mode_Normal;
    DMA_InitStructure.DMA_PeripheralBaseAddr    = (unsigned int)(&USART1->DR);
    DMA_InitStructure.DMA_PeripheralBurst       = DMA_PeripheralBurst_Single;
    DMA_InitStructure.DMA_PeripheralDataSize    = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_PeripheralInc         = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_Priority              = DMA_Priority_High;
    DMA_DeInit(DMA2_Stream5);
    while(DMA_GetCmdStatus(DMA2_Stream5) != DISABLE);
    DMA_Init(DMA2_Stream5, &DMA_InitStructure);
    
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
    DMA_Cmd(DMA2_Stream5, ENABLE);
    
    NVIC_InitTypeDef   NVIC_InitStructure_USART = {0};
    NVIC_InitStructure_USART.NVIC_IRQChannel                    = USART1_IRQn;
    NVIC_InitStructure_USART.NVIC_IRQChannelCmd                 = ENABLE;
    NVIC_InitStructure_USART.NVIC_IRQChannelSubPriority         = 0;
    NVIC_InitStructure_USART.NVIC_IRQChannelPreemptionPriority  = UART1_RX_IDLE_PRIORITY;
    NVIC_Init(&NVIC_InitStructure_USART);
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
}

/** 
 * @brief  配置串口1 发送 DMA 使用的是 DMA2 的数据流 7 通道 4
 * @param  void
 * @return void
 */
void Uart1_TxDMA_Init(void)
{
    /* 配置 DMA 参数 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
    DMA_InitTypeDef   DMA_InitStructure = {0};
    DMA_InitStructure.DMA_BufferSize            = 0;
    DMA_InitStructure.DMA_Channel               = DMA_Channel_4;
    DMA_InitStructure.DMA_DIR                   = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_FIFOMode              = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold         = DMA_FIFOThreshold_1QuarterFull;
    DMA_InitStructure.DMA_Memory0BaseAddr       = NULL;
    DMA_InitStructure.DMA_MemoryBurst           = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_MemoryDataSize        = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_MemoryInc             = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_Mode                  = DMA_Mode_Normal;
    DMA_InitStructure.DMA_PeripheralBaseAddr    = (unsigned int)(&USART1->DR);
    DMA_InitStructure.DMA_PeripheralBurst       = DMA_PeripheralBurst_Single;
    DMA_InitStructure.DMA_PeripheralDataSize    = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_PeripheralInc         = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_Priority              = DMA_Priority_High;
    DMA_DeInit(DMA2_Stream7);
    while(DMA_GetCmdStatus(DMA2_Stream7) != DISABLE);
    DMA_Init(DMA2_Stream7, &DMA_InitStructure);
    
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
    
    NVIC_InitTypeDef   NVIC_InitStructure = {0};
    NVIC_InitStructure.NVIC_IRQChannel              = DMA2_Stream7_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd           = ENABLE; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority   = 0;  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = UART1_TX_DMA_PRIORITY;
    NVIC_Init(&NVIC_InitStructure);
    DMA_ClearFlag(DMA2_Stream7, DMA_FLAG_TCIF7);
    DMA_ITConfig(DMA2_Stream7, DMA_IT_TC, ENABLE);
}

/** 
 * @brief  串口 1 初始化
 * @param  baudRate 设置波特率
 * @return void
 */
void Uart1_Init(unsigned int baudRate)
{
    /* 所需资源时钟使能 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    
    /* 串口1对应引脚复用映射 */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,  GPIO_AF_USART1); 
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1); 
    
    /* GPIO 初始化 */
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    GPIO_InitStructure.GPIO_Pin                 = GPIO_Pin_9 | GPIO_Pin_10; 
    GPIO_InitStructure.GPIO_Mode                = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_PuPd                = GPIO_PuPd_UP; 
    GPIO_Init(GPIOA, &GPIO_InitStructure); 
    
    /* 串口功能配置 */
    USART_InitTypeDef USAInitStructure = {0};
    USAInitStructure.USART_BaudRate             = baudRate;
    USAInitStructure.USART_WordLength           = USART_WordLength_8b;
    USAInitStructure.USART_StopBits             = USART_StopBits_1;
    USAInitStructure.USART_Parity               = USART_Parity_No;
    USAInitStructure.USART_HardwareFlowControl  = USART_HardwareFlowControl_None;
    USAInitStructure.USART_Mode                 = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USAInitStructure); 
    USART_Cmd(USART1, ENABLE);  
}

__weak void Uart1_RxOverProcess(char* pData, unsigned short len) {}
__weak void Uart1_TxOverProcess(void) {}
    
/** 
 * @brief  Uart1 接收中断
 * @param  void
 * @return void
 */
void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) 
    {
        /* ----- ！！！特殊的清除空闲中断标志 -----	*/
        DMA_Cmd(DMA2_Stream5, DISABLE);
        USART1->SR;
        USART1->DR;
        DMA_ClearFlag(DMA2_Stream5, DMA_FLAG_TCIF5);
        
        /* 提取当前接收数据长度 */
        unsigned short receiveDataSize = 0;
        receiveDataSize = UART1_RX_BUF_LEN - DMA_GetCurrDataCounter(DMA2_Stream5);
        
        /* 具体处理接收数据的函数，重写弱定义函数 */
        Uart1_RxOverProcess(s_UartCommon.uart1RxBuf, receiveDataSize);
        
        DMA_SetCurrDataCounter(DMA2_Stream5, UART1_RX_BUF_LEN);
        DMA_Cmd(DMA2_Stream5, ENABLE);
    }
}

/** 
 * @brief  串口 1 DMA2 Stream7 发送数据完成中断
 * @param  void
 * @return void
 */  
void DMA2_Stream7_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA2_Stream7, DMA_IT_TCIF7))
    {
        DMA_ClearITPendingBit(DMA2_Stream7, DMA_IT_TCIF7);
        
        Uart1_TxOverProcess();
    }
}

/** 
 * @brief  串口1 DMA 发送数据
 * @param  pSendData  发送的数据指针
 * @param  DataLength 发送的数据长度
 * @return int 成功返回0， 失败返回 -1
 */
int Uart1_DMA_Send(char* pData, unsigned short len)
{
    if ((pData == NULL) || (len == 0))
    {
        printf("Uart1_DMA_Send Input Abnormal\r\n");
        return -1;
    }
    
    DMA_Cmd(DMA2_Stream7, DISABLE);
    DMA_SetCurrDataCounter(DMA2_Stream7, len);
    DMA2_Stream7->M0AR = (unsigned int)pData;
    DMA_Cmd(DMA2_Stream7, ENABLE);
    
    return 0;
}
