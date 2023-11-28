#include "MPU6050.h"

#define MPU6050_1 0x68 // MPU6050_1의 I2C 주소
#define MPU6050_2 0x69 // MPU6050_2의 I2C 주소

void Initialize_MPU6050(uint8_t address) ;

void Enable_Motion_Interrupt(uint8_t address, uint8_t threshold) ;