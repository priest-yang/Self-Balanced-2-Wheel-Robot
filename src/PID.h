#ifndef RM_FRAME_C_PID_H
#define RM_FRAME_C_PID_H

#include "callback.h"

typedef struct PID_Regulator_t {
    float ref;
    float fdb;
    float err[4];
    float errSum;
    float kp;
    float ki;
    float kd;
    float componentKp;
    float componentKi;
    float componentKd;
    float componentKpMax;
    float componentKiMax;
    float componentKdMax;
    float output;
    float outputMax;
} PID_Regulator_t;

void PID_Reset(PID_Regulator_t *PIDInfo);
float PID_PIDCalc(PID_Regulator_t *PIDInfo, float target,float feedback);
float PID_PIDCalc_max(PID_Regulator_t *PIDInfo, float target,float feedback,float max);


#endif //RM_FRAME_C_PID_H
