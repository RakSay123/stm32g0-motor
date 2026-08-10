#pragma once

typedef enum {
	TB6612FNG_OK,
	TB6612FNG_ERR
} TB6612FNG_Status_t;

typedef struct {

} TB6612FNG_t;

TB6612FNG_Status_t tb6612fng_init(TB6612FNG_t *tb6612fng);
