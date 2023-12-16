#ifndef _SRC_MOTOR_H
#define _SRC_MOTOR_H

#include "callback.h"

#define CanRxGetU16(canRxMsg, num) (((uint16_t)canRxMsg[num * 2] << 8) | (uint16_t)canRxMsg[num * 2 + 1])

typedef struct {
    int16_t angle;
    int16_t speed;
    int16_t moment;
    int16_t temp;
}C6x0Rx_t;

typedef struct {
    C6x0Rx_t feedback;
    int16_t intensity;
}Motor_t;

void MotorTxHandle();

void MotorPutIntensity(Motor_t *_motor, int16_t _intensity);

#endif