#include "gpio_drv.h"
#include <xc.h>

/*
 * Internal helper macro
 * Ensures pin index is within valid range
 */
#define GPIO_PIN_VALID(pin)   ((pin) < 32u)

/* ---------------------------------------------------------
 * Configure GPIO pin direction
 * --------------------------------------------------------- */
void gpio_configure_pin(gpio_port_id_t port,
                        uint8_t pin,
                        gpio_direction_t direction)
{
    if (!GPIO_PIN_VALID(pin))
        return;

    if (direction == GPIO_DIR_OUTPUT)
    {
        PORT_REGS->GROUP[port].PORT_DIRSET = (1u << pin);
    }
    else
    {
        PORT_REGS->GROUP[port].PORT_DIRCLR = (1u << pin);
    }
}

/* ---------------------------------------------------------
 * Drive GPIO pin HIGH
 * --------------------------------------------------------- */
void gpio_write_high(gpio_port_id_t port, uint8_t pin)
{
    if (!GPIO_PIN_VALID(pin))
        return;

    PORT_REGS->GROUP[port].PORT_OUTSET = (1u << pin);
}

/* ---------------------------------------------------------
 * Drive GPIO pin LOW
 * --------------------------------------------------------- */
void gpio_write_low(gpio_port_id_t port, uint8_t pin)
{
    if (!GPIO_PIN_VALID(pin))
        return;

    PORT_REGS->GROUP[port].PORT_OUTCLR = (1u << pin);
}

/* ---------------------------------------------------------
 * Toggle GPIO pin state
 * --------------------------------------------------------- */
void gpio_write_toggle(gpio_port_id_t port, uint8_t pin)
{
    if (!GPIO_PIN_VALID(pin))
        return;

    PORT_REGS->GROUP[port].PORT_OUTTGL = (1u << pin);
}

/* ---------------------------------------------------------
 * Read GPIO input level
 * --------------------------------------------------------- */
bool gpio_read_pin(gpio_port_id_t port, uint8_t pin)
{
    if (!GPIO_PIN_VALID(pin))
        return false;

    return ((PORT_REGS->GROUP[port].PORT_IN >> pin) & 0x1u) != 0u;
}
