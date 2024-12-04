#ifndef __YUYIN_H
#define __YUYIN_H

#include "system.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

extern char YuYin_RxPacket[100];


void YuYin_Init(u32 baud);

#endif
