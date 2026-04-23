# Sonorus | Arduino Uno Mini Prototype

![Status](https://img.shields.io/badge/Status-Phase_1:_Hardware_Logic-blue)
![Platform](https://img.shields.io/badge/Platform-Arduino_Uno_Mini-00979D)

**Sonorus** is the foundational stage of a custom peripheral project. Currently configured as a media/interface controller, this build is designed to stabilize the firmware and UI logic required for a high-performance **Gaming Mouse**.

---

## 🚀 The Vision
This project is a stepping stone. By starting with a standalone media controller, I am mastering:
1. **Input Polling:** High-speed response from buttons and a potentiometer.
2. **UI Rendering:** Managing an OLED display without blocking system logic.
3. **Storage/Audio:** Integrating SD card reads and DAC output for future features.

---

## 🛠 Hardware Stack (Version 1.0)
| Component | Description |
| :--- | :--- |
| **Microcontroller** | Arduino Uno Mini (ATmega328P) |
| **Display** | 0.96" I2C OLED (SSD1306) |
| **Audio** | Dedicated External DAC |
| **Storage** | Micro SD Reader (SPI) |
| **Input** | 3x Tactile Buttons + 1x 10k Potentiometer |

---

## 📦 Dependencies
To compile the current code, the following libraries are required:

* **Adafruit_SSD1306** & **Adafruit_GFX** (OLED Display)
* **SD.h** (Standard SD library)
* **SPI.h** & **Wire.h** (Communication protocols)

---

## 🕹 Mouse Development Roadmap
As the code stabilizes and the "outer design" transitions from a breadboard to a shell:

- [x] **Phase 1: Logic.** Master I/O, UI menus, and data handling on Uno Mini.
- [ ] **Phase 2: HID Migration.** Port code to an HID-enabled chip (like ATmega32U4) for native mouse/keyboard recognition.
- [ ] **Phase 3: Ergonomics.** Design and 3D-print a custom gaming mouse shell.
- [ ] **Phase 4: Sensor Integration.** Integrate a high-polling rate optical gaming sensor.

---

## 📂 File Structure
* `/src` - The main `.ino` sketch and logic.
* `/docs` - Wiring diagrams and hardware schematics.
* `/assets` - Photos of the evolving outer design.

---

## 🛠 Setup & Installation
1. Clone this repository: `git clone https://github.com/asw1um/Sonorus`
2. Install the required libraries via the Arduino Library Manager.
3. Connect the hardware as per the schematics in `/docs`.
4. Flash the `Sonorus.ino` to your Arduino Uno Mini.

---
*This is my first project focused on learning embedded systems—feedback on code efficiency and memory management is always welcome!*
