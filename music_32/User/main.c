#include "system.h"
#include "SysTick.h"
#include "Delay.h"
#include "string.h"

#include "freertos.h"
#include "task.h"
#include "queue.h"

#include "LED.h"
#include "Key.h"
#include "K210.h"
#include "motor.h"
#include "MP3.h"
#include "YuYin.h"

#define CMD_MUSIC_1 "ZJL"
#define CMD_MUSIC_2 "JW"
#define CMD_MUSIC_3 "FZU"
#define CMD_PLAY	"PLAY"
#define CMD_STOP	"STOP"

typedef struct {
    char* command;  // 命令字符串
} MotorCommand;

// 任务句柄
TaskHandle_t StartTask_Handler;
TaskHandle_t K210Task_Handler;
TaskHandle_t MP3Task_Handler;
TaskHandle_t LEDTask_Handler;
TaskHandle_t MotorTask_Handler;
TaskHandle_t KeyTask_Handler;
TaskHandle_t YuYinTask_Handler;

// 队列句柄
QueueHandle_t K210Queue_Handler;
QueueHandle_t MP3Queue_Handler;
QueueHandle_t YuYinQueue_Handler;

// 队列返回值
BaseType_t K210Queue_Return;
BaseType_t MP3Queue_Return;
BaseType_t YuYinQueue_Return;

// 声明函数
void Start_Task(void);
void K210_Task(void);
void MP3_Task(void);
void LED_Task(void);
void Motor_Task(void);
void Key_Task(void);
void YuYin_Task(void);

u8 KeyNum;

int main()
{
	SysTick_Init(72);
	K210_Init(115200);	
	MP3_USART_Init(9600);
	YuYin_Init(9600);
	LED_Init();
	Key_Init();
	Motor_Init();
	
	// 创建开始任务
    xTaskCreate((TaskFunction_t)Start_Task,          // 任务函数
                (const char *)"Start_Task",          // 任务名称
                (uint16_t)128,                       // 任务堆栈大小
                (void *)NULL,                        // 传递给任务函数的参数
                (UBaseType_t)1,                      // 任务优先级
                (TaskHandle_t *)&StartTask_Handler); // 任务句柄
				
	vTaskStartScheduler();
}

void Start_Task(void)
{
	taskENTER_CRITICAL(); // 进入临界区
	
	// 创建队列
	K210Queue_Handler = xQueueCreate((UBaseType_t)10,           		 
                                     (UBaseType_t)sizeof(K210_RxPacket));
	
	MP3Queue_Handler = xQueueCreate((UBaseType_t)10,           
                                    (UBaseType_t)sizeof(MotorCommand));
	
	YuYinQueue_Handler = xQueueCreate((UBaseType_t)10,           		 
                                     (UBaseType_t)sizeof(YuYin_RxPacket));
	
										
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
									   				
	xTaskCreate((TaskFunction_t)LED_Task,
                (const char *)"LED_Task",
                (uint16_t)128,
                (void *)NULL,
                (UBaseType_t)3,
                (TaskHandle_t *)&LEDTask_Handler);
				
	xTaskCreate((TaskFunction_t)Motor_Task,
                (const char *)"Motor_Task",
                (uint16_t)256,
                (void *)NULL,
                (UBaseType_t)2,
                (TaskHandle_t *)&MotorTask_Handler);
				
	xTaskCreate((TaskFunction_t)Key_Task,
                (const char *)"Key_Task",
                (uint16_t)256,
                (void *)NULL,
                (UBaseType_t)2,
                (TaskHandle_t *)&KeyTask_Handler);
				
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
	MotorCommand cmd;
	
	while(1)
	{
		// 接收队列返回值
		K210Queue_Return = xQueueReceive((QueueHandle_t)K210Queue_Handler,
                                         (void *const)&K210_RxPacket,    
                                         (TickType_t)0);   
										   
		if (pdTRUE == K210Queue_Return)
		{
			if(strcmp(K210_RxPacket, CMD_MUSIC_1) == 0)
			{
				MP3_Star(0, 0);
				LED1 = 1;
				isMP3Playing = true;
			}				
			else if (strcmp(K210_RxPacket, CMD_MUSIC_2) == 0)
			{
				MP3_Star(1, 1);
				LED2 = 1;
				isMP3Playing = true;
			}				
			else if (strcmp(K210_RxPacket, CMD_MUSIC_3) == 0)
			{
				MP3_Star(2, 2);
				LED3 = 1;
				isMP3Playing = true;	
			}				
		}
		
		YuYinQueue_Return = xQueueReceive((QueueHandle_t)YuYinQueue_Handler,
                                         (void *const)&YuYin_RxPacket,    
                                         (TickType_t)0);  
										 
		if (pdTRUE == YuYinQueue_Return)
		{			
			Flag_LED = 0; 
			if (strcmp(YuYin_RxPacket,"ZJL_ON")==0)
			{
				MP3_Star(0, 0);
				LED1 = 1;
				isMP3Playing = true;
			}
			else if (strcmp(YuYin_RxPacket,"JW_ON")==0)
			{
				MP3_Star(1, 1);
				LED2 = 1;
				isMP3Playing = true;
			}
			else if (strcmp(YuYin_RxPacket,"FZU_ON")==0)
			{
				MP3_Star(2, 2);
				LED3 = 1;
				isMP3Playing = true;
			}
			else if (strcmp(YuYin_RxPacket,"PAUSE")==0)
			{
				MP3_Stop();
				isMP3Playing = false;
			}
			else if (strcmp(YuYin_RxPacket,"CONTINUE")==0)
			{
				MP3_Continue();
                isMP3Playing = true;
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


void LED_Task(void)
{
	while(1)
	{
		vTaskDelay(pdMS_TO_TICKS(100));	
//		Flag_LED = 1;
//		vTaskDelay(500);
//		Flag_LED = 0;
//		vTaskDelay(500);
	}
	vTaskDelete(NULL);
}

void Key_Task(void)
{
	while(1)
	{
		KeyNum = Key_GetNum();
		if (KeyNum == 2)
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
		else if (KeyNum == 1)
		{
			MP3_Last();
		}
		else if (KeyNum == 3)
		{
			MP3_Next();
		}
		vTaskDelay(100);
	}
	vTaskDelete(NULL);
}