#ifndef __MP3_H
#define __MP3_H

#include "stm32f10x.h"                  // Device header
#include <stdbool.h>

#include "freertos.h"
#include "task.h"

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