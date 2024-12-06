#include "K210.h"

extern QueueHandle_t K210Queue_Handler;

char K210_RxPacket[100];

int fputc(int ch, FILE *p) // 函数默认的，在使用printf函数时自动调用
{
	USART_SendData(USART1, (u8)ch);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
		;
	return ch;
}

void K210_Init(uint32_t Baud)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	//TX
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	//RX
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	USART_InitTypeDef USART_InitStruct;
	USART_InitStruct.USART_BaudRate = Baud;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1,&USART_InitStruct);
	
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);
	
	USART_Cmd(USART1,ENABLE);
}

void USART1_IRQHandler(void)
{
	static uint8_t pRxPacket=0;		//指示接收到第几个变量了
	static uint8_t RxState=0;		//静态变量，只会初始化一次0，在函数结束后，数据仍然有效，只不过和全局变量不同的是只能在本函数内部使用。
	
	if(USART_GetITStatus (USART1,USART_IT_RXNE) == SET)
	{
		uint8_t RxData = USART_ReceiveData(USART1);
		if(RxState == 0)
		{
			if(RxData == '@')
			{
				RxState = 1;
				pRxPacket = 0;
			}
		}
		else if(RxState==1)
		{
			if(RxData=='\r')
			{
				RxState=2;
			}
			else
			{
				K210_RxPacket[pRxPacket] = RxData;
				pRxPacket ++;
			}
		}
				
		else if(RxState==2)
		{
			if(RxData=='\n')
			{
				RxState=0;
				K210_RxPacket[pRxPacket]='\0';
			}
		}
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}

	/*写队列*/
	xQueueSendToFrontFromISR((QueueHandle_t)K210Queue_Handler, 	// 队列句柄
							 (const void *)&K210_RxPacket,		// 指针，指向队尾发送的消息(前面定义)
							 (BaseType_t *)pdFALSE				// NULL
	);
}