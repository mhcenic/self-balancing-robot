#ifndef TICK_TICK_H_
#define TICK_TICK_H_

#include "stm32f10x.h"
#include <stdbool.h>

#define TICK_FREQUENCY_HZ			1000
#define TICK_CPU_FREQUENCY			64000000

uint32_t tick_get_time_ms(void);

void tick_delay_ms(uint32_t time);

void tick_init(void);

void tick_test(void);

bool tick_get_init_status(void);

#endif
