#include "callback.h"
#include "IMU.h"
#include "motor.h"
#include "MusicAutoPlay.h"
#include "Lib_songs.h"

typedef enum{
    UP_POS = 1u,MID_POS = 3u,DOWN_POS = 2u
}SWITCH_STATE_E;

uint8_t TxData[8];

static uint8_t can1_rx_message[8];
static uint32_t can1_rx_messageID = 0;
static uint8_t can1_rx_messageLength = 0;
static uint16_t can1_timestamp = 0;
static CAN_MSG_RX_ATTRIBUTE can1_msgAttr = CAN_MSG_RX_DATA_FRAME;

static uint8_t can2_rx_message[8];
static uint32_t can2_rx_messageID = 0;
static uint8_t can2_rx_messageLength = 0;
static uint16_t can2_timestamp = 0;
static CAN_MSG_RX_ATTRIBUTE can2_msgAttr = CAN_MSG_RX_DATA_FRAME;


volatile uint32_t msElapsed = 0;
uint8_t secElapsed = 0;
CAN_FIFO_INTERRUPT_FLAG_MASK canint;




extern Motor_t M_Left;
extern Motor_t M_Right;

float right_col = 0;
float left_rol = 0;
SWITCH_STATE_E sRight = DOWN_POS;

PID_Regulator_t speed_PID = {
    .kp = 0.01,
    .ki = 0.01,
    .kd = 0,
    .componentKpMax = 16384,
    .componentKiMax = 1.5,
    .componentKdMax = 0,
    .outputMax = 40
};
PID_Regulator_t stand_PID_spd = {
    .kp = 110,
    .ki = 0,
    .kd = 10000,
    .componentKpMax = 16384,
    .componentKiMax = 0,
    .componentKdMax = 16000,
    .outputMax = 16000
};
PID_Regulator_t follow_PID = {
    .kp = 5,
    .ki = 0,
    .kd = 0,
    .componentKpMax = 3000.0,
    .componentKiMax = 300,
    .componentKdMax = 3000.0,
    .outputMax = 5000
};
PID_Regulator_t motor_PID = {
    .kp = 15,
    .ki = 0,
    .kd = 0,
    .componentKpMax = 16384,
    .componentKiMax = 0,
    .componentKdMax = 0,
    .outputMax = 16384
};

float speed_feedback;
float rotate_speed_feedback;
float speed_output;
float intensity;
float rotate_intensity;
float pitch;
float protect_1;
float protect_2;
uint8_t stop_flag;
uint32_t stop_time;

T_Song *cur_Song = NULL;

float vbat;

void CallbackRegister_All()
{
    TMR1_CallbackRegister((TMR1_CALLBACK)&MainControlLoop, (uintptr_t)NULL);

    CAN1_CallbackRegister((CAN_CALLBACK)&CAN1ReceiveHandle, (uintptr_t)NULL, 1);
    CAN1_MessageReceive(&can1_rx_messageID, &can1_rx_messageLength, can1_rx_message, &can1_timestamp, 1, &can1_msgAttr);

    CAN2_CallbackRegister((CAN_CALLBACK)&CAN2ReceiveHandle, (uintptr_t)NULL, 1);
    CAN2_MessageReceive(&can2_rx_messageID, &can2_rx_messageLength, can2_rx_message, &can2_timestamp, 1, &can2_msgAttr);

    cur_Song = &SuperMario;
}

