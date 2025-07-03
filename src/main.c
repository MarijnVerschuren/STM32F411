#include "usb/usb.h"
#include "usb/hid.h"
#include "EXTI.h"


/*!<
 * definitions
 * */
#define CUSTOM

#ifndef CUSTOM
#define LED_GPIO_PORT GPIOC
#define LED_PIN 13
#define BTN_GPIO_PORT GPIOA
#define BTN_PIN 0
#else
#define LED_GPIO_PORT GPIOB
#define LED_PIN 0
#define BTN_GPIO_PORT GPIOA
#define BTN_PIN 0
#endif



/*!<
 * variables
 * */
volatile uint8_t GO = 0;
const uint8_t strings[3][3] = {
	{'H'-'=', 'I'-'=', 'I'-'='},
	{'Y'-'=', 'O'-'=', 'O'-'='},
	{'W'-'=', 'T'-'=', 'F'-'='},
};


/*!<
 * types
 * */
typedef enum {
	NONE =		0b0,
	ENTER_ATE =	0b1
} key_opt_t;


/*!<
 * functions
 * */
void send_HID_keys(USB_handle_t* handle, const uint8_t* keys, uint32_t len, key_opt_t options);


/*!<
 * interrupts
 * */
void EXTI0_handler(void) {
	EXTI->PR = 1;
	GO = 1;
}


/*!<
 * main
 * */
int main(void) {
	// sys_clock: 25Mhz / 25 * 192 / 2 = 96Mhz
	// usb_clock: 25Mhz / 25 * 192 / 4 = 48Mhz
	set_SYS_PLL_config(25, 192, PLL_P_DIV2, 4, PLL_SRC_HSE);
	set_SYS_CLOCK_config(SYS_CLK_SRC_PLL, AHB_CLK_NO_DIV, APBx_CLK_DIV2, APBx_CLK_NO_DIV, 0);
	set_SYS_FLASH_config(FLASH_LATENCY4, 1, 1, 1);  // latency is set automatically (when need be)
	set_SYS_tick_config(1, 1);
	sys_clock_init();

	// GPIO
	config_GPIO(LED_GPIO_PORT, LED_PIN, GPIO_output | GPIO_no_pull | GPIO_push_pull);
	config_GPIO(BTN_GPIO_PORT, BTN_PIN, GPIO_input);
	config_GPIO(GPIOB, 6, GPIO_output | GPIO_push_pull);
	config_GPIO(GPIOB, 7, GPIO_output | GPIO_push_pull);
	GPIO_write(LED_GPIO_PORT, LED_PIN, 1);  // led is active low
	GPIO_write(GPIOB, 6, 0);  // led is active low
	GPIO_write(GPIOB, 7, 1);  // led is active low

	// EXTI
	config_EXTI_GPIO(BTN_GPIO_PORT, BTN_PIN, 1, 0);
	start_EXTI(BTN_PIN);

	// USB
	config_USB(USB, &HID_class, &FS_Desc, 0, 1);
	start_USB(USB);


	while (USB_handle.dev_state != DEV_STATE_CONFIGURED);
	GPIO_write(LED_GPIO_PORT, LED_PIN, 0);
	_NOP();

	uint8_t i = 0;
	// main loop
	for(;;) {
		if (!GO) { _WFI(); continue; }

		send_HID_keys(&USB_handle, &strings[i], 3, ENTER_ATE);
		i = (i + 1) % 3;

		GO = 0;
	}
}



/*!<
 * function definitions
 * */
void send_HID_keys(USB_handle_t* handle, const uint8_t* keys, uint32_t len, key_opt_t options) {
	static uint8_t HID_buffer[8] = {0, 0, 0x4, 0, 0, 0, 0, 0};

	for (uint32_t i = 0; i < len; i++) {
		HID_buffer[2] = keys[i];
		send_HID_report(handle, HID_buffer, 8);	delay_ms(HID_DELAY);
		HID_buffer[2] = 0;
		send_HID_report(handle, HID_buffer, 8);	delay_ms(HID_DELAY);
	}

	if (options & ENTER_ATE) {
		HID_buffer[2] = 0x28;
		send_HID_report(handle, HID_buffer, 8); delay_ms(HID_DELAY);
		HID_buffer[2] = 0;
		send_HID_report(handle, HID_buffer, 8);	delay_ms(HID_DELAY);
	}
}