#include "stm32f10x.h"
#include <stdbool.h>
#include <stdio.h>

#include "Led/led.h"
#include "Tick/tick.h"
#include "Uart/uart.h"
#include "PWM/pwm.h"

void action_help(void *arg);
void action_led_blue_on(void *arg);
void action_led_blue_off(void *arg);
void action_set_pwm(void *arg);
bool check_empty(void *data, void *ret_value);
bool check_uint(void *data, void *ret_value);
bool check_float(void *data, void *ret_value);

#define MENU_RX_BUFF_SIZE	16
#define LSM303D_ADDR 0x3a

typedef struct{
	uint8_t *text;
	void (*action)(void *);
	bool (*arg_check)(void *, void *);
	//...
}UserMenuRow;



UserMenuRow user_menu[]={
		{.text = "Help", 			.action =action_help, 			.arg_check = check_empty},
		{.text = "Led Blue On", 	.action =action_led_blue_on, 	.arg_check = check_empty},
		{.text = "Led Blue Off", 	.action =action_led_blue_off, 	.arg_check = check_empty},
		{.text = "Set PWM Value", 	.action =action_set_pwm, 		.arg_check = check_uint},
		//Example: {.text = "Set Kp Value", 	.action =action_set_kp, 		.arg_check = check_foat}
};

void action_help(void *arg) {
	uint32_t i;
	for (i = 0; i < sizeof(user_menu) / sizeof(UserMenuRow); i++) {
		printf("%u. %s\r\n", i + 1, user_menu[i]);
	}
}

void action_led_blue_on(void *arg){
	printf("Led blue on\r\n");
	LED_BLUE_ON;
}

void action_led_blue_off(void *arg){
	printf("Led blue off\r\n");
	LED_BLUE_OFF;
}

void action_set_pwm(void *arg){
	uint32_t *value = (uint32_t *) arg;
	pwm_set_duty(*value);
	printf("Set PWM value: %d\r\n", *value);
}

bool check_empty(void *data, void *ret_value){
	uint8_t *rx_data = (uint8_t *)data;
	if(rx_data[0] == 0){
		return true;
	}else{
		return false;
	}
}

bool check_uint(void *data, void *ret_value) {
	uint8_t *rx_data_tmp = (uint8_t *) data;
	uint32_t *ret_data_tmp = (uint32_t *) ret_value;

	if(strlen(rx_data_tmp)==0){
		return false;
	}

	//Check int
	uint32_t i;
	for (i = 0; i < strlen(rx_data_tmp); i++) {
		if (rx_data_tmp[i] >= '0' && rx_data_tmp[i] <= '9') {
			//
		} else{
			return false;
		}
	}

	*ret_data_tmp=atoi(rx_data_tmp);
	return true;
}

//bool check_float(void *data, void *ret_value){
//	uint8_t *rx_data_tmp = (uint8_t *)data;
//	float *ret_data_tmp = (float *)ret_value;
//
//	uint8_t len = 0;
//	float dummy = 0.0;
//	if(sscanf(rx_data_tmp,"%f%n", &dummy, &len) == 1 && len == (uint8_t)strlen(rx_data_tmp)){
//		*ret_data_tmp = dummy;
//		return true;
//	}else{
//		return false;
//	}
//}



void menu_state_machine(uint8_t * rx_data){
	static uint8_t rx_buff_menu[MENU_RX_BUFF_SIZE];
	static uint32_t rx_buff_menu_ptr =0 ;
	bool rx_complete=false;

	if(rx_data == '\r'){
		return;
	}

	if(rx_data == ' '){
		rx_data = 0;
	}

	if(rx_data == '\n'){
		rx_data = 0;
		rx_complete = true;
	}

	rx_buff_menu[rx_buff_menu_ptr]=rx_data;
	rx_buff_menu_ptr++; // a co je¿eli zrobimy na full i tu zwiekszymy to zadzia³a nam if chyba trzeba dac +1
	if(rx_buff_menu_ptr==MENU_RX_BUFF_SIZE){
		printf("\r\nInvalid argument, buffer overflow\r\n");
		rx_buff_menu_ptr=0;
		memset(rx_buff_menu, 0 , MENU_RX_BUFF_SIZE);
	}

	if(rx_complete){

		uint32_t command_id = 0;
		if (check_uint(rx_buff_menu, &command_id)) {
			if (command_id > 0 && command_id <= sizeof(user_menu) / sizeof(UserMenuRow)) {
				if (rx_buff_menu[strlen(rx_buff_menu)] == 0) { ///a co jezeli id menu ma wiecej niz jedna cyfre
					uint32_t ret_value = 0;
					if (user_menu[command_id - 1].arg_check(rx_buff_menu + strlen(rx_buff_menu)+1, &ret_value)) {
						user_menu[command_id - 1].action(&ret_value);
					} else {
						printf("Invalid argument\r\n");
					}
				} else {
					printf("Invalid argument\r\n");
				}

			} else {
				printf("Command out of range\r\n");
			}
		} else {
			printf("Invalid command\r\n");
		}
		rx_buff_menu_ptr=0;
		memset(rx_buff_menu, 0, MENU_RX_BUFF_SIZE);
	}
}




