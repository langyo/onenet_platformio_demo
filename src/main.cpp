#define DEVICE_ID "xxxxxx"
#define DEVICE_API_KEY "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
#define DEVICE_HOST "api.heclouds.com"

#define WIFI_SSID "xxxxxx"
#define WIFI_PWD "xxxxxx"

// 单片机头文件
#include "stm32f1xx.h"

// 网络协议层
#include "onenet.h"

// 网络设备
#include "esp8266.h"

// 硬件驱动
#include "delay.h"
#include "beep.h"
#include "usart.h"
#include "i2c.h"
#include "sht20.h"

// C库
#include <string.h>

void Hardware_Init(void)
{
	NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_2); // 中断控制器分组设置

	Delay_Init(); // systick初始化

	Usart1_Init(115200); // 串口1，打印信息用

	Usart2_Init(115200); // 串口2，驱动ESP8266用

	IIC_Init(); // IIC初始化

	Beep_Init(); // 蜂鸣器初始化

	UsartPrintf(USART_DEBUG, " Hardware init OK\r\n");
}

void SysTick_Handler(void)
{
	/* USER CODE BEGIN SysTick_IRQn 0 */

	/* USER CODE END SysTick_IRQn 0 */
	HAL_IncTick();
	/* USER CODE BEGIN SysTick_IRQn 1 */

	/* USER CODE END SysTick_IRQn 1 */
}

int main()
{

	HAL_Init();

	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitTypeDef gpiot;
	gpiot.Mode = GPIO_MODE_OUTPUT_PP;
	gpiot.Pin = GPIO_PIN_7 | GPIO_PIN_8;
	gpiot.Pull = GPIO_NOPULL;
	gpiot.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOC, &gpiot);

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);

	SysTick_Handler();

	while (1)
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
		HAL_Delay(1000);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
		HAL_Delay(1000);

		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
	}

	unsigned short timeCount = 0; // 发送间隔变量

	unsigned char *dataPtr = nullptr;

	Hardware_Init(); // 初始化外围硬件

	ESP8266_Init(); // 初始化ESP8266

	Beep_Set(BEEP_ON); // 鸣叫提示接入成功
	DelayXms(250);
	Beep_Set(BEEP_OFF);

	while (1)
	{

		if (++timeCount >= 500) // 发送间隔5s
		{
			SHT20_GetValue();

			UsartPrintf(USART_DEBUG, "OneNet_SendData\r\n");

			OneNet_SendData();

			timeCount = 0;
			ESP8266_Clear();
		}

		dataPtr = ESP8266_GetIPD(0);
		if (dataPtr != nullptr)
			OneNet_RevPro(dataPtr);

		DelayXms(10);
	}
}
