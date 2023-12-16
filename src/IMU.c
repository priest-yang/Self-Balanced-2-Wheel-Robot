#include "IMU.h"
#include <math.h>
#include "callback.h"

IMU_state_t state;
float quat[4];
IMU_Raw_Data_t rawData;
IMU_Attitude_t attitude;
uint8_t imu_err = 0;

void IMU_Init() {
    imu_err = BMI088_init();
    Delay_(1);
    BMI088_read(rawData.gyro,rawData.accel,&rawData.temp);
    quat[0] = 1.0f,quat[1] = 0,quat[2] = 0,quat[3] = 0;
}

void IMU_Handle() {


    BMI088_read(rawData.gyro,rawData.accel,&rawData.temp);

    AHRS_update(quat, 0.001f, rawData.gyro, rawData.accel, rawData.mag);
    get_angle(quat,&attitude.yaw, &attitude.pitch, &attitude.rol);
    attitude.yaw_v = rawData.gyro[2];
    attitude.pitch_v = rawData.gyro[0];
    attitude.rol_v = rawData.gyro[1];

    attitude.neg_yaw_v = -attitude.yaw_v;
    attitude.neg_pitch_v = -attitude.pitch_v;
    attitude.neg_rol_v = -attitude.rol_v;

}

void AHRS_update(float quat[4], float time, float gyro[3], float accel[3], float mag[3])
{
    MahonyAHRSupdate(quat, gyro[0], gyro[1], gyro[2],
            accel[0], accel[1], accel[2],
            0,0,0);
}

void get_angle(float q[4], float *yaw, float *pitch, float *roll)
{
    *yaw = atan2f(2.0f*(q[0]*q[3]+q[1]*q[2]), 2.0f*(q[0]*q[0]+q[1]*q[1])-1.0f);
    *pitch = asinf(-2.0f*(q[1]*q[3]-q[0]*q[2]));
    *roll = atan2f(2.0f*(q[0]*q[1]+q[2]*q[3]),2.0f*(q[0]*q[0]+q[3]*q[3])-1.0f);
}
