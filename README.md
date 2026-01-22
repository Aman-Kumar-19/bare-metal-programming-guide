# Bare-Metal Programming 🧠⚙️

A practical, step-by-step guide to **bare-metal firmware development**, written from real industry experience and implemented on **independent, NDA-safe hardware setups**.

This repository focuses on **how firmware actually works under the hood** — not just HAL usage.

---

## 🎯 Purpose of This Repository

- Build **strong bare-metal fundamentals**
- Explain *why* things work, not just *how*
- Create a reusable reference for:
  - MCU bring-up
  - Debugging
  - Register-level programming
- Share clean engineering workflows used in industry



---

## 🧱 Target Audience

- Embedded / firmware engineers
- ECE students transitioning to firmware roles
- Engineers preparing for **low-level embedded interviews**
- Anyone tired of copy-paste HAL code 😄

---

## 🧩 Hardware Platforms (Planned)

- PIC32CX (primary reference)
- STM32 (where concepts overlap)
- Generic Cortex-M concepts where applicable

> Concepts are MCU-agnostic unless explicitly stated.

---

## 📚 Planned Content Roadmap

### 1️⃣ MCU Boot & Startup
- Reset vector flow
- Startup code responsibilities
- Memory initialization (BSS / DATA)
- Role of linker script

### 2️⃣ Clock System
- Clock tree overview
- Internal vs external clocks
- PLL concepts
- Common clock misconfiguration bugs

### 3️⃣ Linker Script Fundamentals
- FLASH vs RAM layout
- Stack & heap placement
- Sections explained
- How linker scripts break firmware (and how to debug)

### 4️⃣ Register-Level Programming
- Reading datasheets effectively
- Bit masks, shifts, and ownership
- Safe register write patterns

### 5️⃣ Interrupts & NVIC
- Interrupt flow
- Vector tables
- Priority configuration
- Debugging interrupt issues

### 6️⃣ ADC Deep Dive
- Polling vs DMA
- Trigger sources
- Timing & accuracy pitfalls
- Debugging real ADC issues

### 7️⃣ Debugging Techniques
- Startup hangs
- Clock-related failures
- Peripheral not responding
- DMA & interrupt traps

### 8️⃣ Clean Firmware Architecture
- Folder structure
- Driver layering
- Naming conventions
- Scalable project layout

---

## 🛠️ How This Repo Is Structured

```text
bare-metal-programming-guide/
├── docs/                  # Theory, explanations, diagrams
│   ├── startup.md
│   ├── clock-system.md
│   ├── linker-script.md
│   └── interrupts.md
│
├── drivers/               # Register-level peripheral drivers
│   ├── gpio/
│   │   ├── gpio_drv.c         # PIC32CX GPIO driver implementation
│   │   └── gpio_drv.h         # GPIO driver public API
│   │
│   └── timer_counter/
│       ├── timer_counter_drv.c        # Timer/Counter driver implementation
│       └── timer_counter_drv.h        # Timer/Counter driver public API
│
├── examples/              # Minimal usage examples
│   └── gpio_blink/
│       └── main.c
│
├── notes/                 # Debugging notes & lessons learned
│   └── gpio-debugging.md
│
├── tools/                 # Helper scripts, diagrams, utilities
│
└── README.md

```
---

## 📂 Examples

This repository contains **minimal, focused examples** that demonstrate  
bare-metal firmware concepts using **clean, reusable drivers**.

The goal of these examples is to show **how application code should interact
with low-level drivers**, without exposing register-level details at the
application layer.

### 🔹 GPIO Blink Example

**Location:**  
`examples/gpio_blink/main.c`

**Demonstrates:**
- GPIO pin configuration using a driver abstraction
- Output control via driver APIs instead of direct register access
- Clear separation between **application layer** and **driver layer**
- Simple blocking delay for timing validation

**APIs used:**
- `gpio_configure_pin()`
- `gpio_write_high()`
- `gpio_write_low()`

> This example serves as a reference for writing **clean, maintainable
bare-metal applications** using reusable peripheral drivers.


