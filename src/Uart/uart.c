#include "uart.h"

static bool init_status = false;

static volatile uint8_t rx_buff[UART_RX_BUFF_SIZE];
static volatile uint8_t tx_buff[UART_TX_BUFF_SIZE];

static volatile CyclicByteBuffer rx_circ_buff = {
		.buffer = rx_buff,
		.length = UART_RX_BUFF_SIZE,
		.overflow_allowed = false
};

static volatile CyclicByteBuffer tx_circ_buff = {
		.buffer = tx_buff,
		.length = UART_TX_BUFF_SIZE,
		.overflow_allowed = false
};

static void uart_init_gpio(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;

	//UART - TX
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//UART - RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

static void uart_init_nvic(void) {
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_SetPriority(USART2_IRQn, 8);
}

static void uart_init_uart(void) {
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_HardwareFlowControl =
	USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2, &USART_InitStructure);

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART2, ENABLE);
}

void uart_test(void) {
	if (!DEBUG_TEST_ENABLE) {
		return;
	}
	//TODO
}

bool uart_get_init_status(void) {
	return init_status;
}

void uart_init(void) {
	cyclic_byte_clear((CyclicByteBuffer *) &rx_circ_buff);
	cyclic_byte_clear((CyclicByteBuffer *) &tx_circ_buff);

	uart_init_gpio();
	uart_init_nvic();
	uart_init_uart();

	uart_test();

	init_status = true;
}

bool uart_get_byte(uint8_t *data) {
	return cyclic_byte_get((CyclicByteBuffer *) &rx_circ_buff, data);
}

void uart_send_byte(uint8_t data) {
	cyclic_byte_add((CyclicByteBuffer *) &tx_circ_buff, data);
	USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
}

void uart_send_string(uint8_t *str) {
	while (*str) {
		cyclic_byte_add((CyclicByteBuffer *) &tx_circ_buff, *str++);
	}
	USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
}

void USART2_IRQHandler(void) {
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
		cyclic_byte_add((CyclicByteBuffer *) &rx_circ_buff,
				USART_ReceiveData(USART2));
	}

	if (USART_GetITStatus(USART2, USART_IT_TXE) != RESET) {
		uint8_t data;
		if (cyclic_byte_get((CyclicByteBuffer *) &tx_circ_buff, &data)) {
			USART_SendData(USART2, data);
		}

		if (cyclic_byte_get_elements((CyclicByteBuffer *) &tx_circ_buff) == 0) {
			USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
		}
	}
}

