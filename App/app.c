/*
 * Application behavior belongs here.
 *
 * This file should describe what the firmware does,
 * not how peripherals are configured.
 */

#include "app.h"

#include "app_config.h"
#include "device_instances.h"

#include "systick/systick.h"
#include "uart/uart.h"

#include "led/led.h"
#include "tb6612fng/tb6612fng.h"
#include "dc_motor/dc_motor.h"

//TB6612FNG_t *tb6612fng = board_get_tb6612fng();

TB6612FNG_Direction_t direction;

static uint32_t previous_toggle_ms;
static uint32_t previous_motor_switch_ms;

void app_init(void)
{
	dc_motor_set_speed_and_direction(board_get_motor(), TB6612FNG_DIRECTION_CW, 0);
	tb6612fng_set_duty_cycle(board_get_tb6612fng(), TB6612FNG_CHB, 100);
	uart_write_line(USART2, "SUCCESSFUL BOOT");

	direction = TB6612FNG_DIRECTION_CW;

	previous_toggle_ms = millis();
	previous_motor_switch_ms = millis();
}

void app_update(void)
{
	uint32_t current_ms = millis();

	if (current_ms - previous_toggle_ms >= APP_STATUS_LED_PERIOD_MS)
	{
		previous_toggle_ms = current_ms;
		led_toggle(board_get_status_led());
	}

	if (current_ms - previous_motor_switch_ms >= APP_MOTOR_DIR_SWITCH_INTERVAL_MS)
	{
		previous_motor_switch_ms = current_ms;

		if (direction == TB6612FNG_DIRECTION_CW)
		{
			direction = TB6612FNG_DIRECTION_CCW;
			dc_motor_set_speed_and_direction(board_get_motor(), direction, 100);
		}
		else if (direction == TB6612FNG_DIRECTION_CCW)
		{
			direction = TB6612FNG_DIRECTION_CW;
			dc_motor_set_speed_and_direction(board_get_motor(), direction, 100);
		}
	}
}
