#include "utils.h"

volatile uint32_t critical_cnt = 0;

void enter_critical(void) {
	__disable_irq();
	critical_cnt++;
}

void exit_critical(void) {
	critical_cnt--;
	if (critical_cnt == 0) {
		__enable_irq();
	}
}
