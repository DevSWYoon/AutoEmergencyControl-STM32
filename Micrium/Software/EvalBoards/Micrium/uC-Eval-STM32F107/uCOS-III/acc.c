// set ACC1, ACC2 (MPU 6050) Intterrupt Enable & It's Threshold Using "MPU6050.h"

#include "acc.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_exti.h"

#include "MPU6050.h"

void ACC_Config(void)
{
    // GPIO Configuration
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // TODO: GPIO Pin setting can be changed
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    EXTI_InitTypeDef EXTI_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);
    
    // TODO: EXTI Line setting can be changed by GPIO Pin setting
    EXTI_InitStructure.EXTI_Line = EXTI_Line0 | EXTI_Line1;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    
    EXTI_Init(&EXTI_InitStructure);
    
    // NVIC Configuration
    NVIC_InitTypeDef NVIC_InitStructure;
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn | EXTI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    
    NVIC_Init(&NVIC_InitStructure);
    
    // MPU6050 Configuration
    // TODO: MPU6050 Configuration can be changed by GPIO Pin setting
    MPU6050_Initialize();
    MPU6050_SetInterruptPinLevel(0);
    MPU6050_SetInterruptDrive(0);
    MPU6050_SetInterruptLatch(1);
    MPU6050_SetInterruptLatchClear(1);
    MPU6050_SetIntDataReadyEnabled(1);
    MPU6050_SetIntZeroMotionEnabled(0);
    MPU6050_SetIntMotionEnabled(0);
    MPU6050_SetIntFreeFallEnabled(0);
    MPU6050_SetIntMasterEnabled(0);
    MPU6050_SetIntPLLReadyEnabled(0);
    MPU6050_SetIntDMPEnabled(0);
    MPU6050_SetDMPEnabled(0);
    MPU6050_SetI2CBypassEnabled(0);
    MPU6050_SetClockSource(0);
    MPU6050_SetFullScaleAccelRange(0);
    MPU6050_SetFullScaleGyroRange(0);
    MPU6050_SetSleepEnabled(0);
}

void EXTI0_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        // TODO : Add interrupt handling code here.
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

void EXTI1_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line1) != RESET)
    {
        // TODO : Add interrupt handling code here.
        EXTI_ClearITPendingBit(EXTI_Line1);
    }
}

// End of File