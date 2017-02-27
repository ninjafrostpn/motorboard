#include "output.h"
#include "error.h"

#include <libopencm3/stm32/f1/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/f1/gpio.h>

#define PWM_PORT GPIOA
#define CTRL_PORT GPIOB

#define M0INa GPIO12
#define M0INb GPIO13
#define M0ENa GPIO14
#define M0ENb GPIO15
#define M1INa GPIO8
#define M1INb GPIO9
#define M1ENa GPIO10
#define M1ENb GPIO11
#define M0PWM GPIO_TIM2_CH2
#define M1PWM GPIO_TIM2_CH1_ETR

#define setup_ctrl_gpio(pin) \
	gpio_set_mode(CTRL_PORT, GPIO_MODE_OUTPUT_2_MHZ, \
	              GPIO_CNF_OUTPUT_PUSHPULL, pin)

#define setup_pwm_pin(pin) \
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, \
	              GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, pin);

//throws an error if the channel is not one of 0 or 1
#define check_channel(ch) do { \
		if (!(ch == 0 || ch == 1)) return ENOCH; \
	} while(0)

//throws and error if the direction given is not one of the predefined direcion constants
#define check_direction(dir) do { \
		if (!(dir < DIR_COUNT)) return ENODIR; \
	} while(0)


#define check_speed(speed) do { \
		if (!(speed >= 0 && speed <= 4096)) return ENOSPEED; \
	} while(0)

int output_init(void) {
	setup_ctrl_gpio(M0INa);
	setup_ctrl_gpio(M0INb);
	setup_ctrl_gpio(M0ENa);
	setup_ctrl_gpio(M0ENb);
	setup_ctrl_gpio(M1INa);
	setup_ctrl_gpio(M1INb);
	setup_ctrl_gpio(M1ENa);
	setup_ctrl_gpio(M1ENb);

	rcc_peripheral_enable_clock(&RCC_APB1ENR, RCC_APB1ENR_TIM2EN);
	setup_pwm_pin(M0PWM);
	setup_pwm_pin(M1PWM);

	timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
	timer_set_period(TIM2, 2000);
	timer_set_prescaler(TIM2, 1);

	timer_set_oc_mode(TIM2, TIM_OC1, TIM_OCM_PWM1);
	timer_set_oc_mode(TIM2, TIM_OC2, TIM_OCM_PWM1);
	timer_enable_oc_preload(TIM2, TIM_OC1);
	timer_enable_oc_preload(TIM2, TIM_OC2);

	timer_set_oc_polarity_high(TIM2, TIM_OC1);
	timer_set_oc_polarity_high(TIM2, TIM_OC2);
	timer_enable_oc_output(TIM2, TIM_OC1);
	timer_enable_oc_output(TIM2, TIM_OC2);

	output_speed(0, 0);
	output_speed(1, 0);

	timer_enable_preload(TIM2);
	timer_enable_counter(TIM2);
	return 0;
}

//sets the relevant pins to enable the given motor
int output_enable(int channel) {
	check_channel(channel);
	if (channel == 0) {
		gpio_set(CTRL_PORT, M0ENa);
		gpio_set(CTRL_PORT, M0ENb);
	} else {
		gpio_set(CTRL_PORT, M1ENa);
		gpio_set(CTRL_PORT, M1ENb);
	}
	return 0;
}

//sets the relevant pins to disable the given motor
int output_disable(int channel) {
	check_channel(channel);
	if (channel == 0) {
		gpio_clear(CTRL_PORT, M0ENa);
		gpio_clear(CTRL_PORT, M0ENb);
	} else {
		gpio_clear(CTRL_PORT, M1ENa);
		gpio_clear(CTRL_PORT, M1ENb);
	}
	return 0;
}

//sets the relevant pins to control the direction in which the motor given by channel turns (or doesn't)
int output_direction(int channel, direction_t direction) {
	check_channel(channel);
	check_direction(direction);
	if (direction == DIR_HALT) {        //sets pins for stopping the relevant motor
		if (channel == 0) {
			gpio_clear(CTRL_PORT, M0INa);
			gpio_clear(CTRL_PORT, M0INb);
		} else {
			gpio_clear(CTRL_PORT, M1INa);
			gpio_clear(CTRL_PORT, M1INb);
		}
	} else if (direction == DIR_FWD) {  //sets pins for turning the relevant motor forward
		if (channel == 0) {
			gpio_set(CTRL_PORT, M0INa);
			gpio_clear(CTRL_PORT, M0INb);
		} else {
			gpio_set(CTRL_PORT, M1INa);
			gpio_clear(CTRL_PORT, M1INb);
		}
	} else if (direction == DIR_REV) {
		if (channel == 0) {
			gpio_clear(CTRL_PORT, M0INa);
			gpio_set(CTRL_PORT, M0INb);
		} else {
			gpio_clear(CTRL_PORT, M1INa);
			gpio_set(CTRL_PORT, M1INb);
		}
	}
	return 0;
}

//sets the pwm pins to the corresponding values for the speed given for the motor given
int output_speed(int channel, int speed) {
	check_channel(channel);
	check_speed(speed);
	if (channel == 0) {
		timer_set_oc_value(TIM2, TIM_OC2, speed*20);
	} else {
		timer_set_oc_value(TIM2, TIM_OC1, speed*20);
	}
	return 0;
}
