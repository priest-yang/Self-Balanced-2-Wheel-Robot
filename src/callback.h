#ifndef SRC_CALLBACK_H
#define SRC_CALLBACK_H

#include "definitions.h"

#define INRANGE(NUM, MIN, MAX) \
{\
    if(NUM<MIN){\
        NUM=MIN;\
    }else if(NUM>MAX){\
        NUM=MAX;\
    }\
}

void CallbackRegister_All();

void MainControlLoop(uint32_t status, uintptr_t context);

void CAN1ReceiveHandle(uint32_t status, uintptr_t context);

void CAN2ReceiveHandle(uint32_t status, uintptr_t context);

void ADCHandle();

void Delay_(uint16_t ms);

float u8Arry2float(uint8_t *data);

#endif