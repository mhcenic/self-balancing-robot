#ifndef LED_LED_H_
#define LED_LED_H_

#include "stm32f10x.h"
#include <stdbool.h>

#define LED_RED_ON				GPIO_SetBits(GPIOC, GPIO_Pin_0)
#define LED_RED_OFF				GPIO_ResetBits(GPIOC, GPIO_Pin_0)
#define LED_RED_TOGGLE			GPIOC->ODR ^= GPIO_Pin_0
#define LED_RED_CHECK			GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0)

#define LED_GREEN_ON			GPIO_SetBits(GPIOA, GPIO_Pin_5)
#define LED_GREEN_OFF			GPIO_ResetBits(GPIOA, GPIO_Pin_5)
#define LED_GREEN_TOGGLE		GPIOA->ODR ^= GPIO_Pin_5
#define LED_GREEN_CHECK			GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5)

#define LED_BLUE_ON				GPIO_SetBits(GPIOC, GPIO_Pin_3)
#define LED_BLUE_OFF			GPIO_ResetBits(GPIOC, GPIO_Pin_3)
#define LED_BLUE_TOGGLE			GPIOC->ODR ^= GPIO_Pin_3
#define LED_BLUE_CHECK			GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)

void led_init(void);

void led_test(void);

bool led_get_init_status(void);

#endif