void print_init(void){
	printf("Initialization...\r\n");
	printf("Compilation time: %s %s\r\n", __DATE__, __TIME__);
	action_help((void *)0);
}








#define PWM_RX_BUFF_SIZE		32

void pwm_add_byte(uint8_t rx_data);
bool pwm_hex_string_to_uint(uint8_t pc_str[], uint32_t *value);


static uint8_t pwm_buff_cnt = 0;
static volatile uint8_t pwm_buff[PWM_RX_BUFF_SIZE];


#define TASK_LED_PLINK_PERIOD_MS	250

void task_led_blink(void) {
	static uint32_t time = 0;

	if (tick_get_time_ms() > time) {
		time += TASK_LED_PLINK_PERIOD_MS;
		LED_GREEN_TOGGLE;
	}
}

void task_uart_recive(void) {
	uint8_t rx_data;
	if (uart_get_byte(&rx_data)) {
		//Echo
		uart_send_byte(rx_data);
		menu_state_machine(rx_data);
	}
}

void uart_buffer_test(void) {
	static uint8_t num = 0;
	static uint32_t time = 0;
	if (num == 255) {
		return;
	}
	if (tick_get_time_ms() > time) {
		time += TASK_LED_PLINK_PERIOD_MS;
		uart_send_byte(num++);
	}
}

void pwm_add_byte(uint8_t rx_data) {
	if (rx_data == 'z') {
		pwm_buff[pwm_buff_cnt] = rx_data;
		uint32_t puiValue = 0;
		bool result;
		result = pwm_hex_string_to_uint(pwm_buff, &puiValue);
		pwm_buff_cnt = 0;

		if (result) {
			pwm_set_duty(puiValue);
			uart_send_string("OK\r\n");
		} else {
			uart_send_string("Invalid argument!\r\n");
		}
	} else {
		pwm_buff[pwm_buff_cnt] = rx_data;
		pwm_buff_cnt++;
	}
}

bool pwm_hex_string_to_uint(uint8_t pc_str[], uint32_t *value) {

	unsigned char ucCounter;
	unsigned char ucCurrentCharacter;

	*value = 0;

	if ((pc_str[0] != '0') || (pc_str[1] != 'x') || (pc_str[2] == 'z')) {
		return false;
	}

	for (ucCounter = 2; ucCounter < 7; ucCounter++) {
		ucCurrentCharacter = pc_str[ucCounter];
		if (ucCurrentCharacter == 'z') {
			return true;

		} else if (ucCounter == 6) {
			return false;

		}

		*value = *value << 4;

		if ((ucCurrentCharacter <= '9') && (ucCurrentCharacter >= '0')) {
			ucCurrentCharacter = ucCurrentCharacter - '0';
		} else if ((ucCurrentCharacter <= 'F') && (ucCurrentCharacter >= 'A')) {
			ucCurrentCharacter = ucCurrentCharacter - 'A' + 10;
		} else {
			return false;
		}
		*value = (*value) | ucCurrentCharacter;
	}
}

//printf("Init");
//[00000001] Init\r\n
//printf("[%08u] Init\n\t", tick_get_time());
//#define PRINTF(str, args...)  printf("[%08u]"str"\r\n", tick_get_time_ms(), args)

//Systick P=1
//Uart P=8


#define ENCODER_A_CHECK			GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)
#define ENCODER_B_CHECK			GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1)

void encoder_gpio_init_test(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitTypeDef GPIOA_InitStructure;
	GPIOA_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIOA_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIOA_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOA, &GPIOA_InitStructure);

	/////////
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	TIM_TimeBaseInitTypeDef Timer_InitStruct;
	TIM_TimeBaseStructInit(&Timer_InitStruct);
	Timer_InitStruct.TIM_CounterMode = TIM_CounterMode_Up | TIM_CounterMode_Down;

	TIM_TimeBaseInit(TIM1, &Timer_InitStruct);
