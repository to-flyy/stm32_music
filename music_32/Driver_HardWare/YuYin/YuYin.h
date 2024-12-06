#ifndef __YUYIN_H
#define __YUYIN_H

#include "system.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define YuYin_MUSIC_1 			"ZJL"
#define YuYin_MUSIC_2 			"JW"
#define YuYin_MUSIC_3 			"FZU"
#define YuYin_MUSIC_PAUSE 		"PAUSE"
#define YuYin_MUSIC_CONTINUE 	"CONTINUE"

void YuYin_Init(u32 baud);

extern char YuYin_RxPacket[100];

#endif
