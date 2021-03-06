#include "cyclic.h"
#include "../Utils/utils.h"
#include "../Led/led.h"

void cyclic_byte_clear(CyclicByteBuffer *cyclic) {
	cyclic->elements = 0;
	cyclic->max_elements = 0;
	cyclic->read_ptr = 0;
	cyclic->write_ptr = 0;
}

uint32_t cyclic_byte_get_elements(CyclicByteBuffer *cyclic) {
	return cyclic->elements;
}

uint32_t cyclic_byte_get_max_elements(CyclicByteBuffer *cyclic) {
	return cyclic->max_elements;
}

void cyclic_byte_add(CyclicByteBuffer *cyclic, uint8_t data) {
	enter_critical();

	cyclic->buffer[cyclic->write_ptr] = data;

	cyclic->write_ptr++;
	if (cyclic->write_ptr == cyclic->length) {
		cyclic->write_ptr = 0;
	}

	cyclic->elements++;
	if (cyclic->elements > cyclic->length) {
		if (cyclic->overflow_allowed) {
			cyclic->elements--;

			cyclic->read_ptr++;
			if (cyclic->read_ptr == cyclic->length) {
				cyclic->read_ptr = 0;
			}
		}
		else {
			LED_RED_ON;
			while (1);
		}
	}
	if (cyclic->elements > cyclic->max_elements) {
		cyclic->max_elements = cyclic->elements;
	}
	exit_critical();
}

bool cyclic_byte_get(CyclicByteBuffer *cyclic, uint8_t *data) {
	enter_critical();
	if (cyclic->elements == 0) {
		exit_critical();
		return false;
	}
	*data = cyclic->buffer[cyclic->read_ptr];
	cyclic->read_ptr++;

	if (cyclic->read_ptr == cyclic->length) {
		cyclic->read_ptr = 0;
	}

	cyclic->elements--;

	exit_critical();
	return true;
}
