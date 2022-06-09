#ifndef __IP101GR_H
#define __IP101GR_H

#include "stm32f4x7_eth.h"


#define IP101GR_PHY_ADDRESS  	0x01				//IP101GR PHY芯片地址.

extern ETH_DMADESCTypeDef *DMARxDscrTab;			//以太网DMA接收描述符数据结构体指针
extern ETH_DMADESCTypeDef *DMATxDscrTab;			//以太网DMA发送描述符数据结构体指针 
extern uint8_t *Rx_Buff; 							//以太网底层驱动接收buffers指针 
extern uint8_t *Tx_Buff; 							//以太网底层驱动发送buffers指针
extern ETH_DMADESCTypeDef  *DMATxDescToSet;			//DMA发送描述符追踪指针
extern ETH_DMADESCTypeDef  *DMARxDescToGet; 		//DMA接收描述符追踪指针 
extern ETH_DMA_Rx_Frame_infos *DMA_RX_FRAME_infos;	//DMA最后接收到的帧信息指针
 

uint8_t IP101GR_Init(void);
uint8_t ETH_MACDMA_Config(void);
FrameTypeDef ETH_Rx_Packet(void);
uint8_t ETH_Tx_Packet(uint16_t FrameLength);
uint32_t ETH_GetCurrentTxBuffer(void);
uint8_t ETH_Mem_Malloc(void);
void ETH_Mem_Free(void);
#endif 

