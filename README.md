# 🚗 VAG_KW1281

![GitHub forks](https://img.shields.io/github/forks/muki01/VAG_KW1281?style=flat)
![GitHub Repo stars](https://img.shields.io/github/stars/muki01/VAG_KW1281?style=flat)
![GitHub Issues or Pull Requests](https://img.shields.io/github/issues/muki01/VAG_KW1281?style=flat)
![GitHub License](https://img.shields.io/github/license/muki01/VAG_KW1281?style=flat)
![GitHub last commit](https://img.shields.io/github/last-commit/muki01/VAG_KW1281)

**Arduino & ESP32 Project for Communicating with Volkswagen Group ECUs via the KW1281 Protocol**

---

## 📝 Overview

VAG_KW1281 is an open-source project designed to enable **Arduino** and **ESP32** microcontrollers to communicate with **Volkswagen Group (VW, Audi, Škoda, SEAT) ECUs** using the **KW1281 diagnostic protocol**.  
This protocol, widely used in VAG vehicles from the 1990s to early 2000s, operates over the **K-line (ISO 9141-based)** physical layer, allowing access to diagnostic data, fault codes, and ECU information.

This project focuses on:
- ⚡ Enabling **low-level communication** with VAG ECUs.
- 🛠 Providing a **hardware-friendly solution** for Arduino and ESP32 platforms.
- 📚 Offering an **educational platform** for understanding KW1281 and automotive diagnostics.

You can also see my other car projects:
1. [Тhis](https://github.com/muki01/I-K_Bus) project is for BMW with I/K bus system. 
2. [Тhis](https://github.com/muki01/OBD2_CAN_Bus_Reader) project is for Cars with CAN Bus.
3. [Тhis](https://github.com/muki01/OBD2_K-line_Reader) project is for Cars with ISO9141 and ISO14230 protocols.
4. [Тhis](https://github.com/muki01/OBD2_CAN_Bus_Library) is my OBD2 CAN Bus Communication Library for Arduino IDE.
5. [Тhis](https://github.com/muki01/OBD2_KLine_Library) is my OBD2 K-Line Communication Library for Arduino IDE.

---

## ✨ Features

- 🔌 Communication with VAG ECUs via K-line using KW1281 protocol.
- 🖥 Support for Arduino and ESP32 boards.
- 📖 Example sketches for sending diagnostic requests and reading responses.
- ⚙ Lightweight, hardware-oriented design suitable for microcontroller environments.
- 🌐 Open-source and easily extendable for research, testing, and educational purposes.

---

## 🛠 Hardware Requirements

- Arduino board (Uno, Mega, Nano, etc.) **or** ESP32.  
- K-line interface module or circuit capable of handling the 12V automotive K-line signal.  
- Standard USB cable for programming and debugging.  
- Optional: Level shifter or resistor network depending on ECU and board.

---

## 🔌 Example Circuit

> ⚠️ Make sure to use proper voltage conversion and protection for the K-line to avoid damaging your microcontroller or ECU.

---

## 🚀 Getting Started

1. Clone the repository:

    ```bash
    git clone https://github.com/yourusername/vag-kw1281.git
    cd vag-kw1281

2. Open the provided example sketch in Arduino IDE.
3. Connect the hardware according to the example circuit.
4. Upload the sketch to your Arduino or ESP32 board.
5. Open the Serial Monitor (115200 baud recommended) to view ECU responses.

---

## ☕ Support My Work

If you enjoy my projects and want to support me, you can do so through the links below:

[![Buy Me A Coffee](https://img.shields.io/badge/-Buy%20Me%20a%20Coffee-FFDD00?style=for-the-badge&logo=buy-me-a-coffee&logoColor=black)](https://www.buymeacoffee.com/muki01)
[![PayPal](https://img.shields.io/badge/-PayPal-00457C?style=for-the-badge&logo=paypal&logoColor=white)](https://www.paypal.com/donate/?hosted_button_id=SAAH5GHAH6T72)
[![GitHub Sponsors](https://img.shields.io/badge/-Sponsor%20Me%20on%20GitHub-181717?style=for-the-badge&logo=github)](https://github.com/sponsors/muki01)

📧 **Contact:** `muksin.muksin04@gmail.com`

---
