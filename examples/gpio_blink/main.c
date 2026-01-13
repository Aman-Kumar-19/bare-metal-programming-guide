/**
 * @file main.c
 * @brief GPIO blink example using GPIO driver APIs
 *
 * Demonstrates basic GPIO output control using
 * abstracted GPIO driver functions instead of
 * direct register access.
 */

#include <xc.h>          /* Device-specific definitions */
#include <stdint.h>
#include "gpio.h"

/* --------------------------------------------------
 * LED pin configuration
 * -------------------------------------------------- */
#define LED1_PORT   GPIO_PORT_C
#define LED1_PIN    21    /* PC21 */

#define LED2_PORT   GPIO_PORT_A
#define LED2_PIN    16    /* PA16 */

/* --------------------------------------------------
 * Simple blocking delay (CPU busy-wait)
 * -------------------------------------------------- */
static void delay(volatile uint32_t count)
{
    while (count--)
    {
        __asm__("nop");
    }
}

int main(void)
{
    /* --------------------------------------------------
     * Configure GPIO pins as OUTPUT
     * -------------------------------------------------- */
    gpio_configure_pin(LED1_PORT, LED1_PIN, GPIO_DIR_OUTPUT);
    gpio_configure_pin(LED2_PORT, LED2_PIN, GPIO_DIR_OUTPUT);

    while (1)
    {
        /* LED1 ON, LED2 OFF */
        gpio_write_high(LED1_PORT, LED1_PIN);
        gpio_write_low(LED2_PORT, LED2_PIN);
        delay(100000);

        /* LED1 OFF, LED2 ON */
        gpio_write_low(LED1_PORT, LED1_PIN);
        gpio_write_high(LED2_PORT, LED2_PIN);
        delay(100000);
    }
}