/*
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_Channel=TIM_Channel_1;

      TIM_ICInitStructure.TIM_ICFilter=0x04;

      TIM_ICInit(TIM1,&TIM_ICInitStructure);

      TIM_ICInitStructure.TIM_Channel=TIM_Channel_2;

      TIM_ICInitStructure.TIM_ICFilter=0x04;

      TIM_ICInit(TIM1,&TIM_ICInitStructure);
*/
	TIM_EncoderInterfaceConfig(TIM1, TIM_EncoderMode_TI2, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_Cmd(TIM1, ENABLE);


}
int main(void) {

	//


	//


	//Set priority mode
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	//Led initialization
	led_init();

	//SysTick initialization
	tick_init();

	//PWM initialize
	pwm_init();

	//UART initialize
	uart_init();

	print_init();
//	tick_delay_ms(5000);
//	pwm_set_duty(0);

	encoder_gpio_init_test();
	while (1) {
		tick_delay_ms(100);

		printf("%u\n\r", TIM_GetCounter(TIM1));
		/*
		if(ENCODER_A_CHECK){
			LED_RED_ON;
		}else{
			LED_RED_OFF;
		}

		if(ENCODER_B_CHECK){
			LED_BLUE_ON;
		}else{
			LED_BLUE_OFF;
		}
		*/

		//task_led_blink();
		//task_uart_recive();


//		uart_buffer_test();
	}
}




//#include "stm32f10x.h"
//
//#define SPIx_RCC      	RCC_APB2Periph_SPI1
//#define SPIx          	SPI1
//#define SPI_GPIO_RCC  	RCC_APB2Periph_GPIOA
//#define SPI_GPIO      	GPIOA
//#define SPI_PIN_MOSI  	GPIO_Pin_7
//#define SPI_PIN_MISO  	GPIO_Pin_6
//#define SPI_PIN_SCK   	GPIO_Pin_5
//#define SPI_PIN_SS    	GPIO_Pin_4
//
//
//void SPIx_Init(void);
//uint8_t SPIx_Transfer(uint8_t data);
//void SPIx_EnableSlave(void);
//void SPIx_DisableSlave(void);
//
//uint16_t receivedByte1;
//uint16_t receivedByte2;
//int main(void)
//{
//    SPIx_Init();
//
//	while (1){
//		SPIx_EnableSlave();
//		receivedByte1= SPIx_Transfer(0x80 | 0x0f);
//		receivedByte2 = SPIx_Transfer(0x00);
//		SPIx_DisableSlave();
//    }
//}
//
//void SPIx_Init()
//{
//    SPI_InitTypeDef SPI_InitStruct;
//    GPIO_InitTypeDef GPIO_InitStruct;
//
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
//    RCC_APB2PeriphClockCmd(SPIx_RCC, ENABLE);
//    RCC_APB2PeriphClockCmd(SPI_GPIO_RCC, ENABLE);
//
//
//    GPIO_InitStruct.GPIO_Pin = SPI_PIN_MOSI | SPI_PIN_SCK;
//    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
//    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init(SPI_GPIO, &GPIO_InitStruct);
//
//    GPIO_InitStruct.GPIO_Pin = SPI_PIN_MISO;
//    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
//    GPIO_Init(SPI_GPIO, &GPIO_InitStruct);
//
//    // GPIO pin for SS
//    GPIO_InitStruct.GPIO_Pin = SPI_PIN_SS;
//    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
//    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init(SPI_GPIO, &GPIO_InitStruct);
//
//    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
//    SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;
//    SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;
//    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
//    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
//    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
//    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
//    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
//    SPI_InitStruct.SPI_CRCPolynomial = 7;
//
//    SPI_Init(SPIx, &SPI_InitStruct);
//    SPI_Cmd(SPIx, ENABLE);
//
//    SPIx_DisableSlave();
//}
//
//uint8_t SPIx_Transfer(uint8_t data)
//{
//    // Write data to be transmitted to the SPI data register
//    SPIx->DR = (data);
//    // Wait until transmit complete
//    while (!(SPIx->SR & (SPI_I2S_FLAG_TXE)));
//    // Wait until receive complete
//    while (!(SPIx->SR & (SPI_I2S_FLAG_RXNE)));
//    // Wait until SPI is not busy anymore
//    while (SPIx->SR & (SPI_I2S_FLAG_BSY));
//    // Return received data from SPI data register
//    return SPIx->DR;
//}
//
//void SPIx_EnableSlave()
//{
//    // Set slave SS pin low
//    SPI_GPIO->BRR = SPI_PIN_SS;
//}
//
//void SPIx_DisableSlave()
//{
//    // Set slave SS pin high
//    SPI_GPIO->BSRR = SPI_PIN_SS;
//}

