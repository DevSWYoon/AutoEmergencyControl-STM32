#include "USR_INIT.h"
#include <includes.h>
#include "MPU6050.h"



void Initialize_MPU6050() {	
	  MPU6050_SetClockSource(MPU6050_CLOCK_PLL_XGYRO);
    MPU6050_SetFullScaleGyroRange(MPU6050_GYRO_FS_250);
    MPU6050_SetFullScaleAccelRange(MPU6050_ACCEL_FS_2);
    MPU6050_SetSleepModeStatus(DISABLE);
}

void Enable_Motion_Interrupt(uint8_t address, uint8_t threshold) {
    // 모션 인터럽트 임계값 설
	  // 충분한 가속도 임계값을 주면 없어도 되지 않을까?
		// MPU6050_WriteBits(address, MPU6050_RA_ACCEL_CONFIG, 0, 7, 0x01);
		
		MPU6050_WriteBit(address, MPU6050_RA_INT_ENABLE, MPU6050_INTERRUPT_MOT_BIT, 1);
		MPU6050_WriteBit(address, MPU6050_RA_MOT_THR, 7, 1);
		MPU6050_WriteBit(address, MPU6050_RA_MOT_DUR, 0, 1);
  	MPU6050_WriteBits(address, MPU6050_RA_INT_PIN_CFG, 0, 7, 0x00);
     
	
	/*
		uint8_t c;
		MPU6050_ReadBit(MPU6050_DEFAULT_ADDRESS, 0x37, 3, &c);
		APP_TRACE_INFO(("%d\n\r", c));
		MPU6050_ReadBit(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_INT_ENABLE, MPU6050_INTERRUPT_MOT_BIT, &c);
		APP_TRACE_INFO(("%d\n\r", c));*/
	  // MPU6050_WriteBits(address, MPU6050_RA_MOT_DUR, 0, 8, 1);
		// MPU6050_WriteBits(address, MPU6050_RA_MOT_DETECT_CTRL, 0, 7, 0x15);
    // 모션 인터럽트 설정
}

void RCC_Configuration(void) {
    // RCC 설정
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
}

void GPIO_Configuration(void) {
    // GPIO 설정
    GPIO_InitTypeDef GPIO_InitStructure;

    // MPU6050_1의 INT 핀 설정
    GPIO_InitStructure.GPIO_Pin = MPU6050_1_INT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // BTN 핀 설정
    GPIO_InitStructure.GPIO_Pin = BTN_INT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // 단란 감지 센서의 INT 핀 설정
    GPIO_InitStructure.GPIO_Pin = SHORTCIRCUIT_INT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void EXTI_Configuration(void) {
    // EXTI 설정
    EXTI_InitTypeDef EXTI_InitStructure;

    // MPU6050_1의 INT 핀 설정
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource1);

    EXTI_InitStructure.EXTI_Line = EXTI_Line1;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // BTN 핀 설정
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource4);

    EXTI_InitStructure.EXTI_Line = EXTI_Line4;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // 단락 감지 센서의 INT 핀 설정
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource5);

    EXTI_InitStructure.EXTI_Line = EXTI_Line5;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}

void NVIC_Configuration(void) {
		RCC_Configuration();
    GPIO_Configuration();
    EXTI_Configuration();

    // BSP Interrupt 설정
    BSP_IntVectSet(BSP_INT_ID_EXTI1, EXTI1_IRQHandler);
    BSP_IntVectSet(BSP_INT_ID_EXTI4, EXTI4_IRQHandler);
    BSP_IntVectSet(BSP_INT_ID_EXTI9_5, EXTI9_5_IRQHandler);

    // BSP Priority 설정
    BSP_IntPrioSet(BSP_INT_ID_EXTI1, 0);
    BSP_IntPrioSet(BSP_INT_ID_EXTI4, 1);
    BSP_IntPrioSet(BSP_INT_ID_EXTI9_5, 2);

    // BSP Interrupt Enable
    BSP_IntEn(BSP_INT_ID_EXTI1);
    BSP_IntEn(BSP_INT_ID_EXTI4);
    BSP_IntEn(BSP_INT_ID_EXTI9_5);
}
