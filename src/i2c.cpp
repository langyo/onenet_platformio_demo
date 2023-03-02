// 单片机头文件
#include "stm32f1xx.h"

// 硬件驱动
#include "i2c.h"
#include "delay.h"
#include "usart.h"

IIC_INFO iic_info;

void IIC_SpeedCtl(unsigned short speed)
{

	iic_info.speed = speed;
}

void IIC_Init(void)
{

	GPIO_InitTypeDef gpio_initstruct;

	__HAL_RCC_GPIOB_CLK_ENABLE();
	gpio_initstruct.Mode = GPIO_MODE_OUTPUT_OD; // 开漏，这样不用去切换输出输入方向
	gpio_initstruct.Pin = GPIO_PIN_10 | GPIO_PIN_11;
	gpio_initstruct.Speed = GPIO_SPEED_MEDIUM; // 50MHz
	HAL_GPIO_Init(GPIOB, &gpio_initstruct);

	IIC_SpeedCtl(5);

	SDA_H; // 拉高SDA线，处于空闲状态
	SCL_H; // 拉高SCL线，处于空闲状态
}

void IIC_Start(void)
{

	SDA_H;									 // 拉高SDA线
	SCL_H;									 // 拉高SCL线
	DelayUs(iic_info.speed); // 延时，速度控制

	SDA_L;									 // 当SCL线为高时，SDA线一个下降沿代表开始信号
	DelayUs(iic_info.speed); // 延时，速度控制
	SCL_L;									 // 钳住SCL线，以便发送数据
}

void IIC_Stop(void)
{

	SDA_L;									 // 拉低SDA线
	SCL_L;									 // 拉低SCL先
	DelayUs(iic_info.speed); // 延时，速度控制

	SCL_H; // 拉高SCL线
	SDA_H; // 拉高SDA线，当SCL线为高时，SDA线一个上升沿代表停止信号
	DelayUs(iic_info.speed);
}

bool IIC_WaitAck(unsigned int timeOut)
{

	SDA_H;
	DelayUs(iic_info.speed); // 拉高SDA线
	SCL_H;
	DelayUs(iic_info.speed); // 拉高SCL线

	while (SDA_R) // 如果读到SDA线为1，则等待。应答信号应是0
	{
		if (--timeOut == 0)
		{
			UsartPrintf(USART1, "WaitAck TimeOut\r\n");

			IIC_Stop(); // 超时未收到应答，则停止总线

			return IIC_Err; // 返回失败
		}

		DelayUs(iic_info.speed);
	}

	SCL_L; // 拉低SCL线，以便继续收发数据

	return IIC_OK; // 返回成功
}

void IIC_Ack(void)
{

	SCL_L; // 拉低SCL线
	SDA_L; // 拉低SDA线
	DelayUs(iic_info.speed);
	SCL_H; // 拉高SCL线
	DelayUs(iic_info.speed);
	SCL_L; // 拉低SCL线
}

void IIC_NAck(void)
{

	SCL_L; // 拉低SCL线
	SDA_H; // 拉高SDA线
	DelayUs(iic_info.speed);
	SCL_H; // 拉高SCL线
	DelayUs(iic_info.speed);
	SCL_L; // 拉低SCL线
}

void IIC_SendByte(unsigned char byte)
{

	unsigned char count = 0;

	SCL_L; // 拉低时钟开始数据传输

	for (; count < 8; count++) // 循环8次，每次发送一个bit
	{
		if (byte & 0x80) // 发送最高位
			SDA_H;
		else
			SDA_L;

		byte <<= 1; // byte左移1位

		DelayUs(iic_info.speed);
		SCL_H;
		DelayUs(iic_info.speed);
		SCL_L;
	}
}

unsigned char IIC_RecvByte(void)
{

	unsigned char count = 0, receive = 0;

	SDA_H; // 拉高SDA线，开漏状态下，需线拉高以便读取数据

	for (; count < 8; count++) // 循环8次，每次发送一个bit
	{
		SCL_L;
		DelayUs(iic_info.speed);
		SCL_H;

		receive <<= 1; // 左移一位

		if (SDA_R) // 如果SDA线为1，则receive变量自增，每次自增都是对bit0的+1，然后下一次循环会先左移一次
			receive++;

		DelayUs(iic_info.speed);
	}

	return receive;
}

