#include "tick.h"
#include "../Led/led.h"

static bool init_status = false;
static volatile uint32_t tick_counter = 0;

uint32_t tick_get_time_ms(void) {
	return tick_counter;
}

void tick_delay_ms(uint32_t time) {
	time = tick_counter + time;
	while (time - tick_counter != 0);
}

void tick_init(void) {
	SystemCoreClockUpdate();

	SysTick_Config(SystemCoreClock / TICK_FREQUENCY_HZ);
	NVIC_SetPriority(SysTick_IRQn, 1);

	tick_test();

	init_status = true;
}

void tick_test(void) {
	if ((RCC->CFGR) & (RCC_CFGR_SWS != 0x08)) {
		LED_RED_ON;
		while (1); //TODO replace to debug
	}

	if (SystemCoreClock != TICK_CPU_FREQUENCY) {
		LED_RED_ON;
		while (1); //TODO replace to debug
	}
	if (!DEBUG_TEST_ENABLE) { //TODO
		return;
	}
}

bool tick_get_init_status(void) {
	return init_status;
}

void SysTick_Handler(void) {
	tick_counter++;
}
