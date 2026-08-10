#pragma once

typedef enum {
	DC_MOTOR_OK,
	DC_MOTOR_ERR
} DC_MOTOR_Status_t;

typedef struct {

} DC_MOTOR_t;

DC_MOTOR_Status_t dc_motor_init(DC_MOTOR_t *motor);
