#include "pic32cx1025sg61128.h"
#include "timer_counter_drv.h"

/* ================= TC BASE TABLE ================= */
#define TC_MAX 8
#define TC_CHANNELS 2

static tc_registers_t *const tc_table[] =
{
    TC0_REGS, TC1_REGS, TC2_REGS, TC3_REGS,
    TC4_REGS, TC5_REGS, TC6_REGS, TC7_REGS
};

/* ================= CALLBACK STORAGE ================= */
static void (*tc_callbacks[TC_MAX][TC_CHANNELS])(void) = {0};

/* =========================================================
 * TC Clock Configuration Tables (Datasheet Accurate)
 * ========================================================= */
static volatile uint32_t *const tc_apb_mask_reg[] =
{
    &MCLK_REGS->MCLK_APBAMASK, /* TC0 */
    &MCLK_REGS->MCLK_APBAMASK, /* TC1 */
    &MCLK_REGS->MCLK_APBBMASK, /* TC2 */
    &MCLK_REGS->MCLK_APBBMASK, /* TC3 */
    &MCLK_REGS->MCLK_APBCMASK, /* TC4 */
    &MCLK_REGS->MCLK_APBCMASK, /* TC5 */
    &MCLK_REGS->MCLK_APBDMASK, /* TC6 */
    &MCLK_REGS->MCLK_APBDMASK  /* TC7 */
};

static const uint32_t tc_apb_mask_bit[] =
{
    MCLK_APBAMASK_TC0_Msk,
    MCLK_APBAMASK_TC1_Msk,
    MCLK_APBBMASK_TC2_Msk,
    MCLK_APBBMASK_TC3_Msk,
    MCLK_APBCMASK_TC4_Msk,
    MCLK_APBCMASK_TC5_Msk,
    MCLK_APBDMASK_TC6_Msk,
    MCLK_APBDMASK_TC7_Msk
};

static const uint8_t tc_gclk_id[] =
{
    9,  /* TC0 */
    9,  /* TC1 */
    26, /* TC2 */
    26, /* TC3 */
    30, /* TC4 */
    30, /* TC5 */
    39, /* TC6 */
    39  /* TC7 */
};

/* ================= CLOCK ENABLE ================= */
static void tc_clock_enable(uint8_t tc_index)
{
    // Enable APB clock for this TC
    *tc_apb_mask_reg[tc_index] |= tc_apb_mask_bit[tc_index];

    // Enable GCLK for this TC
    GCLK_REGS->GCLK_PCHCTRL[tc_gclk_id[tc_index]] =
        GCLK_PCHCTRL_GEN_GCLK0 | GCLK_PCHCTRL_CHEN_Msk;

    // Wait until clock is ready
    while (!(GCLK_REGS->GCLK_PCHCTRL[tc_gclk_id[tc_index]] & GCLK_PCHCTRL_CHEN_Msk));
}

/* ================= INITIALIZATION ================= */
void tc_init(uint8_t tc_index,
             tc_mode_t mode,
             tc_prescaler_t prescaler,
             tc_waveform_t waveform,
             uint32_t compare_value)
{
    tc_registers_t *tc = tc_table[tc_index];

    tc_clock_enable(tc_index);

    /* Disable TC before configuring */
    tc->COUNT16.TC_CTRLA &= ~TC_CTRLA_ENABLE_Msk;
    while (tc->COUNT16.TC_SYNCBUSY & TC_SYNCBUSY_ENABLE_Msk);

    /* Software reset */
    tc->COUNT16.TC_CTRLA |= TC_CTRLA_SWRST_Msk;
    while (tc->COUNT16.TC_SYNCBUSY & TC_SYNCBUSY_SWRST_Msk);

    /* Set mode & prescaler */
    tc->COUNT16.TC_CTRLA = TC_CTRLA_MODE(mode) | TC_CTRLA_PRESCALER(prescaler);

    /* Waveform generation */
    tc->COUNT16.TC_WAVE = waveform;
    while (tc->COUNT16.TC_SYNCBUSY & TC_SYNCBUSY_ENABLE_Msk);

    /* Compare value */
    tc->COUNT16.TC_CC[0] = compare_value;
    while (tc->COUNT16.TC_SYNCBUSY & TC_SYNCBUSY_CC0_Msk);

    /* Clear interrupts */
    tc->COUNT16.TC_INTFLAG = TC_INTFLAG_Msk;
}

/* ================= CONTROL ================= */
void tc_start(uint8_t tc_index)
{
    tc_registers_t *tc = tc_table[tc_index];
    tc->COUNT16.TC_CTRLA |= TC_CTRLA_ENABLE_Msk;
    while (tc->COUNT16.TC_SYNCBUSY & TC_SYNCBUSY_ENABLE_Msk);
}

void tc_stop(uint8_t tc_index)
{
    tc_registers_t *tc = tc_table[tc_index];
    tc->COUNT16.TC_CTRLA &= ~TC_CTRLA_ENABLE_Msk;
    while (tc->COUNT16.TC_SYNCBUSY & TC_SYNCBUSY_ENABLE_Msk);
}

/* ================= COMPARE ================= */
void tc_set_compare(uint8_t tc_index, uint32_t value)
{
    tc_registers_t *tc = tc_table[tc_index];
    tc->COUNT16.TC_CC[0] = value;
    while (tc->COUNT16.TC_SYNCBUSY & TC_SYNCBUSY_CC0_Msk);
}

bool tc_compare_match(uint8_t tc_index)
{
    tc_registers_t *tc = tc_table[tc_index];
    if(tc->COUNT16.TC_INTFLAG & TC_INTFLAG_MC0_Msk)
    {
        tc->COUNT16.TC_INTFLAG = TC_INTFLAG_MC0_Msk;
        return true;
    }
    return false;
}

