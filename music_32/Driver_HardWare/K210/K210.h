#ifndef __USART1_H
#define __USART1_H

#include "stm32f10x.h"                  
#include "stdio.h" 

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

void K210_Init(uint32_t Baud);

extern char K210_RxPacket[100];

#endif
