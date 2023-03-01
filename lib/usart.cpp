// 硬件驱动
#include "usart.h"
#include "delay.h"

// C库
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

void Usart1_Init(unsigned int baud)
{

	GPIO_InitTypeDef gpio_initstruct;
	USART_HandleTypeDef usart_initstruct;

	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN);	 // 使能GPIOA
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1EN); // 使能USART1

	// PA9	TXD
	gpio_initstruct.Mode = GPIO_MODE_AF_PP;
	gpio_initstruct.Pin = GPIO_PIN_9;
	gpio_initstruct.Speed = GPIO_SPEED_MEDIUM; // 50MHz
	HAL_GPIO_Init(GPIOA, &gpio_initstruct);

	// PA10	RXD
	gpio_initstruct.Mode = GPIO_MODE_AF_INPUT;
	gpio_initstruct.Pin = GPIO_PIN_10;
	gpio_initstruct.Speed = GPIO_SPEED_MEDIUM; // 50MHz
	HAL_GPIO_Init(GPIOA, &gpio_initstruct);

	usart_initstruct.Instance = USART1;
	usart_initstruct.Init.BaudRate = baud;
	usart_initstruct.Init.Mode = USART_MODE_TX_RX;					// 接收和发送
	usart_initstruct.Init.Parity = USART_PARITY_NONE;				// 无校验
	usart_initstruct.Init.StopBits = USART_STOPBITS_1;			// 1位停止位
	usart_initstruct.Init.WordLength = USART_WORDLENGTH_8B; // 8位数据位
	HAL_USART_Init(&usart_initstruct);

	__HAL_USART_ENABLE(&usart_initstruct); // 使能串口

	__HAL_USART_ENABLE_IT(&usart_initstruct, USART_IT_RXNE); // 使能接收中断

	HAL_NVIC_SetPendingIRQ(USART1_IRQn);
	HAL_NVIC_SetPriority(USART1_IRQn, 0, 2);
	HAL_NVIC_EnableIRQ(USART1_IRQn);
}

void Usart2_Init(unsigned int baud)
{

	GPIO_InitTypeDef gpio_initstruct;
	USART_HandleTypeDef usart_initstruct;

	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN);	 // 使能GPIOA
	SET_BIT(RCC->APB1ENR, RCC_APB1ENR_USART2EN); // 使能USART2

	// PA2	TXD
	gpio_initstruct.Mode = GPIO_MODE_AF_PP;
	gpio_initstruct.Pin = GPIO_PIN_2;
	gpio_initstruct.Speed = GPIO_SPEED_MEDIUM; // 50MHz
	HAL_GPIO_Init(GPIOA, &gpio_initstruct);

	// PA3	RXD
	gpio_initstruct.Mode = GPIO_MODE_AF_INPUT;
	gpio_initstruct.Pin = GPIO_PIN_3;
	gpio_initstruct.Speed = GPIO_SPEED_MEDIUM; // 50MHz
	HAL_GPIO_Init(GPIOA, &gpio_initstruct);

	usart_initstruct.Instance = USART1;
	usart_initstruct.Init.BaudRate = baud;
	usart_initstruct.Init.Mode = USART_MODE_TX_RX;					// 接收和发送
	usart_initstruct.Init.Parity = USART_PARITY_NONE;				// 无校验
	usart_initstruct.Init.StopBits = USART_STOPBITS_1;			// 1位停止位
	usart_initstruct.Init.WordLength = USART_WORDLENGTH_8B; // 8位数据位
	HAL_USART_Init(&usart_initstruct);

	__HAL_USART_ENABLE(&usart_initstruct); // 使能串口

	__HAL_USART_ENABLE_IT(&usart_initstruct, USART_IT_RXNE); // 使能接收中断

	HAL_NVIC_SetPendingIRQ(USART2_IRQn);
	HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(USART2_IRQn);
}

void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len)
{

	unsigned short count = 0;

	for (; count < len; count++)
	{
		SET_BIT(USARTx->DR, (*str) & (uint16_t)0x01FF); // 发送数据
		str += 1;
		while (READ_BIT(USARTx->SR, USART_SR_TC) == RESET)
			; // 等待发送完成
	}
}

void UsartPrintf(USART_TypeDef *USARTx, const char *str)
{
	while (*str != 0)
	{
		SET_BIT(USARTx->DR, (*str) & (uint16_t)0x01FF); // 发送数据
		str += 1;
		while (READ_BIT(USARTx->SR, USART_SR_TC) == RESET)
			; // 等待发送完成
	}
}

void USART1_IRQHandler()
{

	if (READ_BIT(USART1->SR, USART_SR_TC) != RESET) // 接收中断
	{
		SET_BIT(USART1->SR, ~USART_SR_TC); // 清除中断标志
	}
}
