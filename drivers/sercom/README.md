# What is SERCOM?
SERCOM (Serial Communication Interface) is a flexible serial peripheral used in Microchip SAM / PIC32CX microcontrollers.
Each SERCOM module can be configured as:
USART (UART)
SPI
I²C
👉 **In this project, SERCOM7 is configured as USART.**
---
# What is USART?
USART (Universal Synchronous / Asynchronous Receiver Transmitter) is a hardware module used for serial communication.
USART supports:
- Asynchronous mode → UART (most common)
- Synchronous mode → clocked serial communication
Typical USART features:
- TX (Transmit)
- RX (Receive)
- Configurable baud rate
- Interrupt & DMA support
- Full-duplex communication
---
# How USART Works 
##🟢 Transmitting Data
- CPU writes a byte to the DATA register
- USART shifts the data bit-by-bit
- Data is sent on TX pin
- Start bit → Data bits → Stop bit
## 🔵 Receiving Data
- USART detects start bit on RX pin
- Bits are sampled based on baud rate
- Complete byte stored in DATA register
- RX Complete flag is set
  ---
**SERCOM Instance** : SERCOM7
**Mode**            : USART (Asynchronous)
**Peripheral MUX**  : Function C
---
# Clocking of SERCOM7 USART
SERCOM requires two clocks:
## 1. Core Clock
- Source: GCLK0 (48 MHz)
- Used for baud rate generation and logic

## 2. Slow Clock
- Shared among all SERCOM modules
- Used for internal synchronization
Both clocks must be enabled before using USART.
---
## Baud Rate Calculation
USART is configured in Asynchronous Arithmetic Mode.
**Formula:**
``
BAUD = 65536 × (1 − (16 × Baudrate / fref))
``
---
# How to Implement SERCOM7 USART (Step-by-Step)
Step 1: **Enable Clocks**
- Enable APBD bus clock
- Route GCLK0 to SERCOM7
- Enable SERCOM slow clock
Step 2: **Configure Pins**
- Enable PMUX
- Assign PC12 → TX
- Assign PC13 → RX
Step 3: **Software Reset**
- Reset SERCOM to ensure clean state
Step 4: **Configure USART Mode**
- Internal clock
- 8-bit data
- No parity
- 1 stop bit
- LSB first
Step 5: **Set Baud Rate**
- Calculate baud value
- Write to BAUD register
Step 6: **Enable USART**
- Enable transmitter
- Enable receiver
----
## 🔧 Current Driver Features
-  Blocking TX & RX
- Register-level implementation
- No Harmony / ASF dependency
- Lightweight & bare-metal
- Easy to extend to interrupts or DMA
---
## 🚀 Future Improvements (Planned)
- Interrupt-based RX/TX
- Ring buffer support
- DMA support
- SERCOM-generic driver (SERCOMx)
- Power-saving sleep support
----
# Learning Outcome
This project demonstrates:
- SERCOM architecture understanding
- Clock tree configuration
- Pin multiplexing
- Register-level USART programming
- Embedded firmware design principles
---
