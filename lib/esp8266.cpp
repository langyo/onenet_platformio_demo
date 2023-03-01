// 单片机头文件
#include "stm32f1xx.h"

// 网络设备驱动
#include "esp8266.h"

// 硬件驱动
#include "delay.h"
#include "usart.h"

// C库
#include <string.h>
#include <stdio.h>

#define ESP8266_WIFI_INFO "AT+CWJAP=\"OrayBox-304\",\"bei301pea\"\r\n"

#define ESP8266_ONENET_INFO "AT+CIPSTART=\"TCP\",\"183.230.40.33\",80\r\n"

unsigned char esp8266_buf[256];
unsigned short esp8266_cnt = 0, esp8266_cntPre = 0;

void ESP8266_Clear(void)
{

	memset(esp8266_buf, 0, sizeof(esp8266_buf));
	esp8266_cnt = 0;
}

bool ESP8266_WaitRecive(void)
{

	if (esp8266_cnt == 0) // 如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
		return REV_WAIT;

	if (esp8266_cnt == esp8266_cntPre) // 如果上一次的值和这次相同，则说明接收完毕
	{
		esp8266_cnt = 0; // 清0接收计数

		return REV_OK; // 返回接收完成标志
	}

	esp8266_cntPre = esp8266_cnt; // 置为相同

	return REV_WAIT; // 返回接收未完成标志
}

bool ESP8266_SendCmd(char *cmd, char *res)
{

	unsigned char timeOut = 200;

	Usart_SendString(USART2, (unsigned char *)cmd, strlen((const char *)cmd));

	while (timeOut--)
	{
		if (ESP8266_WaitRecive() == REV_OK) // 如果收到数据
		{
			if (strstr((const char *)esp8266_buf, res) != nullptr) // 如果检索到关键词
			{
				ESP8266_Clear(); // 清空缓存

				return 0;
			}
		}

		DelayXms(10);
	}

	return 1;
}

void ESP8266_SendData(unsigned char *data, unsigned short len)
{

	char cmdBuf[32];

	ESP8266_Clear();													 // 清空接收缓存
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len); // 发送命令
	if (!ESP8266_SendCmd(cmdBuf, ">"))				 // 收到‘>’时可以发送数据
	{
		Usart_SendString(USART2, data, len); // 发送设备连接请求数据
	}
}

unsigned char *ESP8266_GetIPD(unsigned short timeOut)
{

	char *ptrIPD = nullptr;

	do
	{
		if (ESP8266_WaitRecive() == REV_OK) // 如果接收完成
		{
			ptrIPD = strstr((char *)esp8266_buf, "IPD,"); // 搜索“IPD”头
			if (ptrIPD == nullptr)												// 如果没找到，可能是IPD头的延迟，还是需要等待一会，但不会超过设定的时间
			{
				// UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
			}
			else
			{
				ptrIPD = strchr(ptrIPD, ':'); // 找到':'
				if (ptrIPD != nullptr)
				{
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
				}
				else
					return nullptr;
			}
		}

		DelayXms(5); // 延时等待
	} while (timeOut--);

	return nullptr; // 超时还未找到，返回空指针
}

void ESP8266_Init(void)
{

	GPIO_InitTypeDef GPIO_Initure;

	__HAL_RCC_GPIOB_CLK_ENABLE();

	// ESP8266复位引脚
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Initure.Pin = GPIO_PIN_14;					// GPIOC14-复位
	GPIO_Initure.Speed = GPIO_SPEED_MEDIUM; // 50MHz
	HAL_GPIO_Init(GPIOC, &GPIO_Initure);

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);
	DelayXms(250);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET);
	DelayXms(500);

	ESP8266_Clear();

	UsartPrintf(USART_DEBUG, "1. AT\r\n");
	while (ESP8266_SendCmd("AT\r\n", "OK"))
		DelayXms(500);

	UsartPrintf(USART_DEBUG, "2. CWMODE\r\n");
	while (ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK"))
		DelayXms(500);

	UsartPrintf(USART_DEBUG, "3. AT+CWDHCP\r\n");
	while (ESP8266_SendCmd("AT+CWDHCP=1,1\r\n", "OK"))
		DelayXms(500);

	UsartPrintf(USART_DEBUG, "4. CWJAP\r\n");
	while (ESP8266_SendCmd(ESP8266_WIFI_INFO, "GOT IP"))
		DelayXms(500);

	UsartPrintf(USART_DEBUG, "5. CIPSTART\r\n");
	while (ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT"))
		DelayXms(500);

	UsartPrintf(USART_DEBUG, "6. ESP8266 Init OK\r\n");
}

void USART2_IRQHandler(void)
{

	if ((READ_BIT(USART2->SR, USART_SR_RXNE)) != 0)
	{
		if (esp8266_cnt >= sizeof(esp8266_buf))
			esp8266_cnt = 0; // 防止串口被刷爆
		esp8266_buf[esp8266_cnt++] = USART2->DR;

		// 清零
		CLEAR_BIT(USART2->SR, USART_SR_RXNE);
	}
}
