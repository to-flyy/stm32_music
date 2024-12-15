/* 系统头文件 */
#include "system.h"
#include "SysTick.h"
#include "Delay.h"
#include "string.h"

/* FreeRTOS头文件 */
#include "freertos.h"
#include "task.h"
#include "queue.h"

/* 硬件头文件 */
#include "LED.h"
#include "K210.h"
#include "motor.h"
#include "MP3.h"
#include "YuYin.h"
#include "LCD.h"


// 任务句柄
TaskHandle_t StartTask_Handler;
TaskHandle_t K210Task_Handler;
TaskHandle_t MP3Task_Handler;
TaskHandle_t MotorTask_Handler;
TaskHandle_t YuYinTask_Handler;
TaskHandle_t LCDTask_Handler;

// 队列句柄
QueueHandle_t K210Queue_Handler;
QueueHandle_t MP3Queue_Handler;
QueueHandle_t YuYinQueue_Handler;
QueueHandle_t LCDQueue_Handler;

// 队列返回值
BaseType_t K210Queue_Return;
BaseType_t MP3Queue_Return;
BaseType_t YuYinQueue_Return;
BaseType_t LCDQueue_Return;

// 声明函数
void Start_Task(void);
void K210_Task(void);
void MP3_Task(void);
void Motor_Task(void);
void YuYin_Task(void);
void LCD_Task(void);
void HardWare_Init(void);

/* main函数 */
int main()
{
	HardWare_Init();
	
	// 创建开始任务
    xTaskCreate((TaskFunction_t)Start_Task,          // 任务函数
                (const char *)"Start_Task",          // 任务名称
                (uint16_t)256,                       // 任务堆栈大小
                (void *)NULL,                        // 传递给任务函数的参数
                (UBaseType_t)1,                      // 任务优先级
                (TaskHandle_t *)&StartTask_Handler); // 任务句柄
				
	vTaskStartScheduler();
}

void Start_Task(void)
{
	/* 确保在创建任务和队列时不会被中断，以防止竞态条件 */
	taskENTER_CRITICAL(); // 进入临界区
	
	// 创建队列
	K210Queue_Handler = xQueueCreate((UBaseType_t)10,           		 
                                     (UBaseType_t)sizeof(K210_RxPacket));
	
	MP3Queue_Handler = xQueueCreate((UBaseType_t)10,           
                                    (UBaseType_t)sizeof(char));
	
	YuYinQueue_Handler = xQueueCreate((UBaseType_t)10,           		 
                                     (UBaseType_t)sizeof(YuYin_RxPacket));
	
	LCDQueue_Handler = xQueueCreate((UBaseType_t)10,           		 
                                     (UBaseType_t)sizeof(LCD_RxData));
	
										
	xTaskCreate((TaskFunction_t)K210_Task,
                (const char *)"K210_Task",
                (uint16_t)128,
                (void *)NULL,
                (UBaseType_t)3,
                (TaskHandle_t *)&K210Task_Handler); 
				
	xTaskCreate((TaskFunction_t)YuYin_Task,
                (const char *)"YuYin_Task",
                (uint16_t)128,
                (void *)NULL,
                (UBaseType_t)3,
                (TaskHandle_t *)&YuYinTask_Handler);
				
	xTaskCreate((TaskFunction_t)MP3_Task,
                (const char *)"MP3_Task",
                (uint16_t)256,
                (void *)NULL,
                (UBaseType_t)2,
                (TaskHandle_t *)&MP3Task_Handler);									   				
				
	xTaskCreate((TaskFunction_t)Motor_Task,
                (const char *)"Motor_Task",
                (uint16_t)256,
                (void *)NULL,
                (UBaseType_t)2,
                (TaskHandle_t *)&MotorTask_Handler);				
				
	xTaskCreate((TaskFunction_t)LCD_Task,
                (const char *)"LCD_Task",
                (uint16_t)128,
                (void *)NULL,
                (UBaseType_t)3,
                (TaskHandle_t *)&LCDTask_Handler);
				
	vTaskDelete(NULL);
	taskEXIT_CRITICAL(); // 退出临界区
}

