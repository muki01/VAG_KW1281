# 🚗 VAG_KW1281

![GitHub forks](https://img.shields.io/github/forks/muki01/VAG_KW1281?style=flat)
![GitHub Repo stars](https://img.shields.io/github/stars/muki01/VAG_KW1281?style=flat)
![GitHub Issues or Pull Requests](https://img.shields.io/github/issues/muki01/VAG_KW1281?style=flat)
![GitHub License](https://img.shields.io/github/license/muki01/VAG_KW1281?style=flat)
![GitHub last commit](https://img.shields.io/github/last-commit/muki01/VAG_KW1281)

## 📝 Overview

VAG_KW1281 is an open-source project designed to enable Arduino, ESP32, and other microcontroller platforms to communicate with **Volkswagen Group (VW, Audi, Škoda, SEAT) ECUs** using the **KW1281 diagnostic protocol**.  
This protocol, widely used in VAG vehicles from the 1990s to early 2000s, operates over the **K-line (ISO 9141-based)** physical layer, allowing access to diagnostic data, fault codes, and ECU information.

This project focuses on:
- ⚡ Enabling **low-level communication** with VAG ECUs.
- 🛠 Providing a **hardware-friendly solution** for Arduino, ESP32, and other microcontroller platforms.
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
- 🖥 Support for Arduino, ESP32, and other microcontroller platforms.
- 📖 Example sketches for sending diagnostic requests and reading responses.
- ⚙ Lightweight, hardware-oriented design suitable for microcontroller environments.
- 🌐 Open-source and easily extendable for research, testing, and educational purposes.

---

## 🛠 Hardware Requirements

- Arduino board (Uno, Mega, Nano, etc.) or ESP32 or different microcontroller.  
- K-line interface module or circuit capable of handling the 12V automotive K-line signal.  
- Standard USB cable for programming and debugging.  
- Optional: Level shifter or resistor network depending on ECU and board.

---

## 🔌 Example Circuit
> ⚠️ Make sure to use proper voltage conversion and protection for the K-line to avoid damaging your microcontroller or ECU.

#### This is the schematic with Transistors
<img src="https://github.com/muki01/OBD2_K-line_Reader/blob/main/Schematics/Transistor%20Schematic.png" width=70%>

The **R6** resistor in this schematic is designed for **3.3V** microcontrollers. If you are using a **5V** microcontroller, you need to change the **R6** value to **5.3kΩ**.

Additionally, I have observed that many test devices use a **1kΩ** value for **R4**. However, according to the K-Line documentation, the recommended value for **R4** is **510Ω**. It is advisable to follow this value. That being said, using **1kΩ** for **R4** will not cause any issues in the circuit. However, if you prefer to adhere to the documentation, **510Ω** is the recommended value.

#### This is the schematic with L9637D
<img src="https://github.com/muki01/OBD2_K-line_Reader/blob/main/Schematics/L9637D.png" width=70%>

---

## 🚀 Getting Started

1. Clone the repository:
2. Open the provided example sketch in Arduino IDE.
3. Connect the hardware according to the example circuit.
4. Upload the sketch to your board.
5. Open the Serial Monitor (115200 baud recommended) to view ECU responses.

---

## 📸 Project Photos
 - Here is the photo where I directly connect my custom-built device to the ECU of the Audi A6 2.5L 2001 using wired connections to read real-time data. The ECU model used in this setup is the Bosch EDC15VM+.

<img src="https://github.com/user-attachments/assets/491eb042-311d-4672-b1ea-3c548b4ee2d6" width=50%>

</br>
</br>

 - If you are wondering how I determined which wire should be connected to which pin, you can simply search the internet for your ECU model or use software tools designed for pinout identification. As an example, here is the pinout for my ECU, which I obtained online.

<img src="https://github.com/user-attachments/assets/5377c988-da81-401c-8701-5b0ce9f9cfd8" width=70%>

---

## ☕ Support My Work

If you enjoy my projects and want to support me, you can do so through the links below:

[![Buy Me A Coffee](https://img.shields.io/badge/-Buy%20Me%20a%20Coffee-FFDD00?style=for-the-badge&logo=buy-me-a-coffee&logoColor=black)](https://www.buymeacoffee.com/muki01)
[![PayPal](https://img.shields.io/badge/-PayPal-00457C?style=for-the-badge&logo=paypal&logoColor=white)](https://www.paypal.com/donate/?hosted_button_id=SAAH5GHAH6T72)
[![GitHub Sponsors](https://img.shields.io/badge/-Sponsor%20Me%20on%20GitHub-181717?style=for-the-badge&logo=github)](https://github.com/sponsors/muki01)

---

## 📬 Contact

For information, job offers, collaboration, sponsorship, or purchasing my devices, you can contact me via email.

📧 Email: muksin.muksin04@gmail.com

---
