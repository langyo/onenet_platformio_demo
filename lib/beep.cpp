// 单片机头文件
#include "stm32f1xx.h"

// 硬件驱动
#include "beep.h"

BEEP_INFO beep_info = {0};

void Beep_Init(void)
{

	GPIO_InitTypeDef gpio_initstruct;

	__HAL_RCC_GPIOB_CLK_ENABLE(); // 打开GPIOB的时钟

	SET_BIT(RCC->APB2ENR, ENABLE); // 打开复用功能时钟
	// GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); // 禁止JTAG功能

	gpio_initstruct.Mode = GPIO_MODE_OUTPUT_PP; // 设置为输出
	gpio_initstruct.Pin = GPIO_PIN_3;						// 将初始化的Pin脚
	gpio_initstruct.Speed = GPIO_SPEED_MEDIUM;	// 可承载的最大频率, 50MHz

	HAL_GPIO_Init(GPIOB, &gpio_initstruct); // 初始化GPIOB

	Beep_Set(BEEP_OFF); // 初始化完成后，关闭蜂鸣器
}

void Beep_Set(bool status)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, status == BEEP_ON ? GPIO_PIN_SET : GPIO_PIN_RESET);

	beep_info.Beep_Status = status;
}
