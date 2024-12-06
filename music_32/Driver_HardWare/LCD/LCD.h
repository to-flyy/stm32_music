#ifndef __LCD_H
#define __LCD_H

#include "stm32f10x.h"                  // Device header
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define LCD_MUSIC_1		0x00
#define LCD_MUSIC_2		0x11
#define LCD_MUSIC_STOP	0xAD
#define LCD_MUSIC_LAST	0xAF
#define LCD_MUSIC_NEXT	0xB0

extern uint8_t LCD_RxData;

void LCD_Init(u32 baud);


#endif