void MainControlLoop(uint32_t status, uintptr_t context)
{
    msElapsed++;
    if (msElapsed % 1000 == 0)
    {
        GPIO_RB5_LED3_Toggle();
        GPIO_RB4_LED2_Clear();
        secElapsed++;
    }

    speed_feedback = 0.1f * (M_Right.feedback.speed - M_Left.feedback.speed) + 0.9f * speed_feedback;
    rotate_speed_feedback = 0.1f * (M_Right.feedback.speed + M_Left.feedback.speed) + 0.9f * rotate_speed_feedback;
    speed_output = PID_PIDCalc(&speed_PID, right_col * 3000.0f, speed_feedback); // 3000.0f
    intensity = PID_PIDCalc(&stand_PID_spd, -speed_output, pitch);
    rotate_intensity = PID_PIDCalc(&follow_PID, -left_rol * 400.0f, rotate_speed_feedback);

    protect_2 = abs(M_Left.feedback.speed) + abs(M_Right.feedback.speed);

    if (((protect_1 * 250.0f) + protect_2 * 1.5f) > 16000.0f) {
        stop_flag = 1;
        stop_time = msElapsed;
    }

    if (msElapsed - stop_time > 2000){
        stop_flag = 0;
    }

    if ((abs(pitch) < 80 || 1) && sRight == UP_POS && !stop_flag) { // 10000 for each
        MotorPutIntensity(&M_Left, -intensity + rotate_intensity);
        MotorPutIntensity(&M_Right, intensity + rotate_intensity);
    } else if (sRight == MID_POS || stop_flag) {
        speed_feedback = 0;
        MotorPutIntensity(&M_Left, PID_PIDCalc(&motor_PID, 0, M_Left.feedback.speed));
        MotorPutIntensity(&M_Right, PID_PIDCalc(&motor_PID, 0, M_Right.feedback.speed));
    } else {
        MotorPutIntensity(&M_Left, 0);
        MotorPutIntensity(&M_Right, 0);
    }

    MotorTxHandle();

    //canint = *(volatile uint32_t *)(&C1FIFOINT0 + (1 * (0x10U)));

    UART1_Write(TxData, 8);

    ADCHandle();

    Music_auto_play();
}

void CAN1ReceiveHandle(uint32_t status, uintptr_t context)
{
    if (can1_rx_messageID == 0x201) {
        M_Left.feedback.angle = CanRxGetU16(can1_rx_message, 0);
        M_Left.feedback.speed = CanRxGetU16(can1_rx_message, 1);
        M_Left.feedback.moment = CanRxGetU16(can1_rx_message, 2);
    } else if (can1_rx_messageID == 0x202) {
        M_Right.feedback.angle = CanRxGetU16(can1_rx_message, 0);
        M_Right.feedback.speed = CanRxGetU16(can1_rx_message, 1);
        M_Right.feedback.moment = CanRxGetU16(can1_rx_message, 2);
    }
    memset(can1_rx_message, 0x00, sizeof(can1_rx_message));
    CAN1_MessageReceive(&can1_rx_messageID, &can1_rx_messageLength, can1_rx_message, &can1_timestamp, 1, &can1_msgAttr);
}

void CAN2ReceiveHandle(uint32_t status, uintptr_t context)
{
    if (can2_rx_messageID == 0x121) {
        pitch = u8Arry2float(can2_rx_message);
        protect_1 = u8Arry2float(can2_rx_message + 4);
    } else if (can2_rx_messageID == 0x122) {
        right_col = u8Arry2float(can2_rx_message);
        left_rol = u8Arry2float(can2_rx_message + 4);
    } else if (can2_rx_messageID == 0x123) {
        sRight = can2_rx_message[0];
    }
    memset(can2_rx_message, 0x00, sizeof(can2_rx_message));
    CAN2_MessageReceive(&can2_rx_messageID, &can2_rx_messageLength, can2_rx_message, &can2_timestamp, 1, &can2_msgAttr);
}

void ADCHandle(){
    ADC_InputScanSelect(ADC_INPUT_SCAN_AN0);
    if (msElapsed % 500 == 0)
    {
        if (vbat < 22.0f && vbat > 15.0f)
        {
            cur_Song = &A;
        }
        ADC_SamplingStart();
    } else if (msElapsed % 500 == 250)
    {
        ADC_ConversionStart();
    }
    if (ADC_ResultIsReady()){
        uint32_t a = ADC_ResultGet(ADC_RESULT_BUFFER_0);
        vbat = a * 3.3f * 11.0f / 1023.0f;
    }
}

void Delay_(uint16_t ms)
{
    uint16_t start_time = msElapsed;
    while (msElapsed < ms + start_time)
    {
    }
}

float u8Arry2float(uint8_t *data) {
    float fa = 0;
    uint8_t uc[4];

    uc[0] = data[0];
    uc[1] = data[1];
    uc[2] = data[2];
    uc[3] = data[3];

    memcpy(&fa, uc, 4);
    return fa;
}
