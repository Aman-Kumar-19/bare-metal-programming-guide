# GPIO Driver – PIC32CX

This module implements a **bare-metal GPIO driver** for the PIC32CX1025SG61128 MCU using **direct register access**.

The driver provides a clean API for configuring pin direction, controlling output state, and reading input levels via the PORT peripheral.

---

## 📌 MCU & Peripheral
- **MCU:** PIC32CX1025SG61128
- **Peripheral:** PORT (GPIO controller)

---

## 📂 Files
- `gpio.c` – GPIO driver implementation
- `gpio.h` – Public GPIO API

---

## ⚙️ Features
- Configure GPIO pin as input or output
- Atomic pin set / clear / toggle operations
- Read real-time input pin state
- Register-level implementation (no HAL dependency)

---

## 🧩 API Overview

```c
void gpio_configure_pin(gpio_port_id_t port,uint8_t pin,gpio_direction_t direction);
void gpio_write_high(gpio_port_id_t port, uint8_t pin);
void gpio_write_low(gpio_port_id_t port, uint8_t pin);
void gpio_write_toggle(gpio_port_id_t port, uint8_t pin);
bool gpio_read_pin(gpio_port_id_t port, uint8_t pin);