void K210_Task(void)
{
	while(1)
	{
		 vTaskDelay(pdMS_TO_TICKS(100));		
	}
	vTaskDelete(NULL);
}

void YuYin_Task(void)
{
	while(1)
	{  								 
		vTaskDelay(pdMS_TO_TICKS(100));	
	}
	vTaskDelete(NULL);
}

void MP3_Task(void)
{	
	while(1)
	{
		// 接收队列返回值
		K210Queue_Return = xQueueReceive((QueueHandle_t)K210Queue_Handler,
                                         (void *const)&K210_RxPacket,    
                                         (TickType_t)0);   										   
		if (pdTRUE == K210Queue_Return)
		{
			if(strcmp(K210_RxPacket, K210_MUSIC_1) == 0)
			{
				MP3_Star(0, 0);
				isMP3Playing = true;
			}				
			else if (strcmp(K210_RxPacket, K210_MUSIC_2) == 0)
			{
				MP3_Star(1, 1);
				isMP3Playing = true;
			}				
			else if (strcmp(K210_RxPacket, K210_MUSIC_3) == 0)
			{
				MP3_Star(2, 2);
				isMP3Playing = true;	
			}				
		}
		
		YuYinQueue_Return = xQueueReceive((QueueHandle_t)YuYinQueue_Handler,
                                         (void *const)&YuYin_RxPacket,    
                                         (TickType_t)0);  										 
		if (pdTRUE == YuYinQueue_Return)
		{			
			Flag_LED = 0; 
			if (strcmp(YuYin_RxPacket,YuYin_MUSIC_1)==0)
			{
				MP3_Star(0, 0);
				isMP3Playing = true;
			}
			else if (strcmp(YuYin_RxPacket,YuYin_MUSIC_2)==0)
			{
				MP3_Star(1, 1);
				isMP3Playing = true;
			}
			else if (strcmp(YuYin_RxPacket,YuYin_MUSIC_3)==0)
			{
				MP3_Star(2, 2);
				isMP3Playing = true;
			}
			else if (strcmp(YuYin_RxPacket,YuYin_MUSIC_PAUSE)==0)
			{
				MP3_Stop();
				isMP3Playing = false;
			}
			else if (strcmp(YuYin_RxPacket,YuYin_MUSIC_CONTINUE)==0)
			{
				MP3_Continue();
                isMP3Playing = true;
			}
		}
		
		LCDQueue_Return = xQueueReceive((QueueHandle_t)LCDQueue_Handler,
                                         (void *const)&LCD_RxData,    
                                         (TickType_t)0);  
		if (pdTRUE == LCDQueue_Return)
		{
			if (LCD_RxData == LCD_MUSIC_1)
			{
				MP3_Star(0, 0);
				isMP3Playing = true;
			}
			else if (LCD_RxData == LCD_MUSIC_2)
			{
				MP3_Star(1, 1);
				isMP3Playing = true;
			}
			else if (LCD_RxData == LCD_MUSIC_STOP)
			{
				if(isMP3Playing)
				{
					MP3_Stop();
					isMP3Playing = false;
				}				
				else
				{
					MP3_Continue();
					isMP3Playing = true;
				}
			}
			else if (LCD_RxData == LCD_MUSIC_LAST)
			{
				MP3_Last();
			}
			else if (LCD_RxData == LCD_MUSIC_NEXT)
			{
				MP3_Next();
			}
		}
	}
	vTaskDelete(NULL);
}

void Motor_Task(void)
{	
	while(1)
	{	
		if (isMP3Playing)
			Motor_Two_Forward(5);
		else 
			Motor_Stop();
	}
	vTaskDelete(NULL);
}

void LCD_Task(void)
{
	while(1)
	{
		vTaskDelay(pdMS_TO_TICKS(100));	
	}
	vTaskDelete(NULL);
}

void HardWare_Init(void)
{
	SysTick_Init(72);
	MP3_USART_Init(9600);
	YuYin_Init(9600);
	LCD_Init(115200);
	K210_Init(9600);
	Motor_Init();	
	LED_Init();
}