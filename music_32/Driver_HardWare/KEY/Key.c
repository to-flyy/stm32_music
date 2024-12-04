#include "Key.h"                  // Device header


void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

uint8_t Key_GetNum(void)     //unsigned char 
{
	uint8_t KeyNum = 0;
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)==0)
	{
		vTaskDelay(20);
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)==0);
		vTaskDelay(20);
		KeyNum = 1;
	}
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)==0)
	{
		vTaskDelay(20);
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)==0);
		vTaskDelay(20);
		KeyNum = 2;
	}
	
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7)==0)
	{
		vTaskDelay(20);
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7)==0);
		vTaskDelay(20);
		KeyNum = 3;
	}
		
	return KeyNum;
}

