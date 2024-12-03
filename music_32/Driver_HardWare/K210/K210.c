#include "K210.h"

extern QueueHandle_t K210Queue_Handler;

char K210_RxPacket[100];

int fputc(int ch, FILE *p) // ����Ĭ�ϵģ���ʹ��printf����ʱ�Զ�����
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
	static uint8_t pRxPacket=0;		//ָʾ���յ��ڼ���������
	static uint8_t RxState=0;		//��̬������ֻ���ʼ��һ��0���ں���������������Ȼ��Ч��ֻ������ȫ�ֱ�����ͬ����ֻ���ڱ������ڲ�ʹ�á�
	
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

	/*д����*/
	xQueueSendToFrontFromISR((QueueHandle_t)K210Queue_Handler, 	// ���о��
							 (const void *)&K210_RxPacket,		// ָ�룬ָ���β���͵���Ϣ(ǰ�涨��)
							 (BaseType_t *)pdFALSE				// NULL
	);
}