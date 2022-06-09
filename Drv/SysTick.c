/**
 ******************************************************************************
 * @file    SysTick.c
 * @author  jkeyvv
 * @version V1.0.0
 * @date    2021-12-2
 * @brief   System tick timer config
 ******************************************************************************
 **/
 

#include "SysTick.h"

__IO uint32_t TickCount;

uint8_t SysTick_Init(uint32_t PreemptPriority, uint32_t SubPriority)
{
    /* Configure Systick clock source as HCLK */
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
    
    /* Configure the SysTick to have interrupt in 1ms time basis*/
    RCC_ClocksTypeDef RCC_Clocks;
    RCC_GetClocksFreq(&RCC_Clocks);
    if (SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000U) != 0U)
    {
      return 1;
    }

    /* Configure the SysTick IRQ priority */
    if (PreemptPriority < (1UL << __NVIC_PRIO_BITS))
    {   
        /* Check the parameters */
        assert_param(IS_NVIC_SUB_PRIORITY(SubPriority));
        assert_param(IS_NVIC_PREEMPTION_PRIORITY(PreemptPriority));
        
        uint32_t Priority = NVIC_EncodePriority(NVIC_GetPriorityGrouping(), PreemptPriority, SubPriority);
        NVIC_SetPriority(SysTick_IRQn, Priority);
    }
    else
    {
      return 1;
    }

    /* Return function status */
    return 0;
}

uint32_t SysTick_GetTickCount(void)
{
    return TickCount;
}


void SysTick_IRQ_Callback(void)
{
    TickCount++;
}
