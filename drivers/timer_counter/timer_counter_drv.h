#ifndef TIMER_COUNTER_DRV_H
#define TIMER_COUNTER_DRV_H

#include <stdint.h>
#include <stdbool.h>

/* ================= TC MODE ================= */
typedef enum
{
    TC_MODE_8BIT  = 0,
    TC_MODE_16BIT = 1,
    TC_MODE_32BIT = 2
} tc_mode_t;

/* ================= PRESCALER ================= */
typedef enum
{
    TC_PRESCALER_DIV1   = 0,
    TC_PRESCALER_DIV2   = 1,
    TC_PRESCALER_DIV4   = 2,
    TC_PRESCALER_DIV8   = 3,
    TC_PRESCALER_DIV16  = 4,
    TC_PRESCALER_DIV64  = 5,
    TC_PRESCALER_DIV256 = 6,
    TC_PRESCALER_DIV1024= 7
} tc_prescaler_t;

/* ================= WAVEFORM GENERATION ================= */
typedef enum
{
    TC_WAVE_NFRQ = 0,   // Normal Frequency
    TC_WAVE_MFRQ = 1,   // Match Frequency
    TC_WAVE_NPWM = 2,   // Normal PWM
    TC_WAVE_MPWM = 3    // Match PWM
} tc_waveform_t;

/* ================= CAPTURE MODES ================= */
typedef enum
{
    TC_CAPTURE_NONE = 0,
    TC_CAPTURE_IO,
    TC_CAPTURE_EVENT,
    TC_CAPTURE_PPWP,   // Period & Pulse-width
    TC_CAPTURE_PWP,    // Pulse width single edge
} tc_capture_mode_t;

/* ================= COUNTER CONFIG ================= */
typedef enum {
    TC_DIR_UP = 0,
    TC_DIR_DOWN = 1
} tc_counter_dir_t;

typedef enum {
    TC_CONTINUOUS = 0,
    TC_ONE_SHOT   = 1
} tc_oneshot_t;

/* ================= API ================= */
void tc_init(uint8_t tc_index,
             tc_mode_t mode,
             tc_prescaler_t prescaler,
             tc_waveform_t waveform,
             uint32_t compare_value);

void tc_start(uint8_t tc_index);
void tc_stop(uint8_t tc_index);

/* Compare & Counter */
void tc_set_compare(uint8_t tc_index, uint32_t value);
bool tc_compare_match(uint8_t tc_index);
uint16_t tc_get_count(uint8_t tc_index);  // Read current counter value

/* ================= PWM ================= */
void tc_pwm_set_duty(uint8_t tc_index, uint32_t duty);

/* ================= CAPTURE ================= */
void tc_capture_enable(uint8_t tc_index, uint8_t channel, tc_capture_mode_t mode, bool invert);
uint16_t tc_capture_read(uint8_t tc_index, uint8_t channel);

/* ================= INTERRUPTS ================= */
void tc_enable_interrupt(uint8_t tc_index, uint32_t flags);
void tc_disable_interrupt(uint8_t tc_index, uint32_t flags);
void tc_clear_interrupt(uint8_t tc_index, uint32_t flags);

/* ================= CALLBACKS ================= */
void tc_register_callback(uint8_t tc_index, uint8_t channel, void (*callback)(void));

/* ================= COUNTER CONTROL ================= */
void tc_set_oneshot(uint8_t tc_index, bool enable);
void tc_set_downcount(uint8_t tc_index, bool enable);

#endif /* TC1_H */
