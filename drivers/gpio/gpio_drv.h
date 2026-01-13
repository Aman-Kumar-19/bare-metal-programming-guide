#ifndef GPIO_DRV_H
#define GPIO_DRV_H

#include <stdint.h>
#include <stdbool.h>

/*
 * GPIO Port Index
 * Maps logical port identifiers to PORT GROUP indices
 */
typedef enum
{
    GPIO_PORT0 = 0,
    GPIO_PORT1,
    GPIO_PORT2,
    GPIO_PORT3
} gpio_port_id_t;

/*
 * GPIO Pin Direction
 */
typedef enum
{
    GPIO_DIR_INPUT = 0,
    GPIO_DIR_OUTPUT
} gpio_direction_t;

/* ---------------- GPIO Configuration APIs ---------------- */

/* Configure direction of a GPIO pin */
void gpio_configure_pin(gpio_port_id_t port,
                        uint8_t pin,
                        gpio_direction_t direction);

/* ---------------- GPIO Output APIs ---------------- */

/* Set pin output to logic HIGH */
void gpio_write_high(gpio_port_id_t port, uint8_t pin);

/* Set pin output to logic LOW */
void gpio_write_low(gpio_port_id_t port, uint8_t pin);

/* Toggle pin output state */
void gpio_write_toggle(gpio_port_id_t port, uint8_t pin);

/* ---------------- GPIO Input API ---------------- */

/* Read current logic level on a pin */
bool gpio_read_pin(gpio_port_id_t port, uint8_t pin);

#endif /* GPIO_DRV_H */
