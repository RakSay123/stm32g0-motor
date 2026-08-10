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

static uint32_t previous_toggle_ms;

void app_init(void)
{
	uart_write_line(USART2, "SUCCESSFUL BOOT");

	previous_toggle_ms = millis();
}

void app_update(void)
{
	uint32_t current_ms = millis();

	if (current_ms - previous_toggle_ms >= APP_STATUS_LED_PERIOD_MS)
	{
		previous_toggle_ms = current_ms;
		led_toggle(board_get_status_led());
	}
}
