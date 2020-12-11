#ifndef CYCLIC_CYCLIC_H_
#define CYCLIC_CYCLIC_H_

#include "stm32f10x.h"
#include <stdbool.h>

typedef struct {
	uint32_t write_ptr;
	uint32_t read_ptr;
	uint32_t elements;
	uint32_t length;
	uint32_t max_elements;
	volatile uint8_t *buffer;
	bool overflow_allowed;
} CyclicByteBuffer;

void cyclic_byte_clear(CyclicByteBuffer *cyclic);
void cyclic_byte_add(CyclicByteBuffer *cyclic, uint8_t data);
bool cyclic_byte_get(CyclicByteBuffer *cyclic, uint8_t *data);
uint32_t cyclic_byte_get_elements(CyclicByteBuffer *cyclic);
uint32_t cyclic_byte_get_max_elements(CyclicByteBuffer *cyclic);

#endif
