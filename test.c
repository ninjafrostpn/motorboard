#include <stdio.h>
#include <stdlib.h>

#include <libopencm3/stm32/f1/rcc.h>
#include <libopencm3/stm32/f1/gpio.h>

#include "led.h"
#include "output.h"

#define DELAY 3

#define delay(x) do { for (int i = 0; i < x * 100000; i++) \
                          __asm__("nop"); \
                    } while(0)

void init(void) {
	rcc_clock_setup_in_hse_8mhz_out_24mhz();

	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPAEN);
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPBEN);
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPCEN);
	led_init();
	output_init();
}

void flash(int led) {
	led_set(led);
	delay(DELAY);
	led_clear(led);
	delay(DELAY);
}

int main(void) {
	int i;

	init();
	output_speed(0, 50);
	output_speed(1, 50);

	while (1) {
		flash(LED_M0_R);
		flash(LED_M0_B);
		flash(LED_M1_R);
		flash(LED_M1_B);

		output_enable(0);
		output_direction(0, DIR_FWD);
		delay(DELAY*2);
		output_direction(0, DIR_REV);
		delay(DELAY*2);
		output_disable(0);
		delay(DELAY);

		output_enable(1);
		output_direction(1, DIR_FWD);
		delay(DELAY*2);
		output_direction(1, DIR_REV);
		delay(DELAY*2);
		output_disable(1);
		delay(DELAY);
	}

	return 0;
}