bool I2C_WriteByte(unsigned char slaveAddr, unsigned char regAddr, unsigned char *byte)
{

	unsigned char addr = 0;

	addr = slaveAddr << 1; // IIC地址是7bit，这里需要左移1位，bit0：1-读	0-写

	IIC_Start(); // 起始信号

	IIC_SendByte(addr);		 // 发送设备地址(写)
	if (IIC_WaitAck(5000)) // 等待应答
		return IIC_Err;

	IIC_SendByte(regAddr); // 发送寄存器地址
	if (IIC_WaitAck(5000)) // 等待应答
		return IIC_Err;

	if (byte)
	{
		IIC_SendByte(*byte);	 // 发送数据
		if (IIC_WaitAck(5000)) // 等待应答
			return IIC_Err;
	}

	IIC_Stop(); // 停止信号

	return IIC_OK;
}

bool I2C_ReadByte(unsigned char slaveAddr, unsigned char regAddr, unsigned char *val)
{

	unsigned char addr = 0;

	addr = slaveAddr << 1; // IIC地址是7bit，这里需要左移1位，bit0：1-读	0-写

	IIC_Start(); // 起始信号

	IIC_SendByte(addr);		 // 发送设备地址(写)
	if (IIC_WaitAck(5000)) // 等待应答
		return IIC_Err;

	IIC_SendByte(regAddr); // 发送寄存器地址
	if (IIC_WaitAck(5000)) // 等待应答
		return IIC_Err;

	IIC_Start(); // 重启信号

	IIC_SendByte(addr + 1); // 发送设备地址(读)
	if (IIC_WaitAck(5000))	// 等待应答
		return IIC_Err;

	*val = IIC_RecvByte(); // 接收
	IIC_NAck();						 // 产生一个非应答信号，代表读取接收

	IIC_Stop(); // 停止信号

	return IIC_OK;
}

bool I2C_WriteBytes(unsigned char slaveAddr, unsigned char regAddr, unsigned char *buf, unsigned char num)
{

	unsigned char addr = 0;

	addr = slaveAddr << 1; // IIC地址是7bit，这里需要左移1位，bit0：1-读	0-写

	IIC_Start(); // 起始信号

	IIC_SendByte(addr);		 // 发送设备地址(写)
	if (IIC_WaitAck(5000)) // 等待应答
		return IIC_Err;

	IIC_SendByte(regAddr); // 发送寄存器地址
	if (IIC_WaitAck(5000)) // 等待应答
		return IIC_Err;

	while (num--) // 循环写入数据
	{
		IIC_SendByte(*buf);		 // 发送数据
		if (IIC_WaitAck(5000)) // 等待应答
			return IIC_Err;

		buf++; // 数据指针偏移到下一个

		DelayUs(10);
	}

	IIC_Stop(); // 停止信号

	return IIC_OK;
}

bool I2C_ReadBytes(unsigned char slaveAddr, unsigned char regAddr, unsigned char *buf, unsigned char num)
{

	unsigned short addr = 0;

	addr = slaveAddr << 1; // IIC地址是7bit，这里需要左移1位，bit0：1-读	0-写

	IIC_Start(); // 起始信号

	IIC_SendByte(addr);		 // 发送设备地址(写)
	if (IIC_WaitAck(5000)) // 等待应答
		return IIC_Err;

	IIC_SendByte(regAddr); // 发送寄存器地址
	if (IIC_WaitAck(5000)) // 等待应答
		return IIC_Err;

	IIC_Start(); // 重启信号

	IIC_SendByte(addr + 1); // 发送设备地址(读)
	if (IIC_WaitAck(5000))	// 等待应答
		return IIC_Err;

	while (num--)
	{
		*buf = IIC_RecvByte();
		buf++; // 偏移到下一个数据存储地址

		if (num == 0)
		{
			IIC_NAck(); // 最后一个数据需要回NOACK
		}
		else
		{
			IIC_Ack(); // 回应ACK
		}
	}

	IIC_Stop();

	return IIC_OK;
}
