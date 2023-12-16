#ifndef RM_FRAME_C_IMU_H
#define RM_FRAME_C_IMU_H

#include "bmi088/BMI088driver.h"
#include "PID.h"
#include "bmi088/MahonyAHRS.h"
#include "definitions.h"

#define SPI_DMA_GYRO_LENGHT       8
#define SPI_DMA_ACCEL_LENGHT      9
#define SPI_DMA_ACCEL_TEMP_LENGHT 4

#define IMU_DR_SHFITS        0u
#define IMU_SPI_SHFITS       1u
#define IMU_UPDATE_SHFITS    2u
#define IMU_NOTIFY_SHFITS    3u


#define BMI088_GYRO_RX_BUF_DATA_OFFSET  1u
#define BMI088_ACCEL_RX_BUF_DATA_OFFSET 2u

/*枚举类型定义------------------------------------------------------------*/
/*结构体定义--------------------------------------------------------------*/
typedef struct{
    float accel[3],gyro[3],temp,time,mag[3];
}IMU_Raw_Data_t;

typedef struct{
    float yaw,pitch,rol;
    float yaw_v,pitch_v,rol_v;
    float neg_yaw_v,neg_pitch_v,neg_rol_v;
}IMU_Attitude_t;

typedef struct{
    volatile uint8_t gyro_update_flag;
    volatile uint8_t accel_update_flag;
    volatile uint8_t accel_temp_update_flag;
    volatile uint8_t mag_update_flag;
    volatile uint8_t imu_start_dma_flag;
}IMU_state_t;

/*类型定义----------------------------------------------------------------*/


void AHRS_update(float quat[4], float time, float gyro[3], float accel[3], float mag[3]);
void get_angle(float q[4], float *yaw, float *pitch, float *roll);
void IMU_Handle();
void IMU_Init();


/*结构体成员取值定义组------------------------------------------------------*/
/*外部变量声明-------------------------------------------------------------*/
/*外部函数声明-------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif //RM_FRAME_C_IMU_H
