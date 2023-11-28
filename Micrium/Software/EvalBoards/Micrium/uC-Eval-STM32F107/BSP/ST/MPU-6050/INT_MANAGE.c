#include "INT_MANAGE.h"
#include "stm32f10x_conf.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "MPU6050.h"
#include "HAL_MPU6050.h"

#include "MPU6050.h"

void setup() {
    // MPU6050_1 및 MPU6050_2 초기화
    // 클럭 소스를 X 축 자이로스코프로 설정
    MPU6050_WriteBits(MPU6050_1, MPU6050_RA_PWR_MGMT_1, 2, 3, 0x01);
    MPU6050_WriteBits(MPU6050_2, MPU6050_RA_PWR_MGMT_1, 2, 3, 0x01);

    // 가속도계의 full-scale range를 ±2G로 설정
    MPU6050_WriteBits(MPU6050_1, MPU6050_RA_ACCEL_CONFIG, 3, 2, 0x00); // ±2G
    MPU6050_WriteBits(MPU6050_2, MPU6050_RA_ACCEL_CONFIG, 3, 2, 0x00); // ±2G

    // 모션 인터럽트 임계값 설정
    uint8_t threshold_1 = 0x40; // 1G에 해당하는 값
    uint8_t threshold_2 = 0x80; // 2G에 해당하는 값

    MPU6050_WriteBits(MPU6050_1, MPU6050_RA_MOT_THR, 0, 8, threshold_1);
    MPU6050_WriteBits(MPU6050_2, MPU6050_RA_MOT_THR, 0, 8, threshold_2);
}


void InterruptInitailize() {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); // GPIOC 클럭 활성화
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); // AFIO 클럭 활성화


    // MPU6050_1의 INT 핀을 EXTI0에 연결
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource0);

    // MPU6050_2의 INT 핀을 EXTI1에 연결
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource1);

    // BTN의 핀을 EXTI2에 연결
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource2);

    // 인터럽트 우선순위 : EXTI0 > EXTI1 > EXTI2

    // EXTI0 인터럽트 설정
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Line0; // EXTI0 사용
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; // 인터럽트 모드
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; // 상승 에지에서 인터럽트 발생
    EXTI_InitStructure.EXTI_LineCmd = ENABLE; // EXTI0 사용 설정
    EXTI_Init(&EXTI_InitStructure);

    // EXTI1 인터럽트 설정
    EXTI_InitStructure.EXTI_Line = EXTI_Line1; // EXTI1 사용
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; // 인터럽트 모드
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; // 상승 에지에서 인터럽트 발생
    EXTI_InitStructure.EXTI_LineCmd = ENABLE; // EXTI1 사용 설정
    EXTI_Init(&EXTI_InitStructure);

    // EXTI2 인터럽트 설정
    EXTI_InitStructure.EXTI_Line = EXTI_Line2; // EXTI2 사용
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; // 인터럽트 모드
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // 상승 에지에서 인터럽트 발생
    EXTI_InitStructure.EXTI_LineCmd = ENABLE; // EXTI2 사용 설정
    EXTI_Init(&EXTI_InitStructure);

    // NVIC 인터럽트 설정
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 인터럽트 우선순위 그룹 2로 설정

    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQChannel; // EXTI0 인터럽트
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; // 선점 우선순위 0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00; // 서브 우선순위 0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // EXTI0 인터럽트 활성화
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQChannel; // EXTI1 인터럽트
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01; // 선점 우선순위 0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00; // 서브 우선순위 0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // EXTI1 인터럽트 활성화
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQChannel; // EXTI2 인터럽트
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01; // 선점 우선순위 0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01; // 서브 우선순위 0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // EXTI2 인터럽트 활성화
    NVIC_Init(&NVIC_InitStructure);
}

void EXTI0_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
        EXTI_ClearITPendingBit(EXTI_Line0); // EXTI0 인터럽트 플래그 클리어
        // MPU6050_1의 INT 핀이 인터럽트를 발생시킴
        // MPU6050_1의 모션 인터럽트 발생
        // MPU6050_1의 INT 핀을 인터럽트 발생시킨 상태로 유지
        // MPU6050_1의 INT 핀을 인터럽트 발생시키지 않는 상태로 유지
    }
}

void EXTI1_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line1) != RESET) {
        EXTI_ClearITPendingBit(EXTI_Line1); // EXTI1 인터럽트 플래그 클리어
        // MPU6050_2의 INT 핀이 인터럽트를 발생시킴
        // MPU6050_2의 모션 인터럽트 발생
        // MPU6050_2의 INT 핀을 인터럽트 발생시킨 상태로 유지
        // MPU6050_2의 INT 핀을 인터럽트 발생시키지 않는 상태로 유지
    }
}

void EXTI2_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line2) != RESET) {
        EXTI_ClearITPendingBit(EXTI_Line2); // EXTI2 인터럽트 플래그 클리어
        // BTN의 핀이 인터럽트를 발생시킴
        // BTN의 인터럽트 발생
    }
}


