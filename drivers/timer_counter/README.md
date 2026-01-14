# ⏱️ Timer / Counter Driver – PIC32CX

This repository provides a configurable and register-level Timer/Counter driver designed for embedded systems and bare-metal firmware development.
The driver supports timer, counter, PWM, compare, and capture operations with interrupt handling and multiple operating modes.

The implementation follows a portable driver architecture, making it suitable for learning, research, and real-world embedded applications.

----
## 📌 Overview: Timer vs Counter
### 🕒 Timer
A timer increments or decrements based on an internal clock source (system clock or prescaled clock).
  It is used for:
  - Time measurement
  - Periodic task scheduling
  - PWM generation
  - Frequency generation
  - Timeout detection
---
### 🔢 Counter
A counter increments or decrements based on external events or pulses.
  It is used for:
  - Event counting
  - Pulse measurement
  - Frequency measurement
  - Signal analysis
This driver supports both timer and counter functionality within a single unified module.
---
## ✨ Key Features
### 🔹 Timer Operating Modes
- **Normal Frequency Generation (NFRQ):**
Counter runs continuously and overflows at the maximum value.
- **Match Frequency Generation (MFRQ):**
Counter resets or triggers an event when a match value is reached.
- **One-Shot Mode:**
The timer runs once and stops after reaching the compare or overflow value.
- **Periodic Mode:**
Timer restarts automatically after overflow or match event.


---

### 🔹 Counter Features
- Up-counter and Down-counter support
- One-shot counter operation
- Start / Stop / Reset counter
- Read and write the current counter value
- External clock or event-driven counting
- Software-controlled counter operations
- 
---
### 🔹 Counter Width Selection
- 8-bit mode
- 16-bit mode
- 32-bit mode
Allows trade-off between resolution, range, and performance.

---

### 🔹 Prescaler & Synchronization
- Programmable prescaler selection
- Clock division for fine timing control
- Prescaler enable/disable
- Hardware clock synchronization support
- Runtime prescaler reconfiguration

---
### 🔹 PWM (Pulse Width Modulation)
- PWM mode support
- Hardware waveform generation
- Configurable PWM frequency
- PWM duty cycle control (NEW)
- Dynamic duty-cycle update during runtime
- Enable/disable PWM output
- Suitable for motor control, LED dimming, and power control
---
### 🔹 Compare Operations
- Configurable compare match value
- Compare-based interrupts
- Match-triggered waveform output
- Counter reset or toggle on compare match
- Used for precise timing and signal generation
----
### 🔹 Waveform Output Operations
- Toggle output pin on compare match
- Set or clear output on match
- Hardware-controlled waveform generation
- Match and PWM-based signal output
---
### 🔹 Capture Features (Input Capture)
- Capture channel enable/disable
- Read captured counter value
- Capture on rising or falling edge
- External signal timestamping
- Supported Capture Modes
- Pulse Width Capture (PW) – Single edge measurement
- Period & Pulse Width Capture (PPW / PWP)
- High-precision pulse and frequency measurement
---
### 🔹 Interrupt Support
- Timer overflow interrupt
- Compare match interrupt
- Capture event interrupt
- Interrupt enable/disable control
- Interrupt flag handling
- Optional callback-based interrupt handling
---
## 📂 File Structure

- `timer_counter_drv.h`        → Public API definitions
- `timer_counter_drv.c`        → Timer / Counter driver implementation
