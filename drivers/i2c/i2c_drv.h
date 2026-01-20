#ifndef I2C_CONFIG_H
#define I2C_CONFIG_H

#include <stdint.h>
#include <stdbool.h>




/* -------- I2C SERCOM selection -------- */
#define I2C_SERCOM       SERCOM6_REGS

/* -------- I2C pin configuration -------- */
/* PORT index: 0=PORTA, 1=PORTB, 2=PORTC, 3=PORTD */
#define I2C_PORT       3       // PORTD
#define I2C_SCL_PIN    8       // PD08  ✅ SCL
#define I2C_SDA_PIN    9       // PD09  ✅ SDA



/* ================= I2C PUBLIC API ================= */

/* Initialize I2C peripheral */
void i2c_init(void);

/* Start condition
 * addr = 7-bit slave address
 * read = true → read, false → write
 */
bool i2c_start(uint8_t addr, bool read);

/* Write one byte */
bool i2c_write(uint8_t data);

/* Read one byte
 * ack = true → ACK
 * ack = false → NACK
 */
uint8_t i2c_read(bool ack);

/* Stop condition */
void i2c_stop(void);

#endif /* I2C_H */
