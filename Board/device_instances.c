/*
 * Physical hardware descriptions.
 *
 * This file maps peripherals, GPIOs, timers, and devices
 * to the current board.
 */

#include "device_instances.h"

static LED_t status_led = {
	.port = GPIOA,
	.pin = 5,
	.mode = LED_MODE_GPIO,
};

static TB6612FNG_t tb6612fng = {

};

static DC_MOTOR_t motor = {

};

LED_t* board_get_status_led(void)
{
	return &status_led;
}

TB6612FNG_t* board_get_tb6612fng(void)
{
	return &tb6612fng;
}

DC_MOTOR_t* board_get_motor(void)
{
	return &motor;
}
