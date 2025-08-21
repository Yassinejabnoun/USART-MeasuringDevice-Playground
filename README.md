# USART Measuring Device Playground 🔌📟

This project turns an STM32 board into a small **voltage measuring device** with **USART (serial) control**.  
It supports single/continuous sampling via the ADC, multiple LED display modes (Binary, BCD, Bar),  
and optional **PC output** over the serial port.

Originally inspired by a lab exercise, extended and documented here as a standalone project.

---

## 🎯 Features

- **Measuring modes**
  - **Single shot**: read current ADC value once and display it.
  - **Continuous**: sample every 100 ms and update the display.
  - **Hold/Resume/Stop** controls for the continuous mode. 
- **Display modes (LEDs)**
  - **Binary**: LEDs represent the value as a binary number.
  - **BCD**: LEDs 7–4 = tens, LEDs 3–0 = ones (e.g., 42 → `0100 0010`).
  - **Bar**: left-to-right bar graph (9 levels: 0…8 LEDs). 
- **PC output mode**: send the measured value via serial as a left-aligned float with unit “V”. 
- **USART control**: operate the device from the keyboard; LEDs and PC messages reflect state changes. 

---

## 🛠 Hardware Setup

- STM32 Nucleo board (e.g., STM32F4xx)
- Breadboard with:
  - 8 LEDs + resistors (status/visualization)
  - 8 push buttons (control panel)
  - Potentiometer or analog source (ADC input)
- USB Logic Analyzer (optional, e.g., PulseView) to verify USART and timing. 

---

## 💻 Software Setup

- [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html)
- Mbed OS style project structure
- **PuTTY** (or any serial terminal) for PC interaction  
  - Install from putty.org and choose **Connection Type: Serial**.  
  - Select the **STLink Virtual COM Port** from Windows Device Manager (e.g., `COM4`).  
  - Set baud rate to **115200** (8N1). 

---

## ⌨️ Controls

### Buttons (on the breadboard)
- Mode: **Binary (5)**, **BCD (6)**, **Bar (7)**, **PC (4)**
- Measurement: **Single (3)**, **Continuous (2)**, **Hold/Resume (1)**, **Stop (0)** 

### Serial keyboard shortcuts (same functions via USART)
| Key | Function / Message |
|---|---|
| `i` | Display mode: Binary |
| `c` | Display mode: BCD |
| `b` | Display mode: Bar |
| `p` | Display mode: PC |
| `e` | Single measurement started |
| `f` | Continuous measurement started |
| `h` | Continuous paused / resumed |
| `s` | Continuous stopped |
| (on start) | `MIT-Digitalvoltmeter : SW-Rev. 0.1` |
| other | Unknown command | 

---

## 📂 Project Structure
TO DO

---

## 📸 Showcase

<img width="782" height="226" alt="image" src="https://github.com/user-attachments/assets/4e5addc0-07d5-47d7-bbb8-18ba1d19b515" />

---

## 🚀 Getting Started

1. Clone the repository
   ```bash
   git clone https://github.com/Yassinejabnoun/USART-MeasuringDevice-Playground.git
   cd USART-MeasuringDevice-Playground
2. Import into STM32CubeIDE.
3. Wire the hardware as in docs/circuit.png (LEDs, keys, ADC input).
4. Build & flash. Open PuTTY @ 115200 baud on the STLink VCOM.
5. Use buttons or serial keys to switch measurement/display modes and observe updates.

---

## 🎥 Demo

(Add your YouTube link here – thumbnail will be generated automatically)

---

## 📜 License

MIT License – free to use for personal or professional projects.

