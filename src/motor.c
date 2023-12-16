#include "motor.h"

Motor_t M_Left;
Motor_t M_Right;

void MotorTxHandle(){
    uint8_t TxData[8];

    TxData[0] = (uint8_t) (M_Left.intensity >> 8);
    TxData[1] = (uint8_t) (M_Left.intensity);
    TxData[2] = (uint8_t) (M_Right.intensity >> 8);
    TxData[3] = (uint8_t) (M_Right.intensity);

    CAN1_MessageTransmit(0x200, 8, TxData, 0u, 0);
}

void MotorPutIntensity(Motor_t *_motor, int16_t _intensity){
    if(_intensity > 16384) _intensity = 16384;
    if(_intensity < -16384) _intensity = -16384;
    _motor->intensity = _intensity;
}
