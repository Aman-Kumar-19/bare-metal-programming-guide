#ifndef SERCOM7_USART_H
#define SERCOM7_USART_H

#include <stdint.h>

/**
 * @brief Initialize SERCOM7 USART peripheral
 *
 * @param baudrate Desired baud rate (e.g., 9600, 115200)
 */
void SERCOM7_USART_Init(uint32_t baudrate);

/**
 * @brief Send one byte over USART
 */
void SERCOM7_USART_WriteByte(uint8_t data);

/**
 * @brief Receive one byte over USART
 */
uint8_t SERCOM7_USART_ReadByte(void);

/**
 * @brief Send a null-terminated string over USART
 */
void SERCOM7_USART_WriteString(const char *str);

#endif /* SERCOM7_USART_H */
