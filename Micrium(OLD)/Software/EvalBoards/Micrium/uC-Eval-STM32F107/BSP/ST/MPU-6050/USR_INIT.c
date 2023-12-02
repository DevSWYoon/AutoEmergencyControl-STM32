#include "stm32f10x_lib.h"
#include "stm32f10x_nvic.h"
#include "stm32f10x_exti.h"
#include "USR_INIT.h"
#include "bsp.h"
#include <includes.h>

void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);
void EXTI2_IRQHandler(void);

void Initialize_MPU6050(uint8_t address) {
    // 클럭 소스를 X 축 자이로스코프로 설정
    MPU6050_WriteBits(address, MPU6050_RA_PWR_MGMT_1, 2, 3, 0x01);

    // 가속도계의 full-scale range를 ±2G로 설정
    MPU6050_WriteBits(address, MPU6050_RA_ACCEL_CONFIG, 3, 2, MPU6050_SCALE); // ±2G
}

void Enable_Motion_Interrupt(uint8_t address, uint8_t threshold) {
    // 모션 인터럽트 임계값 설정
    MPU6050_WriteBits(address, MPU6050_RA_MOT_THR, 0, 8, threshold);

    // 모션 인터럽트 설정
    MPU6050_WriteBits(address, MPU6050_RA_INT_ENABLE, 6, 1, 0x01);
}

void RCC_Configuration(void) {
    // RCC 설정
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
}

void GPIO_Configuration(void) {
    // GPIO 설정
    GPIO_InitTypeDef GPIO_InitStructure;

    // MPU6050_1의 INT 핀 설정
    GPIO_InitStructure.GPIO_Pin = MPU6050_1_INT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // MPU6050_2의 INT 핀 설정
    GPIO_InitStructure.GPIO_Pin = MPU6050_2_INT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // BTN 핀 설정
    GPIO_InitStructure.GPIO_Pin = BTN_INT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void EXTI_Configuration(void) {
    // EXTI 설정
    EXTI_InitTypeDef EXTI_InitStructure;

    // MPU6050_1의 INT 핀 설정
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource0);

    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // MPU6050_2의 INT 핀 설정
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource1);

    EXTI_InitStructure.EXTI_Line = EXTI_Line1;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // BTN 핀 설정
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource2);

    EXTI_InitStructure.EXTI_Line = EXTI_Line2;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}

void NVIC_Configuration(void) {
		BSP_IntInit();
	
		BSP_IntVectSet(BSP_INT_ID_EXTI0, EXTI0_IRQHandler);
		BSP_IntPrioSet(BSP_INT_ID_EXTI0, 0);
	  BSP_IntEn(BSP_INT_ID_EXTI0);
	
		BSP_IntVectSet(BSP_INT_ID_EXTI1, EXTI1_IRQHandler);
		BSP_IntPrioSet(BSP_INT_ID_EXTI1, 1);
	  BSP_IntEn(BSP_INT_ID_EXTI1);
	
	  BSP_IntVectSet(BSP_INT_ID_EXTI2, EXTI2_IRQHandler);
		BSP_IntPrioSet(BSP_INT_ID_EXTI2, 2);
	  BSP_IntEn(BSP_INT_ID_EXTI2);
}
