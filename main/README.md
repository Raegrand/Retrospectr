# Retrospectr — Main Firmware

This folder contains the main firmware and hardware configuration for the **Retrospectr** ESP32-C3 system.

The firmware is designed for the **Seeed Studio XIAO ESP32-C3** and interfaces with the TFT display, rotary encoder, push button, and microSD card.

---

## 🛠️ Hardware Configuration

### ST7789 TFT Display

| TFT Pin | XIAO ESP32-C3 GPIO | Description    |
| ------- | -----------------: | -------------- |
| CS      |            GPIO 21 | Chip Select    |
| DC      |             GPIO 6 | Data / Command |
| RST     |                  — | Not used       |
| BL      |             GPIO 7 | Backlight      |
| VCC     |               3.3V | Power          |
| GND     |                GND | Ground         |
| SCK     |            SPI SCK | SPI clock      |
| MOSI    |           SPI MOSI | SPI data       |

The display configuration uses a **280 × 240** display size in the firmware.

> **Note:** The SPI SCK and MOSI GPIO numbers are not explicitly defined in the provided `config.h`, so they are not assigned specific GPIO numbers here.

### 💾 MicroSD Card Reader

| SD Pin | XIAO ESP32-C3 GPIO | Description  |
| ------ | -----------------: | ------------ |
| CS     |             GPIO 4 | Chip Select  |
| SCK    |            SPI SCK | SPI clock    |
| MOSI   |           SPI MOSI | SPI data out |
| MISO   |           SPI MISO | SPI data in  |
| VCC    |               3.3V | Power        |
| GND    |                GND | Ground       |

The microSD chip-select pin is configured as **GPIO 4**.

### 🎛️ ALPS Rotary Encoder

| Encoder Pin | XIAO ESP32-C3 GPIO | Description          |
| ----------- | -----------------: | -------------------- |
| CLK         |             GPIO 2 | Rotary encoder clock |
| DT          |             GPIO 3 | Rotary encoder data  |
| SW          |            GPIO 20 | Encoder push button  |
| VCC         |               3.3V | Power                |
| GND         |                GND | Ground               |

The encoder GPIO assignments are defined as CLK = GPIO 2, DT = GPIO 3, and SW = GPIO 20.

### 🔘 Extra Push Button

| Button Pin | XIAO ESP32-C3 GPIO | Description  |
| ---------- | -----------------: | ------------ |
| Signal     |             GPIO 5 | Button input |
| GND        |                GND | Ground       |

The extra push button uses **GPIO 5**.

---

## 📌 GPIO Summary

| Function      |     GPIO |
| ------------- | -------: |
| TFT CS        |       21 |
| TFT DC        |        6 |
| TFT RST       | Not used |
| TFT Backlight |        7 |
| SD CS         |        4 |
| Rotary CLK    |        2 |
| Rotary DT     |        3 |
| Rotary SW     |       20 |
| Extra Button  |        5 |

---

## 🖥️ Display Configuration

| Parameter     | Value      |
| ------------- | ---------- |
| **Driver**    | ST7789     |
| **Width**     | 280 pixels |
| **Height**    | 240 pixels |
| **Backlight** | GPIO 7     |

---

## 💾 SD Card Content

The Retrospectr SD card stores gallery images and their corresponding text files.

Gallery files use matching numerical filenames:

```text
SD Card/
├── 1.bin
├── 1.txt
├── 2.bin
├── 2.txt
├── 3.bin
├── 3.txt
└── ...
```

For more information about gallery file preparation and naming, see [`../gallery/README.md`](../gallery/README.md).

---

## 🔧 Firmware

The firmware in this directory is the main program uploaded to the **XIAO ESP32-C3**.

Before uploading the firmware:

1. Connect the required hardware according to the wiring tables above.
2. Prepare the SD card with the required gallery files.
3. Verify the display and input connections.
4. Build and upload the firmware to the XIAO ESP32-C3.

---

## 📁 Related Directories

| Directory                      | Purpose                                   |
| ------------------------------ | ----------------------------------------- |
| [`../3DDesign/`](../3DDesign/) | Mechanical CAD and enclosure design       |
| [`../src/`](../src/)           | Image and Bible text processing utilities |
| [`../gallery/`](../gallery/)   | Gallery `.bin` and `.txt` files           |
| [`../docs/`](../docs/)         | Project documentation                     |

---

## 📄 License

The firmware and software in this directory are **open-source**. You are free to fork, modify, and build upon the code.
