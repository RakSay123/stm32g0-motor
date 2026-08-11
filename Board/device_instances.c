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

GPIO_Pin_t ain1_pin = {
	.port = GPIOA,
	.pin = 6
};

GPIO_Pin_t ain2_pin = {
	.port = GPIOA,
	.pin = 7
};

GPIO_Pin_t pwma_pin = {
	.port = GPIOD,
	.pin = 0
};

TIM_PWM_Config_t pwma_cfg = {
	.TIMx = TIM16,
	.channel = 1,
	.duty_cycle = 0,
	.pwm_mode = TIM_PWM1
};

GPIO_Pin_t bin1_pin = {
	.port = GPIOA,
	.pin = 10
};

GPIO_Pin_t bin2_pin = {
	.port = GPIOB,
	.pin = 3
};

GPIO_Pin_t pwmb_pin = {
	.port = GPIOD,
	.pin = 1
};

TIM_PWM_Config_t pwmb_cfg = {
	.TIMx = TIM17,
	.channel = 1,
	.duty_cycle = 0,
	.pwm_mode = TIM_PWM1
};

GPIO_Pin_t tb6612fng_stby_pin = {
	.port = GPIOA,
	.pin = 8
};

TB6612FNG_Channel_Config_t tb6612fng_cha = {
	.in1 = &ain1_pin,
	.in2 = &ain2_pin,

	.pwm_pin = &pwma_pin,
	.pwm_af = GPIO_AF2,
	.pwm = &pwma_cfg
};

TB6612FNG_Channel_Config_t tb6612fng_chb = {
	.in1 = &bin1_pin,
	.in2 = &bin2_pin,

	.pwm_pin = &pwmb_pin,
	.pwm_af = GPIO_AF2,
	.pwm = &pwmb_cfg
};

static TB6612FNG_t tb6612fng = {
	.stby = &tb6612fng_stby_pin,
	.channel_a = &tb6612fng_cha,
	.channel_b = &tb6612fng_chb
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
