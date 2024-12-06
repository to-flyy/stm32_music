#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f10x.h"    
#include <stdbool.h>

#include "freertos.h"
#include "task.h"


void Motor_Init(void);
void Motor_Stop(void);
void Motor_Two_Forward(uint16_t speed);
void Motor_Two_ForwardLoop(uint16_t Speed, uint8_t Period);
void Motor_Two_Reverse(uint16_t speed);
void Motor_Two_ReverseLoop(uint16_t Speed, uint8_t Period);

extern volatile bool motorRunning;

#endif
