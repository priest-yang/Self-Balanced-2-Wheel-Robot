#include "BMI088Middleware.h"
#include "definitions.h"
#include "callback.h"

void BMI088_GPIO_init(void)
{

}

void BMI088_com_init(void)
{


}

void BMI088_delay_ms(uint16_t ms)
{
    Delay_(ms);
}

void BMI088_delay_us(uint16_t us)
{
    //CORETIMER_DelayUs(us);
    uint16_t i;
    for (i = 0; i < us; i++ )
    {
        int a = 80;  //delay based on main clock, 168Mhz
        while (a-- );
    }
}




void BMI088_ACCEL_NS_L(void)
{
    
    CS1_ACC_Clear();
}
void BMI088_ACCEL_NS_H(void)
{
    CS1_ACC_Set();
}

void BMI088_GYRO_NS_L(void)
{
    CS1_GYRO_Clear();
}
void BMI088_GYRO_NS_H(void)
{
    CS1_GYRO_Set();
}

uint8_t BMI088_read_write_byte(uint8_t txdata)
{
    uint8_t rx_data;
    SPI1_WriteRead(&txdata, 1, &rx_data, 1);
    return rx_data;
}

