#include "i2c.h"
#include "pic32cx1025sg61128.h"


static void i2c_clock_init(void)
{
    MCLK_REGS->MCLK_APBAMASK |= MCLK_APBDMASK_SERCOM6_Msk;

    GCLK_REGS->GCLK_PCHCTRL[SERCOM6_GCLK_ID_CORE] =
    GCLK_PCHCTRL_GEN_GCLK0 | GCLK_PCHCTRL_CHEN_Msk;

    while (!(GCLK_REGS->GCLK_PCHCTRL[SERCOM6_GCLK_ID_CORE] =
             GCLK_PCHCTRL_CHEN_Msk));
}


static void i2c_pins_init(void)
{
    /* Enable PMUX */
    PORT_REGS->GROUP[I2C_PORT].PORT_PINCFG[I2C_SDA_PIN] |= PORT_PINCFG_PMUXEN_Msk;
    PORT_REGS->GROUP[I2C_PORT].PORT_PINCFG[I2C_SCL_PIN] |= PORT_PINCFG_PMUXEN_Msk;

    /* PD09 (odd) → SDA → SERCOM6 PAD0 */
    PORT_REGS->GROUP[I2C_PORT].PORT_PMUX[I2C_SDA_PIN >> 1] =
        (PORT_REGS->GROUP[I2C_PORT].PORT_PMUX[I2C_SDA_PIN >> 1] & 0x0F) |
        (MUX_PD09D_SERCOM6_PAD0 << 4);

    /* PD08 (even) → SCL → SERCOM6 PAD1 */
    PORT_REGS->GROUP[I2C_PORT].PORT_PMUX[I2C_SCL_PIN >> 1] =
        (PORT_REGS->GROUP[I2C_PORT].PORT_PMUX[I2C_SCL_PIN >> 1] & 0xF0) |
        MUX_PD08D_SERCOM6_PAD1;

    /* Pull-ups (OK for test, external recommended) */
    PORT_REGS->GROUP[I2C_PORT].PORT_PINCFG[I2C_SDA_PIN] |= PORT_PINCFG_PULLEN_Msk;
    PORT_REGS->GROUP[I2C_PORT].PORT_PINCFG[I2C_SCL_PIN] |= PORT_PINCFG_PULLEN_Msk;

    PORT_REGS->GROUP[I2C_PORT].PORT_OUTSET =
        (1 << I2C_SDA_PIN) | (1 << I2C_SCL_PIN);
}


void i2c_init(void)
{
    i2c_clock_init();
    i2c_pins_init();

    /* -------- SERCOM RESET (CORRECT WAY) -------- */

    /* Make sure SERCOM is disabled */
    I2C_SERCOM->I2CM.SERCOM_CTRLA &= ~SERCOM_I2CM_CTRLA_ENABLE_Msk;
    while (I2C_SERCOM->I2CM.SERCOM_SYNCBUSY & SERCOM_I2CM_SYNCBUSY_ENABLE_Msk);

    /* Software reset */
    I2C_SERCOM->I2CM.SERCOM_CTRLA |= SERCOM_I2CM_CTRLA_SWRST_Msk;
    while (I2C_SERCOM->I2CM.SERCOM_SYNCBUSY & SERCOM_I2CM_SYNCBUSY_SWRST_Msk);

    /* -------- CONFIGURATION -------- */

    /* CTRLA: I2C Master, PAD0=SDA, PAD1=SCL */
    I2C_SERCOM->I2CM.SERCOM_CTRLA =
          SERCOM_I2CM_CTRLA_MODE_I2C_MASTER
        | SERCOM_I2CM_CTRLA_PINOUT(0)   /* REQUIRED for PD08/PD09 */
        | SERCOM_I2CM_CTRLA_SDAHOLD(3);

    /* CTRLB: Smart mode */
    I2C_SERCOM->I2CM.SERCOM_CTRLB =
          SERCOM_I2CM_CTRLB_SMEN_Msk;
    while (I2C_SERCOM->I2CM.SERCOM_SYNCBUSY & SERCOM_I2CM_SYNCBUSY_SYSOP_Msk);

    /* -------- BAUD RATE -------- */
    I2C_SERCOM->I2CM.SERCOM_BAUD = 232;  /* OK for now */

    /* -------- ENABLE -------- */
    I2C_SERCOM->I2CM.SERCOM_CTRLA |= SERCOM_I2CM_CTRLA_ENABLE_Msk;
    while (I2C_SERCOM->I2CM.SERCOM_SYNCBUSY & SERCOM_I2CM_SYNCBUSY_ENABLE_Msk);

    /* -------- FORCE BUS IDLE -------- */
    I2C_SERCOM->I2CM.SERCOM_STATUS =
        (I2C_SERCOM->I2CM.SERCOM_STATUS & ~SERCOM_I2CM_STATUS_BUSSTATE_Msk) |
        SERCOM_I2CM_STATUS_BUSSTATE(1);

    while (I2C_SERCOM->I2CM.SERCOM_SYNCBUSY & SERCOM_I2CM_SYNCBUSY_SYSOP_Msk);
}

