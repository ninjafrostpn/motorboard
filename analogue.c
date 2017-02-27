#include "analogue.h"
#include "led.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include <libopencm3/stm32/f1/adc.h>
#include <libopencm3/stm32/f1/rcc.h>
#include <libopencm3/stm32/f1/gpio.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/dbgmcu.h>

void analogue_init(void) {
	gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG, GPIO1); /* 12V */
	gpio_set_mode(GPIOC, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG, GPIO0); /* M0 CS */
	gpio_set_mode(GPIOC, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG, GPIO1); /* M0a */
	gpio_set_mode(GPIOC, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG, GPIO2); /* M0b */

	/* NVIC set up */
	nvic_enable_irq(NVIC_ADC1_2_IRQ);
	nvic_set_priority(NVIC_ADC1_2_IRQ, 1);

	/* Timer set up */
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_TIM1EN);

	timer_set_mode(TIM1, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
	/* 24MHz/12kHz = 2kHz timer clock. /2 gives 1000Hz sampling frequency */
	timer_set_period(TIM1, 2);
	timer_set_prescaler(TIM1, 12000);

	timer_set_oc_mode(TIM1, TIM_OC1, TIM_OCM_PWM1);
	timer_enable_oc_preload(TIM1, TIM_OC1);
	timer_set_oc_value(TIM1, TIM_OC1, 1);

	timer_set_master_mode(TIM1, TIM_CR2_MMS_COMPARE_OC1REF);

	timer_enable_preload(TIM1);
	timer_enable_counter(TIM1);

	/* Halt the ADC timer while debugging */
	DBGMCU_CR |= DBGMCU_CR_TIM1_STOP;

	/* ADC set up */
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_ADC1EN);
	rcc_set_adcpre(RCC_CFGR_ADCPRE_PCLK2_DIV2);

	adc_off(ADC1);

	adc_enable_external_trigger_injected(ADC1, ADC_CR2_JEXTSEL_TIM1_TRGO);
	adc_enable_scan_mode(ADC1);
	adc_enable_eoc_interrupt(ADC1);

	adc_power_on(ADC1);

	uint8_t channel[] = {9, 10, 11, 12};
	adc_set_injected_sequence(ADC1, 4, channel);

	/* Wait for things to warm up */
	for (int i=0; i<100000; i++)
		__asm__("nop");
	adc_reset_calibration(ADC1);
	adc_calibration(ADC1);
}

void adc1_2_isr(void) {
	led_toggle(LED_M0_R);
	ADC1_SR = 0;
}
