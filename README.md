# Sonorus | Arduino Based microcontroller MP3 player

![Status](https://img.shields.io/badge/Status-Phase_1:_Hardware_Logic-blue)
![Platform](https://img.shields.io/badge/Platform-Arduino_Uno_Mini-00979D)

**Sonorus** is the project to get us familiarized with designing firmware and hardware for a custom device. This experience will be used to aid us in our pursuit to make our own gaming mouse.

---

## Features
SD card storage
Volume Potentiometer
3 button input for media selection
Battery life (unresolved due to voltage differences in design, will have to add a voltage regulator and maybe a new battery)

---

## 🛠 Hardware Stack (Version 1.0)
| Component | Description |
| :--- | :--- |
| **Microcontroller** | Metro Mini (ATmega328P) |
| **Display** | 0.96" I2C OLED (SSD1306) |
| **Audio** | DAC came included with the SD reader|
| **Storage** |Adafruit Micro SD Reader (SPI) |
| **Input** | 3x Tactile Buttons + 1x 10k Potentiometer |

---

##  Dependencies
To compile the current code, the following libraries are required:

* **Adafruit_SSD1306** & **Adafruit_GFX** (OLED Display)
* **SD.h** (Standard SD library)
* **SPI.h** & **Wire.h** (Communication protocols)

---

##  File Structure
* `/src` - The main `.ino` sketch and logic.
* `/docs` - Wiring diagrams and hardware schematics.
* `/assets` - Photos of the evolving outer design.

---

##  Setup & Installation
1. Clone this repository: `git clone https://github.com/asw1um/Sonorus`
2. Install the required libraries via the Arduino Library Manager.
3. Connect the hardware as per the schematics in `/docs`.
4. Flash the `with_oled.ino` to your Arduino Uno Mini.

---
*This is my first project focused on learning embedded systems—feedback on code efficiency and memory management is always welcome!*
