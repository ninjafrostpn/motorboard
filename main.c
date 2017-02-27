#include <stdio.h>
#include <stdlib.h>

#include <libopencm3/stm32/f1/rcc.h>
#include <libopencm3/stm32/f1/gpio.h>
#include <libopencm3/cm3/scb.h>

#include "led.h"
#include "output.h"
#include "usart.h"
#include "analogue.h"
#include "fw_ver.h"
#include "commands.h"

uint32_t *top_of_ram = ((uint32_t *)0x20001FF0);  //!< pointer to the top of the ram
#define BOOTLOADER_MAGIC 0xFACEBEE5               //!< command that should be seen at the top of the ram if the bootloader should be run

void init(void) {
	rcc_clock_setup_in_hse_8mhz_out_24mhz();

	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPAEN);
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPBEN);
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPCEN);
	//initialises subsystems
	led_init();
	output_init();
	usart_init();
	//analogue_init();

	led_set(LED_M0_R);
}

//prints firmware version message to serial
void print_version(void) {
	printf("MCV4B:%i\n", firmware_version);
}

//causes the system to reboot
void enter_bootloader(void) {
	printf("Entering bootloader\n"); //prints message to serial
	*top_of_ram = BOOTLOADER_MAGIC;  //writes the magic string to the top of the ram
	scb_reset_system();							 //no idea yet
}

//alters the behaviour of the motors
void set_output(int channel, int8_t c) {
	int8_t i = c - 128;						           //convert unsigned input into signed value for use with motor power
	if (c == COMMAND_DISABLE) {							 //presumably for stopping, without braking
		output_disable(channel);
	} else if (c == COMMAND_ENABLE) {				 //presumably for stopping, using the brakes
		output_enable(channel);
		output_direction(channel, DIR_HALT);
	} else {															   //for turning the motors in either direction
		output_enable(channel);
		if (i < 0) {													 //sets the correct direction of motor motion according to the sign of i
			output_direction(channel, DIR_REV);  //i > 0 (128 < c < 256) means forward
		} else {
			output_direction(channel, DIR_FWD);  //i <= 0 (1 < c < 128) means backward
		}
		output_speed(channel, abs(i));				 //sets the motor to move in the given direction with the speed given by i
	}
}

//sets the values that the stat_t type should be able to take
typedef enum {
	STATE_INIT,
	STATE_SPEED0,
	STATE_SPEED1
} state_t;

static state_t state = STATE_INIT;                  //!< current input state

/**************************************************************************/
/*! Finite state machine function
 *  note: you can spam the input with 0s to get back to state = #STATE_INIT without altering the motor speeds
 *
 * \param c the character read from the serial port, the use of which upon the value of #state:
								if state is #STATE_INIT, c signifies a command
								if state is #STATE_SPEED0 or #STATE_SPEED1, c sets the speed motor 0 and motor 1, respectively
 **************************************************************************/
void fsm(int c) {
	switch (state) {              //varies in behaviour depending on state
		case STATE_INIT:						//if state is #STATE_INIT, interprets #c as a command
			switch (c) {
				case COMMAND_NONE:      //sets state to #STATE_INIT
					state = STATE_INIT;   //in other words, doesn't change state
					break;
				case COMMAND_VERSION:   //prints the version string to serial
					state = STATE_INIT;
					print_version();
					break;
				case COMMAND_SPEED0:    //sets the speed of motor 0
					state = STATE_SPEED0;
					break;
				case COMMAND_SPEED1:    //sets the speed of motor 1
					state = STATE_SPEED1;
					break;
				case COMMAND_BOOTLOADER://enters the bootloader
					enter_bootloader();
					break;
				default:                //same as for #COMMAND_NONE, does nothing
					state = STATE_INIT;
					break;
			}
			break;
		case STATE_SPEED0:          //if state is #STATE_SPEED0, interprets c as speed for motor 0
			state = STATE_INIT;
			if (c != COMMAND_NONE) {
				set_output(0, c);
			}
			break;
		case STATE_SPEED1:          //if state is #STATE_SPEED1, interprets c as speed for motor 1
			state = STATE_INIT;
			if (c != COMMAND_NONE) {
				set_output(1, c);
			}
			break;
		default:                    //if state has somehow ended up as something else, set state back to #STATE_INIT
			state = STATE_INIT;
			break;
	}
}


int main(void) {
	//Check to see if we should jump into the bootloader
	if (*top_of_ram == BOOTLOADER_MAGIC) {
		*top_of_ram = 0;                      //prevents the system from rebooting on boot
		//runs some assembly code
		asm("ldr r0, =0x1FFFF000\n\t" \
		    "ldr sp,[r0, #0]\n\t" \
		    "ldr r0,[r0, #4]\n\t" \
		    "bx r0");
	}

	init();

	while (1) {
		int c = usart_get_char(); //read off the serial
		fsm(c);										//submit the bytes to the state machine
	}

	return 0;
}
