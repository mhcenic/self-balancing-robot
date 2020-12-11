#ifndef UART_UART_H_
#define UART_UART_H_

#include <stdbool.h>

#include "stm32f10x.h"
#include "../src/Cyclic/cyclic.h"

#define UART_RX_BUFF_SIZE		32
#define UART_TX_BUFF_SIZE		1024

#define UART_RX_CIRC_BUFF_OVERFLOW_ALLOWED		false
#define UART_TX_CIRC_BUFF_OVERFLOW_ALLOWED		false

bool uart_get_byte(uint8_t *data);
void uart_send_byte(uint8_t data);
void uart_send_string(uint8_t *str);
void uart_init(void);
void uart_test(void);
bool uart_get_init_status(void);

#endif
