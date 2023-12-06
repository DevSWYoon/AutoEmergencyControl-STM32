#include "MPU6050.h"
#include "bsp.h"

#define MPU6050_SCALE 0x00

#define MPU6050_1_INT_PIN GPIO_Pin_1 // MPU6050_1의 INT 핀


#define MPU6050_1_INT_PORT GPIOE // MPU6050_1의 INT 포트

#define BTN_INT_PIN  GPIO_Pin_4 // BTN의 INT 핀
#define BTN_INT_PORT GPIOC // BTN의 INT 포트

#define MPU6050_Threshold 1000 // Threshold 값

void Initialize_MPU6050();
void Enable_Motion_Interrupt(uint8_t address, uint8_t threshold);
void RCC_Configuration(void);
void GPIO_Configuration(void);
void EXTI_Configuration(void);
void NVIC_Configuration(void);

void EXTI1_IRQHandler(void);
void EXTI4_IRQHandler(void);