/* ================= READ COUNTER ================= */
uint16_t tc_get_count(uint8_t tc_index)
{
    tc_registers_t *tc = tc_table[tc_index];
    return tc->COUNT16.TC_COUNT; // Current counter value
}

/* ================= PWM ================= */
void tc_pwm_set_duty(uint8_t tc_index, uint32_t duty)
{
    tc_registers_t *tc = tc_table[tc_index];
    tc->COUNT16.TC_CC[1] = duty;
    while (tc->COUNT16.TC_SYNCBUSY & TC_SYNCBUSY_CC1_Msk);
}

/* ================= CAPTURE ================= */
void tc_capture_enable(uint8_t tc_index, uint8_t channel, tc_capture_mode_t mode, bool invert)
{
    tc_registers_t *tc = tc_table[tc_index];

    /* Enable capture channel */
    tc->COUNT16.TC_CTRLA |= (1U << (TC_CTRLA_CAPTEN0_Pos + channel));

    switch(mode)
    {
        case TC_CAPTURE_IO:
            tc->COUNT16.TC_CTRLA |= (1U << (TC_CTRLA_COPEN0_Pos + channel));
            if (invert)
                tc->COUNT16.TC_DRVCTRL |= (1U << (TC_DRVCTRL_INVEN0_Pos + channel));
            break;

        case TC_CAPTURE_EVENT:
            tc->COUNT16.TC_EVCTRL |= TC_EVCTRL_TCEI_Msk;
            if (invert)
                tc->COUNT16.TC_EVCTRL |= TC_EVCTRL_TCINV_Msk;
            break;

        case TC_CAPTURE_PPWP:
            tc->COUNT16.TC_EVCTRL |= TC_EVCTRL_EVACT_PPW;
            if (invert)
                tc->COUNT16.TC_EVCTRL |= TC_EVCTRL_TCINV_Msk;
            break;

        case TC_CAPTURE_PWP:
            tc->COUNT16.TC_EVCTRL |= TC_EVCTRL_EVACT_PW;
            if (invert)
                tc->COUNT16.TC_EVCTRL |= TC_EVCTRL_TCINV_Msk;
            break;

        default: break;
    }

    /* Clear interrupt flags */
    tc->COUNT16.TC_INTFLAG = TC_INTFLAG_MC0_Msk << channel;
}

uint16_t tc_capture_read(uint8_t tc_index, uint8_t channel)
{
    tc_registers_t *tc = tc_table[tc_index];
    while (!(tc->COUNT16.TC_INTFLAG & (TC_INTFLAG_MC0_Msk << channel)));
    uint16_t val = tc->COUNT16.TC_CC[channel];
    tc->COUNT16.TC_INTFLAG = (TC_INTFLAG_MC0_Msk << channel);
    return val;
}

/* ================= INTERRUPTS ================= */
void tc_enable_interrupt(uint8_t tc_index, uint32_t flags)
{
    tc_registers_t *tc = tc_table[tc_index];
    tc->COUNT16.TC_INTENSET = flags;
}

void tc_disable_interrupt(uint8_t tc_index, uint32_t flags)
{
    tc_registers_t *tc = tc_table[tc_index];
    tc->COUNT16.TC_INTENCLR = flags;
}

void tc_clear_interrupt(uint8_t tc_index, uint32_t flags)
{
    tc_registers_t *tc = tc_table[tc_index];
    tc->COUNT16.TC_INTFLAG = flags;
}

/* ================= CALLBACK REGISTRATION ================= */
void tc_register_callback(uint8_t tc_index, uint8_t channel, void (*callback)(void))
{
    if(tc_index < TC_MAX && channel < TC_CHANNELS)
    {
        tc_callbacks[tc_index][channel] = callback;
        tc_enable_interrupt(tc_index, 1 << channel);
    }
}

/* ================= COMMON ISR HANDLER ================= */
void TCx_Handler(uint8_t tc_index)
{
    tc_registers_t *tc = tc_table[tc_index];
    for(uint8_t ch = 0; ch < TC_CHANNELS; ch++)
    {
        if(tc->COUNT16.TC_INTFLAG & (1 << ch))
        {
            tc->COUNT16.TC_INTFLAG = (1 << ch); // clear flag
            if(tc_callbacks[tc_index][ch])
                tc_callbacks[tc_index][ch]();   // call user callback
        }
    }
}

/* ================= MAPPING ISR HANDLERS ================= */
void TC0_Handler(void) { TCx_Handler(0); }
void TC1_Handler(void) { TCx_Handler(1); }
void TC2_Handler(void) { TCx_Handler(2); }
void TC3_Handler(void) { TCx_Handler(3); }
void TC4_Handler(void) { TCx_Handler(4); }
void TC5_Handler(void) { TCx_Handler(5); }
void TC6_Handler(void) { TCx_Handler(6); }
void TC7_Handler(void) { TCx_Handler(7); }

/* ================= COUNTER CONTROL ================= */
void tc_set_oneshot(uint8_t tc_index, bool enable)
{
    tc_registers_t *tc = tc_table[tc_index];
    if(enable)
        tc->COUNT16.TC_CTRLBSET = TC_CTRLBSET_ONESHOT_Msk;
}

void tc_set_downcount(uint8_t tc_index, bool enable)
{
    tc_registers_t *tc = tc_table[tc_index];
    if(enable)
        tc->COUNT16.TC_CTRLBSET = TC_CTRLBSET_DIR_Msk;
}
