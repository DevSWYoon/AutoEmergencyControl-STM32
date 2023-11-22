#include "INT_MANAGE.h"


void InitializeMPU6050s() {
    // MPU6050 I2C ???
    MPU6050_I2C_Init();

    // ACC1 ??? (AD0 = GND)
    uint8_t pwr_mgmt_1 = 0x00; // ?? ?? ????
    MPU6050_I2C_ByteWrite(MPU6050_ADDRESS_AD0_LOW, &pwr_mgmt_1, MPU6050_RA_PWR_MGMT_1);
    MPU6050_SetFullScaleAccelRange(MPU6050_ACCEL_FS_2); // ???? ?? ?? (±2g)
    MPU6050_SetFullScaleGyroRange(MPU6050_GYRO_FS_250); // ??? ?? ?? (±250°/s)

    MPU6050_I2C_ByteWrite(MPU6050_ADDRESS_AD0_LOW, &ACC1_THR, MPU6050_RA_MOT_THR);

    // ACC2 ??? (AD0 = VCC)
    MPU6050_I2C_ByteWrite(MPU6050_ADDRESS_AD0_HIGH, &pwr_mgmt_1, MPU6050_RA_PWR_MGMT_1);
    MPU6050_SetFullScaleAccelRange(MPU6050_ACCEL_FS_2); // ???? ?? ?? (±2g)
    MPU6050_SetFullScaleGyroRange(MPU6050_GYRO_FS_250); // ??? ?? ?? (±250°/s)
		MPU6050_I2C_ByteWrite(MPU6050_ADDRESS_AD0_HIGH, &ACC2_THR, MPU6050_RA_MOT_THR);

    // ?? ??? ??...
}