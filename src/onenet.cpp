// 单片机头文件
#include "stm32f1xx.h"

// 网络设备
#include "esp8266.h"

// 协议文件
#include "onenet.h"

// 硬件驱动
#include "usart.h"
#include "delay.h"
#include "sht20.h"

// C库
#include <string.h>
#include <stdio.h>

#define DEVID "1052276362"

#define APIKEY "nfKOmB4F7x9c7v9wFyH0rnYgjvM="

void OneNet_FillBuf(char *buf)
{

	char text[24];
	char buf1[128];

	memset(text, 0, sizeof(text));
	memset(buf1, 0, sizeof(buf1));

	strcpy(buf1, "{");

	memset(text, 0, sizeof(text));
	sprintf(text, "\"Temperature\":%0.2f,", sht20_info.tempreture);
	strcat(buf1, text);

	memset(text, 0, sizeof(text));
	sprintf(text, "\"Humidity\":%0.2f", sht20_info.humidity);
	strcat(buf1, text);

	strcat(buf1, "}");

	sprintf(buf, "POST /devices/%s/datapoints?type=3 HTTP/1.1\r\napi-key:%s\r\nHost:api.heclouds.com\r\n"
							 "Content-Length:%d\r\n\r\n",

					DEVID, APIKEY, strlen(buf1));

	strcat(buf, buf1);
}

void OneNet_SendData(void)
{

	char buf[256];

	memset(buf, 0, sizeof(buf));

	OneNet_FillBuf(buf); // 封装数据流

	ESP8266_SendData((unsigned char *)buf, strlen(buf)); // 上传数据
}

void OneNet_RevPro(unsigned char *dataPtr)
{

	if (strstr((char *)dataPtr, "CLOSED"))
	{
		UsartPrintf(USART_DEBUG, "TCP CLOSED\r\n");
	}
	else
	{
		// 这里用来检测是否发送成功
		if (strstr((char *)dataPtr, "succ"))
		{
			UsartPrintf(USART_DEBUG, "Tips:	Send OK\r\n");
		}
		else
		{
			UsartPrintf(USART_DEBUG, "Tips:	Send Err\r\n");
		}
	}

	ESP8266_Clear();
}
