#ifndef __LED_H
#define __LED_H

#include "system.h"

#define LED1 PBout(12)
#define LED2 PBout(13)
#define LED3 PBout(14)
#define Flag_LED PCout(13)

void LED_Init(void);

#endif
