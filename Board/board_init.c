/*
 * Hardware initialization.
 *
 * Initializes board resources in dependency order and reports
 * failures through BOARD_Status_t.
 */

#include "board_init.h"

#include <stdint.h>
#include <stddef.h>

#include "board_config.h"
#include "device_instances.h"

#include "gpio/gpio.h"
#include "systick/systick.h"
#include "timer/timer.h"
#include "timer/timer_pwm/timer_pwm.h"
#include "timer/timer_encoder/timer_encoder.h"
#include "uart/uart.h"

#include "led/led.h"
#include "tb6612fng/tb6612fng.h"
#include "dc_motor/dc_motor.h"

GPIO_Config_t usart2_tx = {
	.port = GPIOA,
	.pin = 2,
	.mode = GPIO_MODE_AF,
	.otype = GPIO_PUSH_PULL,
	.speed = GPIO_SPEED_HIGH,
	.pull = GPIO_NO_PULL,
	.alternate = GPIO_AF1
};

GPIO_Config_t usart2_rx = {
	.port = GPIOA,
	.pin = 3,
	.mode = GPIO_MODE_AF,
	.otype = GPIO_PUSH_PULL,
	.speed = GPIO_SPEED_HIGH,
	.pull = GPIO_NO_PULL,
	.alternate = GPIO_AF1
};

UART_Config_t usart2_cfg = {
	.USARTx = USART2,
	.fclk = BOARD_FCLK_HZ,
	.baud_rate = BOARD_DEBUG_UART_BAUD,
};

TIM_Config_t tim3_cfg = {
	.TIMx = TIM3,
	.psc = BOARD_TIM3_PSC,
	.arr = BOARD_TIM3_ARR,
	.cnt = BOARD_TIM3_CNT,
	.interrupt_status = TIM_INTERRUPTS_DISABLED
};

BOARD_Status_t board_init(void)
{
	LED_t *status_led = board_get_status_led();
	TB6612FNG_t *tb6612fng = board_get_tb6612fng();
	DC_MOTOR_t *motor = board_get_motor();

	if (status_led == NULL) return BOARD_STATUS_LED_ERROR;
	if (tb6612fng == NULL) return BOARD_STATUS_TB6612FNG_ERROR;
	if (motor == NULL) return BOARD_STATUS_DC_MOTOR_ERROR;

	gpio_init(&usart2_tx);
	gpio_init(&usart2_rx);
	systick_init(BOARD_FCLK_HZ / BOARD_SYSTICK_HZ);
	if (timer_init(&tim3_cfg) != TIM_OK) return BOARD_STATUS_TIM3_ERROR;
	uart_init(&usart2_cfg);

	led_init(status_led); // void return type at the moment
	if (tb6612fng_init(tb6612fng) != TB6612FNG_OK) return BOARD_STATUS_TB6612FNG_ERROR;
	if (dc_motor_init(motor) != DC_MOTOR_OK) return BOARD_STATUS_DC_MOTOR_ERROR;

	return BOARD_STATUS_OK;
}
