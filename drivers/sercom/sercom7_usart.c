#include "sercom7_usart.h"
#include <pic32cx1025sg61128.h>

/* ===================== Macros ===================== */
#define PORTC_INDEX        2
#define SERCOM7_GCLK_ID    37
#define SERCOM_SLOW_GCLK   3
#define SERCOM_REF_FREQ    48000000UL   // 48 MHz reference clock

/* ===================== Local Helpers ===================== */

/**
 * @brief Enable clocks required for SERCOM7 USART operation
 *
 * - Enables APBD bus clock for SERCOM7
 * - Connects GCLK0 to SERCOM7 core clock
 * - Enables shared SERCOM slow clock
 */
static void SERCOM7_USART_ClockInit(void)
{
    /* Enable APBD clock for SERCOM7 peripheral */
    MCLK_REGS->MCLK_APBDMASK |= MCLK_APBDMASK_SERCOM7_Msk;

    /* Route GCLK0 to SERCOM7 core */
    GCLK_REGS->GCLK_PCHCTRL[SERCOM7_GCLK_ID] =
        GCLK_PCHCTRL_GEN_GCLK0 |
        GCLK_PCHCTRL_CHEN_Msk;
    while (!(GCLK_REGS->GCLK_PCHCTRL[SERCOM7_GCLK_ID] & GCLK_PCHCTRL_CHEN_Msk));

    /* Enable SERCOM slow clock (shared among SERCOMs) */
    GCLK_REGS->GCLK_PCHCTRL[SERCOM_SLOW_GCLK] =
        GCLK_PCHCTRL_GEN_GCLK0 |
        GCLK_PCHCTRL_CHEN_Msk;
    while (!(GCLK_REGS->GCLK_PCHCTRL[SERCOM_SLOW_GCLK] & GCLK_PCHCTRL_CHEN_Msk));
}

/**
 * @brief Configure PC12 and PC13 for SERCOM7 USART
 *
 * PC12 → SERCOM7 PAD0 → TX  
 * PC13 → SERCOM7 PAD1 → RX  
 * Peripheral Function: C
 */
static void SERCOM7_USART_PinMuxInit(void)
{
    /* Enable peripheral multiplexing */
    PORT_REGS->GROUP[PORTC_INDEX].PORT_PINCFG[12] |= PORT_PINCFG_PMUXEN_Msk;
    PORT_REGS->GROUP[PORTC_INDEX].PORT_PINCFG[13] |= PORT_PINCFG_PMUXEN_Msk;

    /* Configure PMUX for PC12 & PC13 */
    PORT_REGS->GROUP[PORTC_INDEX].PORT_PMUX[12 >> 1] =
        PORT_PMUX_PMUXE(PORT_PMUX_PMUXE_C) |   // PC12
        PORT_PMUX_PMUXO(PORT_PMUX_PMUXO_C);    // PC13
}

/**
 * @brief Perform software reset of SERCOM7 USART
 *
 * Ensures peripheral starts from a known state
 */
static void SERCOM7_USART_SoftwareReset(void)
{
    SERCOM7_REGS->USART_INT.SERCOM_CTRLA |= SERCOM_USART_INT_CTRLA_SWRST_Msk;
    while (SERCOM7_REGS->USART_INT.SERCOM_CTRLA & SERCOM_USART_INT_CTRLA_SWRST_Msk);
}

/**
 * @brief Calculate BAUD register value (Async Arithmetic Mode)
 *
 * Formula:
 * BAUD = 65536 × (1 − (16 × Baudrate / fref))
 *
 * @param baudrate Desired baud rate (e.g., 115200)
 * @return Calculated BAUD register value
 */
static uint16_t SERCOM7_USART_CalcBaud(uint32_t baudrate)
{
    return (uint16_t)(
        65536.0f *
        (1.0f - ((16.0f * baudrate) / (float)SERCOM_REF_FREQ))
    );
}

/* ===================== Public APIs ===================== */

/**
 * @brief Initialize SERCOM7 as USART (8N1, async)
 *
 * @param baudrate Desired baud rate
 */
void SERCOM7_USART_Init(uint32_t baudrate)
{
    SERCOM7_USART_ClockInit();
    SERCOM7_USART_PinMuxInit();
    SERCOM7_USART_SoftwareReset();

    /* Configure USART mode */
    SERCOM7_REGS->USART_INT.SERCOM_CTRLA =
        SERCOM_USART_INT_CTRLA_MODE_USART_INT_CLK | // Internal clock
        SERCOM_USART_INT_CTRLA_RXPO_PAD1 |           // RX on PAD1
        SERCOM_USART_INT_CTRLA_TXPO_PAD0 |           // TX on PAD0
        SERCOM_USART_INT_CTRLA_DORD_LSB;              // LSB first

    /* Enable TX & RX, 8-bit data */
    SERCOM7_REGS->USART_INT.SERCOM_CTRLB =
        SERCOM_USART_INT_CTRLB_RXEN_Msk |
        SERCOM_USART_INT_CTRLB_TXEN_Msk |
        SERCOM_USART_INT_CTRLB_CHSIZE(0);

    while (SERCOM7_REGS->USART_INT.SERCOM_SYNCBUSY &
           SERCOM_USART_INT_SYNCBUSY_CTRLB_Msk);

    /* Set baud rate */
    SERCOM7_REGS->USART_INT.SERCOM_BAUD =
        SERCOM_USART_INT_BAUD_BAUD(SERCOM7_USART_CalcBaud(baudrate));

    /* Enable USART */
    SERCOM7_REGS->USART_INT.SERCOM_CTRLA |= SERCOM_USART_INT_CTRLA_ENABLE_Msk;
    while (SERCOM7_REGS->USART_INT.SERCOM_SYNCBUSY &
           SERCOM_USART_INT_SYNCBUSY_ENABLE_Msk);
}

/**
 * @brief Transmit one byte (blocking)
 */
void SERCOM7_USART_WriteByte(uint8_t data)
{
    while (!(SERCOM7_REGS->USART_INT.SERCOM_INTFLAG &
             SERCOM_USART_INT_INTFLAG_DRE_Msk));

    SERCOM7_REGS->USART_INT.SERCOM_DATA = data;
}

/**
 * @brief Receive one byte (blocking)
 */
uint8_t SERCOM7_USART_ReadByte(void)
{
    while (!(SERCOM7_REGS->USART_INT.SERCOM_INTFLAG &
             SERCOM_USART_INT_INTFLAG_RXC_Msk));

    return (uint8_t)(SERCOM7_REGS->USART_INT.SERCOM_DATA & 0xFF);
}

/**
 * @brief Transmit a null-terminated string
 */
void SERCOM7_USART_WriteString(const char *str)
{
    while (*str)
    {
        SERCOM7_USART_WriteByte((uint8_t)*str++);
    }
}
