/*
 * Application behavior belongs here.
 *
 * This file should describe what the firmware does,
 * not how peripherals are configured.
 */

#include "app.h"

#include <stddef.h>

#include "app_config.h"
#include "device_instances.h"

#include "systick/systick.h"
#include "uart/uart.h"

#include "led/led.h"
#include "tb6612fng/tb6612fng.h"
#include "rotary_encoder/rotary_encoder.h"
#include "dc_motor/dc_motor.h"

DC_MOTOR_t *dc_motor;
ROTARY_ENCODER_t *rotary_encoder;

TB6612FNG_Direction_t direction;

static uint32_t previous_toggle_ms;
static uint32_t previous_motor_switch_ms;
static uint32_t previous_update_ms;
static uint32_t previous_print_ms;

static bool rotary_encoder_update_success;

static uint8_t stop_snapshots_printed;

static void app_print_encoder_position(USART_TypeDef *USARTx, ROTARY_ENCODER_t *rotary_encoder)
{
	uart_write_str(USARTx, "[ROTARY_ENCODER] Direction: ");
	if (rotary_encoder_get_direction(rotary_encoder) == TIM_ENCODER_DIRECTION_UP) uart_write_str(USARTx, "UP | ");
	else uart_write_str(USARTx, "DOWN | ");

	uart_write_str(USARTx, "Count: ");
	uart_write_int(USARTx, rotary_encoder_get_total_count(rotary_encoder));
	uart_write_str(USARTx, " | ");

	uart_write_str(USARTx, "Revolutions: ");
	uart_write_float(USARTx, rotary_encoder_get_revolutions(rotary_encoder));
	uart_write_str(USARTx, " | ");

	uart_write_str(USARTx, "Cumulative angle: ");
	uart_write_float(USARTx, rotary_encoder_get_cumulative_angle_degrees(rotary_encoder));
	uart_write_str(USARTx, " deg | ");

	uart_write_str(USARTx, "Normalized angle: ");
	uart_write_float(USARTx, rotary_encoder_get_normalized_angle_degrees(rotary_encoder));
	uart_write_line(USARTx, " deg");
}

static void app_print_encoder_distance(USART_TypeDef *USARTx, ROTARY_ENCODER_t *rotary_encoder)
{
	uart_write_str(USARTx, "[ROTARY_ENCODER] Displacement: ");
	uart_write_float(USARTx, rotary_encoder_get_displacement_mm(rotary_encoder));
	uart_write_str(USARTx, " mm | ");

	uart_write_str(USARTx, "Distance: ");
	uart_write_float(USARTx, rotary_encoder_get_total_distance_mm(rotary_encoder));
	uart_write_line(USARTx, " mm");
}

static void app_print_encoder_velocity(USART_TypeDef *USARTx, ROTARY_ENCODER_t *rotary_encoder)
{
	uart_write_str(USARTx, "[ROTARY_ENCODER] Rev/s: ");
	uart_write_float(USARTx, rotary_encoder_get_revolutions_per_second(rotary_encoder));
	uart_write_str(USARTx, " | ");

	uart_write_str(USARTx, "RPM: ");
	uart_write_float(USARTx, rotary_encoder_get_rpm(rotary_encoder));
	uart_write_str(USARTx, " | ");

	uart_write_str(USARTx, "Deg/s: ");
	uart_write_float(USARTx, rotary_encoder_get_degrees_per_second(rotary_encoder));
	uart_write_str(USARTx, " | ");

//	uart_write_str(USARTx, "Rad/s: ");
//	uart_write_float(USARTx, rotary_encoder_get_radians_per_second(rotary_encoder));
//	uart_write_str(USARTx, " | ");

	uart_write_str(USARTx, "Velocity: ");
	uart_write_float(USARTx, rotary_encoder_get_linear_velocity_mm_per_second(rotary_encoder));
	uart_write_str(USARTx, " mm/s | ");

	uart_write_str(USARTx, "Top speed: ");
	uart_write_float(USARTx, rotary_encoder_get_top_speed_mm_per_second(rotary_encoder));
	uart_write_str(USARTx, " mm/s | ");

	uart_write_str(USARTx, "Motion: ");
	if (rotary_encoder->motion == ROTARY_ENCODER_UP) uart_write_line(USARTx, "UP");
	else if (rotary_encoder->motion == ROTARY_ENCODER_DOWN) uart_write_line(USARTx, "DOWN");
	else if (rotary_encoder->motion == ROTARY_ENCODER_STOPPED) uart_write_line(USARTx, "STOPPED");
	else uart_write_line(USARTx, "UNKNOWN");
}

