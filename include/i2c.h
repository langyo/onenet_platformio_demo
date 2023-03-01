#ifndef _I2C_H_
#define _I2C_H_

#define IIC_OK 0

#define IIC_Err 1

// SDA		PB11
// SCL		PB10
#define SDA_H HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET)
#define SDA_L HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET)
#define SDA_R HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11)

#define SCL_H HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET)
#define SCL_L HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET)

typedef struct
{

	unsigned short speed;

} IIC_INFO;

extern IIC_INFO iic_info;

void IIC_Init(void);

void IIC_SpeedCtl(unsigned short speed);

bool I2C_WriteByte(unsigned char slaveAddr, unsigned char regAddr, unsigned char *byte);

bool I2C_ReadByte(unsigned char slaveAddr, unsigned char regAddr, unsigned char *val);

bool I2C_WriteBytes(unsigned char slaveAddr, unsigned char regAddr, unsigned char *buf, unsigned char num);

bool I2C_ReadBytes(unsigned char slaveAddr, unsigned char regAddr, unsigned char *buf, unsigned char num);

void IIC_Start(void);

void IIC_Stop(void);

bool IIC_WaitAck(unsigned int timeOut);

void IIC_Ack(void);

void IIC_NAck(void);

void IIC_SendByte(unsigned char byte);

unsigned char IIC_RecvByte(void);

#include "../lib/i2c.cpp"
#endif
