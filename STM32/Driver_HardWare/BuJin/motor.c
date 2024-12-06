#include "motor.h"

volatile bool motorRunning = false;

void Motor_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_4|GPIO_Pin_5 | GPIO_Pin_6;    //PA3,PA4,PA5,PA6控制步进驱动引脚
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
}


void Motor_Stop(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_7|GPIO_Pin_4 |GPIO_Pin_5| GPIO_Pin_6);
}

void Motor_Two_Forward(uint16_t speed)
{			
	GPIO_SetBits(GPIOA,GPIO_Pin_4|GPIO_Pin_5);
	GPIO_ResetBits(GPIOA,GPIO_Pin_6 | GPIO_Pin_7);	
	vTaskDelay(speed);
	
	GPIO_SetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_6);
	GPIO_ResetBits(GPIOA,GPIO_Pin_7 | GPIO_Pin_4);	
	vTaskDelay(speed);
	
	GPIO_SetBits(GPIOA,GPIO_Pin_6|GPIO_Pin_7);
	GPIO_ResetBits(GPIOA,GPIO_Pin_4 | GPIO_Pin_5);	
	vTaskDelay(speed);	
	
	GPIO_SetBits(GPIOA,GPIO_Pin_7|GPIO_Pin_4);
	GPIO_ResetBits(GPIOA,GPIO_Pin_5 | GPIO_Pin_6);	
	vTaskDelay(speed);
}

void Motor_Two_ForwardLoop(uint16_t Speed, uint8_t Period)
{
	for (int i = 0; i <= (Period * 64); i++) // 外轴减速比1：64，还需要*64  4次1个360° 8次2个360°
	{
		Motor_Two_Forward(Speed);
	}
}

void Motor_Two_Reverse(uint16_t speed)
{	
	GPIO_SetBits(GPIOA,GPIO_Pin_4|GPIO_Pin_7);
	GPIO_ResetBits(GPIOA,GPIO_Pin_6 | GPIO_Pin_5);	
	vTaskDelay(speed);	

	GPIO_SetBits(GPIOA,GPIO_Pin_7|GPIO_Pin_6);
	GPIO_ResetBits(GPIOA,GPIO_Pin_5 | GPIO_Pin_4);	
	vTaskDelay(speed);

	GPIO_SetBits(GPIOA,GPIO_Pin_6|GPIO_Pin_5);
	GPIO_ResetBits(GPIOA,GPIO_Pin_3 | GPIO_Pin_5);	
	vTaskDelay(speed);

	GPIO_SetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_4);
	GPIO_ResetBits(GPIOA,GPIO_Pin_7 | GPIO_Pin_6);	
	vTaskDelay(speed);
}


void Motor_Two_ReverseLoop(uint16_t Speed, uint8_t Period)
{
	for (int i = (Period * 64); i >= 0; i--) // 8次循环电机旋转一周，外轴减速比1：64，还需要*64
	{
		Motor_Two_Reverse(Speed);
	}
	 if (!motorRunning) Motor_Stop();
}