/**
 ******************************************************************************
 * @file    Led.c
 * @author  jkeyvv
 * @version V1.0.0
 * @date    2021-12-2
 * @brief   Driver of Led
 ******************************************************************************
 **/

#include "Led.h"

#define LED_NUM (1)

#define LED_1_PIN              GPIO_Pin_15
#define LED_1_PORT             GPIOD
#define LED_1_CLK              RCC_AHB1Periph_GPIOD
               


GPIO_TypeDef* LedPort[LED_NUM] = {LED_1_PORT};

const uint16_t LedPin[LED_NUM] = {LED_1_PIN};

const uint32_t LedClk[LED_NUM] = {LED_1_CLK};

/**
  * @brief  Configures LED GPIO.
  * @retval None
  */
void Led_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure = {0};
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    for (int i = 0; i < LED_NUM; i++)
    {
        /* Enable the GPIO_LED Clock */
        RCC_AHB1PeriphClockCmd(LedClk[i], ENABLE);
        
        /* Configure the GPIO_LED pin */
        GPIO_InitStructure.GPIO_Pin = LedPin[i];
        GPIO_Init(LedPort[i], &GPIO_InitStructure);
        
        Led_Off((LedTypeDef)i);
    }
}

/**
  * @brief  Turns selected LED On.
  * @param  Led: Specifies the Led to be set on. 
  * @retval None
  */
void Led_On(LedTypeDef Led)
{
    GPIO_SetBits(LedPort[Led], LedPin[Led]);
}

/**
  * @brief  Turns selected LED Off.
  * @param  Led: Specifies the Led to be set off. 
  * @retval None
  */
void Led_Off(LedTypeDef Led)
{
    GPIO_ResetBits(LedPort[Led], LedPin[Led]);
}

/**
  * @brief  Toggles the selected LED.
  * @param  Led: Specifies the Led to be toggled. 
  * @retval None
  */
void Led_Toggle(LedTypeDef Led)
{
  LedPort[Led]->ODR ^= LedPin[Led];
}
