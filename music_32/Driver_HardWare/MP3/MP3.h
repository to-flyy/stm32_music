#ifndef __MP3_H
#define __MP3_H

#include "stm32f10x.h"                  // Device header
#include <stdbool.h>

#include "freertos.h"
#include "task.h"

#define GPIO_TX_Port GPIOA
#define GPIO_TX_Pin GPIO_Pin_9

#define GPIO_TX_RX_RCC RCC_APB2Periph_GPIOA

#define GPIO_RX_Port GPIOA
#define GPIO_RX_Pin GPIO_Pin_10

#define GPIO_Busy_Port GPIOC
#define GPIO_Busy_Pin GPIO_Pin_14
#define GPIO_Busy_RCC RCC_APB2Periph_GPIOC

void MP3_USART_Init(uint32_t bound);
void MP3_Init(uint32_t bound);
void MP3_Star(uint8_t first, uint8_t second);
void MP3_Com(u8 com);
u8 MP3_State(void);
void MP3_Stop(void);
void MP3_Continue(void);
void MP3_Next(void);
void MP3_Last(void);

extern volatile bool isMP3Playing;

#endif