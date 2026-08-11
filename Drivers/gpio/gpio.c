#include "gpio.h"

void gpio_enable_clock(GPIO_TypeDef *GPIOx)
{

	if (GPIOx == GPIOA)
	{
		RCC->IOPENR |= (1U << 0);
	}
	else if (GPIOx == GPIOB)
	{
		RCC->IOPENR |= (1U << 1);
	}
	else if (GPIOx == GPIOC)
	{
		RCC->IOPENR |= (1U << 2);
	}
	else if (GPIOx == GPIOD)
	{
		RCC->IOPENR |= (1U << 3);
	}
#ifdef GPIOE
	else if (GPIOx == GPIOE)
	{
		RCC->IOPENR |= (1U << 4);
	}
#endif
	else if (GPIOx == GPIOF)
	{
		RCC->IOPENR |= (1U << 5);
	}
	else
	{
		// Error handling to be implemented here in the future
	}
}

void gpio_mode_config(GPIO_TypeDef *GPIOx, uint8_t pin, GPIO_Mode_t mode)
{
	GPIOx->MODER = (GPIOx->MODER & ~(3U << (2 * pin))) | ((uint32_t)mode << (2 * pin));
}

void gpio_output_type_config(GPIO_TypeDef *GPIOx, uint8_t pin, GPIO_OType_t otype)
{
	if (otype == GPIO_OPEN_DRAIN)
	{
		GPIOx->OTYPER |= (1U << pin);
	}
	else
	{
		GPIOx->OTYPER &= ~(1U << pin);
	}
}

void gpio_speed_config(GPIO_TypeDef *GPIOx, uint8_t pin, GPIO_Speed_t speed)
{
	GPIOx->OSPEEDR = (GPIOx->OSPEEDR & ~(3U << (2 * pin))) | ((uint32_t)speed << (2 * pin));
}

void gpio_pull_config(GPIO_TypeDef *GPIOx, uint8_t pin, GPIO_Pull_t pull)
{
	GPIOx->PUPDR = (GPIOx->PUPDR & ~(3U << (2 * pin))) | ((uint32_t)pull << (2 * pin));
}

void gpio_alternate_function_config(GPIO_TypeDef *GPIOx, uint8_t pin, GPIO_AF_t alternate)
{
	if (pin < 8)
	{
		GPIOx->AFR[0] = (GPIOx->AFR[0] & ~(15U << (4 * pin))) | ((uint32_t)alternate << (4 * pin));
	}
	else
	{
		uint8_t pos = pin - 8;
		GPIOx->AFR[1] = (GPIOx->AFR[1] & ~(15U << (4 * pos))) | ((uint32_t)alternate << (4 * pos));
	}
}

void gpio_init(GPIO_Config_t *cfg)
{
	gpio_enable_clock(cfg->port);
	gpio_mode_config(cfg->port, cfg->pin, cfg->mode);

	if (cfg->mode == GPIO_MODE_AF)
	{
		gpio_alternate_function_config(cfg->port, cfg->pin, cfg->alternate);
	}

	if (cfg->mode == GPIO_MODE_OUTPUT || cfg->mode == GPIO_MODE_AF)
	{
		gpio_output_type_config(cfg->port, cfg->pin, cfg->otype);
		gpio_speed_config(cfg->port, cfg->pin, cfg->speed);
	}

	if (cfg->mode != GPIO_MODE_ANALOG)
	{
		gpio_pull_config(cfg->port, cfg->pin, cfg->pull);
	}
}

void gpio_write(GPIO_Pin_t *gpio, GPIO_State_t state)
{
	if (state == GPIO_HIGH)
	{
		gpio->port->BSRR |= (1U << gpio->pin);
	}
	else if (state == GPIO_LOW)
	{
		gpio->port->BSRR |= (1U << (gpio->pin + 16));
	}
}

void gpio_toggle(GPIO_Pin_t *gpio)
{
	if (gpio->port->ODR & (1U << gpio->pin))
	{
		gpio->port->BSRR |= (1U << (gpio->pin + 16));
	}
	else
	{
		gpio->port->BSRR |= (1U << gpio->pin);
	}
}

void gpio_toggle_pin(GPIO_Config_t *cfg)
{
	if (cfg->port->ODR & (1U << cfg->pin))
	{
		cfg->port->BSRR |= (1U << (cfg->pin + 16));
	}
	else
	{
		cfg->port->BSRR |= (1U << cfg->pin);
	}
}

GPIO_State_t gpio_read(GPIO_Pin_t *gpio)
{
	if (gpio->port->IDR & (1U << gpio->pin))
	{
		return GPIO_HIGH;
	}
	else
	{
		return GPIO_LOW;
	}
}

GPIO_State_t gpio_read_pin(GPIO_Config_t *cfg)
{
	GPIO_Pin_t gpio = {
		.port = cfg->port,
		.pin = cfg->pin
	};

	return gpio_read(&gpio);
}