static void app_print_encoder_all(USART_TypeDef *USARTx, ROTARY_ENCODER_t *rotary_encoder)
{
	app_print_encoder_position(USARTx, rotary_encoder);
	app_print_encoder_distance(USARTx, rotary_encoder);
	app_print_encoder_velocity(USARTx, rotary_encoder);
}

static void app_handle_encoder_print(USART_TypeDef *USARTx, ROTARY_ENCODER_t *encoder)
{
	ROTARY_ENCODER_Motion_t motion = rotary_encoder_get_motion(encoder);

	if (motion == ROTARY_ENCODER_STOPPED)
	{
		if (stop_snapshots_printed >= APP_STOPPED_SNAPSHOT_LIMIT) return;
		stop_snapshots_printed++;
	}
	else
	{
		stop_snapshots_printed = 0U;
	}

	app_print_encoder_all(USARTx, encoder);
	uart_write_line(USARTx, "");
}

void app_init(void)
{
	dc_motor = board_get_motor();
	if (dc_motor == NULL) return;

	rotary_encoder = dc_motor->encoder;
	if (rotary_encoder == NULL) return;

	rotary_encoder_update_success = rotary_encoder_set_count_zero(rotary_encoder) == ROTARY_ENCODER_OK;

	dc_motor_set_speed_and_direction(dc_motor, TB6612FNG_DIRECTION_CW, 0);
	tb6612fng_set_duty_cycle(dc_motor->motor_driver, TB6612FNG_CHB, APP_MOTOR_FULL_SPEED);

	uart_write_line(USART2, "SUCCESSFUL BOOT");
	systick_delay_s(2);

	direction = TB6612FNG_DIRECTION_CW;

	previous_toggle_ms = millis();
	previous_motor_switch_ms = millis();
	previous_update_ms = millis();
	previous_print_ms = millis();

	stop_snapshots_printed = 0;
}

void app_update(void)
{
	uint32_t current_ms = millis();
	rotary_encoder_update_distance(rotary_encoder);

	if (current_ms - previous_toggle_ms >= APP_STATUS_LED_PERIOD_MS)
	{
		previous_toggle_ms = current_ms;
		led_toggle(board_get_status_led());
	}

	if (current_ms - previous_update_ms >= APP_ROTARY_ENCODER_UPDATE_PERIOD_MS)
	{
		previous_update_ms = current_ms;
		rotary_encoder_update_success = rotary_encoder_update_velocity(rotary_encoder, current_ms) == ROTARY_ENCODER_OK;
	}

	if (current_ms - previous_print_ms >= APP_ROTARY_ENCODER_PRINT_PERIOD_MS)
	{
		previous_print_ms = current_ms;

		if (rotary_encoder_update_success) app_handle_encoder_print(USART2, rotary_encoder);
		else uart_write_line(USART2, "[ROTARY_ENCODER] Update failed");
	}

	if (current_ms - previous_motor_switch_ms >= APP_MOTOR_DIR_SWITCH_INTERVAL_MS)
	{
		previous_motor_switch_ms = current_ms;

		if (direction == TB6612FNG_DIRECTION_CW)
		{
			direction = TB6612FNG_DIRECTION_CCW;
			dc_motor_set_speed_and_direction(dc_motor, direction, APP_MOTOR_FULL_SPEED);
		}
		else if (direction == TB6612FNG_DIRECTION_CCW)
		{
			direction = TB6612FNG_DIRECTION_CW;
			dc_motor_set_speed_and_direction(dc_motor, direction, APP_MOTOR_FULL_SPEED);
		}
	}
}
