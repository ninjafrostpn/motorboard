#include "led.h"

void led_init(void) {
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,
	              GPIO_CNF_OUTPUT_PUSHPULL, LED_M0_R);
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,
	              GPIO_CNF_OUTPUT_PUSHPULL, LED_M0_B);
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,
	              GPIO_CNF_OUTPUT_PUSHPULL, LED_M1_R);
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,
	              GPIO_CNF_OUTPUT_PUSHPULL, LED_M1_B);
}
