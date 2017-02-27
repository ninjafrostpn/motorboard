#ifndef __LED_H
#define __LED_H

#include <libopencm3/stm32/f1/gpio.h>

#define LED_M0_R GPIO6
#define LED_M0_B GPIO7
#define LED_M1_R GPIO8
#define LED_M1_B GPIO9

void led_init(void);

#define led_set(led) gpio_set(GPIOC, led)
#define led_clear(led) gpio_clear(GPIOC, led)
#define led_toggle(led) gpio_toggle(GPIOC, led)

#endif /* __LED_H */
