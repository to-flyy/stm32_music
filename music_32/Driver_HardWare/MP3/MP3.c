#include "MP3.h"

volatile bool isMP3Playing = false;

/*USART2*/
void MP3_USART_Init(uint32_t bound)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(GPIO_TX_RX_RCC, ENABLE);
	RCC_APB2PeriphClockCmd(GPIO_Busy_RCC, ENABLE);

	// USART2_TX
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_TX_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // �����������
	GPIO_Init(GPIO_TX_Port, &GPIO_InitStructure);

	// USART2_RX
	GPIO_InitStructure.GPIO_Pin = GPIO_RX_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // ��������
	GPIO_Init(GPIO_RX_Port, &GPIO_InitStructure);

	// Busy
	GPIO_InitStructure.GPIO_Pin = GPIO_Busy_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIO_Busy_Port, &GPIO_InitStructure);

	// USART ��ʼ������
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	USART_Cmd(USART1, ENABLE);
}

/**********************************
���ڷ�������
***********************************/

void MP3_Com(u8 com)
{
	USART_ClearFlag(USART1, USART_FLAG_TC);
	USART_SendData(USART1, com);
	while (!USART_GetFlagStatus(USART1, USART_FLAG_TC));	
}

/*ͨ��JQ8400��������ģ��ٷ�������鿴���͵�ָ��*/
void MP3_Init(uint32_t bound)
{
	MP3_USART_Init(bound);

	vTaskDelay(2);
	
	MP3_Com(0xAA); // ѡ��FLASH����
	MP3_Com(0x08);
	MP3_Com(0x03);
	MP3_Com(0x02);
	MP3_Com(0x5A);
	MP3_Com(0x48);
	MP3_Com(0x59);
	vTaskDelay(2);
	
//	MP3_Com(0xAA); // ����15		0xAA, 0x13, 0x01, 0x05, 0xC3
//	MP3_Com(0x13);
//	MP3_Com(0x01);
//	MP3_Com(0x0F);
//	MP3_Com(0xCD);
	
	MP3_Com(0xAA);
	MP3_Com(0x13);
	MP3_Com(0x01);
	MP3_Com(0x05);
	MP3_Com(0xC3);
	vTaskDelay(2);
}

/*ѡ�񲥷ŵ�����*/
void MP3_Star(uint8_t first, uint8_t second)
{
	u8 first_mu = 0, second_mu = 0;
	first_mu = 0x31 + first;
	second_mu = 0x28 + second;
	
// 01 		0xAA, 0x1B, 0x02, 0x30, 0x31, 0x28, 
// 02		0xAA, 0x1B, 0x02, 0x30, 0x32, 0x29,  
// 03 		0xAA, 0x1B, 0x02, 0x30, 0x33, 0x2A
	MP3_Com(0xAA);
	MP3_Com(0x1B);
	MP3_Com(0x02);
	MP3_Com(0x30);
	MP3_Com(first_mu);
	MP3_Com(second_mu);
}

void MP3_Stop(void)
{
	if(isMP3Playing)
	{
		MP3_Com(0xAA);
		MP3_Com(0x03);
		MP3_Com(0x00);
		MP3_Com(0xAD);
		isMP3Playing = false;
	}
}

void MP3_Continue(void)
{
	if(!isMP3Playing)
	{
		MP3_Com(0xAA);
		MP3_Com(0x02);
		MP3_Com(0x00);
		MP3_Com(0xAC);
		isMP3Playing = true;
	}	
}

void MP3_Next(void)
{
	MP3_Com(0xAA);
	MP3_Com(0x06);
	MP3_Com(0x00);
	MP3_Com(0xB0);
}

void MP3_Last(void)
{
	MP3_Com(0xAA);
	MP3_Com(0x05);
	MP3_Com(0x00);
	MP3_Com(0xAF);
}

u8 MP3_State(void)
{
    return GPIO_ReadInputDataBit(GPIO_Busy_Port, GPIO_Busy_Pin);
}

/*  ��ͣ 0xAA, 0x03, 0x00, 0xAD
	���� 0xAA, 0x02, 0x00, 0xAC*/