bool i2c_start(uint8_t addr, bool read)
{
    /* Send address + R/W */
    I2C_SERCOM->I2CM.SERCOM_ADDR =
        (addr << 1) | (read ? 1 : 0);

    if (read)
    {
        /* Wait for Slave on Bus (SB) */
        while (!(I2C_SERCOM->I2CM.SERCOM_INTFLAG &
                 SERCOM_I2CM_INTFLAG_SB_Msk))
        {
            /* If NACK during address */
            if (I2C_SERCOM->I2CM.SERCOM_STATUS &
                SERCOM_I2CM_STATUS_RXNACK_Msk)
            {
                return false;
            }
        }
    }
    else
    {
        /* Wait for Master on Bus (MB) */
        while (!(I2C_SERCOM->I2CM.SERCOM_INTFLAG &
                 SERCOM_I2CM_INTFLAG_MB_Msk))
        {
            /* If NACK during address */
            if (I2C_SERCOM->I2CM.SERCOM_STATUS &
                SERCOM_I2CM_STATUS_RXNACK_Msk)
            {
                return false;
            }
        }
    }

    return true; /* Address ACKed */
}

bool i2c_write(uint8_t data)
{
    /* Write data byte */
    I2C_SERCOM->I2CM.SERCOM_DATA = data;

    /* Wait for byte complete */
    while (!(I2C_SERCOM->I2CM.SERCOM_INTFLAG &
             SERCOM_I2CM_INTFLAG_MB_Msk))
    {
        /* Optional: timeout here */
    }

    /* Check for NACK */
    if (I2C_SERCOM->I2CM.SERCOM_STATUS &
        SERCOM_I2CM_STATUS_RXNACK_Msk)
    {
        return false; /* Slave NACKed */
    }

    return true; /* ACK received */
}


uint8_t i2c_read(bool ack)
{
    /* Wait for byte received */
    while (!(I2C_SERCOM->I2CM.SERCOM_INTFLAG &
             SERCOM_I2CM_INTFLAG_SB_Msk))
    {
        /* optional timeout */
    }

    uint32_t ctrlb = I2C_SERCOM->I2CM.SERCOM_CTRLB;

    if (ack)
    {
        /* ACK and continue */
        ctrlb &= ~SERCOM_I2CM_CTRLB_ACKACT_Msk;   // ACKACT = 0
        ctrlb = (ctrlb & ~SERCOM_I2CM_CTRLB_CMD_Msk) |
                SERCOM_I2CM_CTRLB_CMD(0x2);
    }
    else
    {
        /* NACK and STOP */
        ctrlb |= SERCOM_I2CM_CTRLB_ACKACT_Msk;    // ACKACT = 1
        ctrlb = (ctrlb & ~SERCOM_I2CM_CTRLB_CMD_Msk) |
                SERCOM_I2CM_CTRLB_CMD(0x3);
    }

    I2C_SERCOM->I2CM.SERCOM_CTRLB = ctrlb;

    /* Now read data (this triggers ACK/NACK on bus) */
    return (uint8_t)I2C_SERCOM->I2CM.SERCOM_DATA;
}


void i2c_stop(void)
{
    uint32_t ctrlb = I2C_SERCOM->I2CM.SERCOM_CTRLB;

    /* Clear ACKACT (optional, safe) and set STOP command */
    ctrlb &= ~SERCOM_I2CM_CTRLB_ACKACT_Msk;
    ctrlb = (ctrlb & ~SERCOM_I2CM_CTRLB_CMD_Msk) | SERCOM_I2CM_CTRLB_CMD(0x3);

    I2C_SERCOM->I2CM.SERCOM_CTRLB = ctrlb;

    /* Wait for SYSOP (bus idle) */
    uint32_t timeout = 100000;
    while (I2C_SERCOM->I2CM.SERCOM_SYNCBUSY & SERCOM_I2CM_SYNCBUSY_SYSOP_Msk)
    {
        if (--timeout == 0)
        {
            /* Optional: handle bus lock */
            break;
        }
    }
}
