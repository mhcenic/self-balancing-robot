#include "led.h"

static bool init_status = false;

static void led_init_gpio(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	//LED green
	GPIO_InitTypeDef GPIOA_InitStructure;
	GPIOA_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIOA_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIOA_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

	GPIO_Init(GPIOA, &GPIOA_InitStructure);

	//LED red & blue
	GPIO_InitTypeDef GPIOC_InitStructure;
	GPIOC_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIOC_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_3;
	GPIOC_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

	GPIO_Init(GPIOC, &GPIOC_InitStructure);
}

void led_init(void) {
	led_init_gpio();

	led_test();

	init_status = true;
}

void led_test(void) {
	if (!DEBUG_TEST_ENABLE) {
		return;
	}

	volatile bool led_green_status = false;
	LED_GREEN_ON;
	LED_GREEN_OFF;
	LED_GREEN_TOGGLE;
	led_green_status = LED_GREEN_CHECK;

	volatile bool led_red_status = false;
	LED_RED_ON;
	LED_RED_OFF;
	LED_RED_TOGGLE;
	led_red_status = LED_RED_CHECK;

	volatile bool led_blue_status = false;
	LED_BLUE_ON;
	LED_BLUE_OFF;
	LED_BLUE_TOGGLE;
	led_blue_status = LED_BLUE_CHECK;
}

bool led_get_init_status(void) {
	return init_status;
}
