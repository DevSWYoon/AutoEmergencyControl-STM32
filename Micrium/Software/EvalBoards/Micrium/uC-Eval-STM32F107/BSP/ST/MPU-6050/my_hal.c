#include "stm32f10x_lib.h"
#include "bsp.h"

void BuzzerInit(void)
{
    // Initialize GPIOA0's buzzer
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void BuzzerToggle(void)
{
    if(GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_3) == 0)
    {
        GPIO_SetBits(GPIOA, GPIO_Pin_3);
    }
    else
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_3);
    }
}


void BuzzerTmrCallback(void *p_tmr, void *p_arg)
{
   BuzzerToggle();
   BSP_LED_Toggle(0);
}

void TimeoutPendingTmrCallback(void *p_tmr, void *p_arg)
{
    APP_TRACE_INFO(("SHORT CIRCUITS DETECTED! PUSH THE BUTTON!\n"));
    BSP_LED_Toggle(3);
}


void RelayInit(void)
{
    // Initialize GPIOA1's relay
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOE, &GPIO_InitStructure);
}

void RelayCutOff(void)
{
    GPIO_ResetBits(GPIOE, GPIO_Pin_11);
}

void RelayTurnOn(void)
{
    GPIO_SetBits(GPIOE, GPIO_Pin_11);
}