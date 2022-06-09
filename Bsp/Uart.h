#ifndef __UART_H
#define __UART_H

/** 
 * 注意：
 * 1、Uartx_RxOverProcess 函数是串口 x 触发接收空闲中断调用的函数，
 *    pData 是接收到的数据头指针，len 是接收到的数据长度，即字节数
 * 
 * 2、Uartx_TxOverProcess 函数是串口 x 调用 Uartx_DMA_Send 发送完成
 *    的中断处理函数
 * 
 */

/* 串口 1 参数配置 */
#define UART1_RX_IDLE_PRIORITY      (6)         // 串口1 DMA 接收空闲中断优先级
#define UART1_TX_DMA_PRIORITY       (6)         // 串口1 DMA 发送完成中断优先级
#define UART1_BAUD_RATE             (115200)    // 串口1 波特率设置

/* 串口 1 相关函数 */
void Uart1_RxDMA_Init(void);
void Uart1_TxDMA_Init(void);
void Uart1_Init(unsigned int baudRate);

__weak void Uart1_RxOverProcess(char* pData, unsigned short len);
__weak void Uart1_TxOverProcess(void);


void USART1_IRQHandler(void);

void DMA2_Stream7_IRQHandler(void); // 串口1 DMA 发送中断

int Uart1_DMA_Send(char* pData, unsigned short len);


#endif

