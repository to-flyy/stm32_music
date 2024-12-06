#include "YuYin.h"

char YuYin_RxPacket[100];

extern QueueHandle_t YuYinQueue_Handler;

void YuYin_Init(u32 baud)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	
	
	USART_DeInit(USART3);

	//TX  
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	
	USART_Init(USART3, &USART_InitStructure); 
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&NVIC_InitStructure);	

	USART_Cmd(USART3, ENABLE);                    
}

void USART3_IRQHandler(void)                	
{
	static uint8_t pRxPacket=0;
	static uint8_t RxState=0;
	
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  
	{
		uint8_t RxData=USART_ReceiveData(USART3);
		if(RxState==0)
		{
			if(RxData=='@')
			{
				RxState =1;
				pRxPacket=0;
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
				YuYin_RxPacket[pRxPacket]=RxData;
				pRxPacket ++;
			}
		}
				
		else if(RxState==2)
		{
			if(RxData=='\n')
			{
				RxState=0;
				YuYin_RxPacket[pRxPacket]='\0';
			}
		}
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);	
	}	
	/*–¥∂”¡–*/
	xQueueSendToFrontFromISR((QueueHandle_t)YuYinQueue_Handler, 	
							 (const void *)&YuYin_RxPacket,		
							 (BaseType_t *)pdFALSE);
} 