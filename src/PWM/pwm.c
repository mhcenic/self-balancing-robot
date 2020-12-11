#include "pwm.h"

static bool init_status = false;

static void pwm_init_gpio(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

static void pwm_init_tim(void) {
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	TIM_TimeBaseInitTypeDef TIM_InitStructure;

	TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_InitStructure.TIM_Period = PWM_PERIOD;
	TIM_InitStructure.TIM_Prescaler = PWM_PRESCALER - 1;
	TIM_InitStructure.TIM_ClockDivision = 0;
	TIM_InitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM4, &TIM_InitStructure);

	TIM_OCInitTypeDef Channel_InitStructure;

	Channel_InitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	Channel_InitStructure.TIM_OutputState = TIM_OutputState_Enable;
	Channel_InitStructure.TIM_Pulse = 0;
	Channel_InitStructure.TIM_OCIdleState = 0;
	Channel_InitStructure.TIM_OCNIdleState = 0;
	Channel_InitStructure.TIM_OCNPolarity = 0;
	Channel_InitStructure.TIM_OutputNState = 0;
	Channel_InitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;

	TIM_OC4Init(TIM4, &Channel_InitStructure);

	TIM_Cmd(TIM4, ENABLE);
}

void pwm_test(void) {
	if (!DEBUG_TEST_ENABLE) {
		return;
	}
	//TODO
}

void pwm_init(void) {
	pwm_init_gpio();
	pwm_init_tim();

	pwm_test();
}

bool pwm_get_init_status(void) {
	return init_status;
}

void pwm_set_duty(uint32_t duty) {
	if (duty > PWM_MAX_PERIOD) {
		duty = PWM_MAX_PERIOD;
	}
	TIM_SetCompare4(TIM4, duty);
}
