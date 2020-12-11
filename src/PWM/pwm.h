#ifndef PWM_PWM_H_
#define PWM_PWM_H_

#include <stdbool.h>
#include "stm32f10x.h"
#include "../Tick/tick.h"

#define PWM_FREQUENCY_HZ		10000
#define PWM_PRESCALER			1
#define PWM_PERIOD				(TICK_CPU_FREQUENCY/PWM_FREQUENCY_HZ)
#define PWM_MAX_PERIOD			PWM_PERIOD
#define PWM_MIN_PERIOD			0

void pwm_init(void);
void pwm_test(void);
void pwm_set_duty(uint32_t duty);
bool pwm_get_init_status(void);

#endif
