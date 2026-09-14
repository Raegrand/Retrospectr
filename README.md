# Retrospectr

**Retrospectr** is an open-source hardware project powered by the **Seeed Studio XIAO ESP32-C3**, featuring a compact TFT display, interactive controls, microSD storage, and portable battery support.

The 3D-printable enclosure is a modified remix based on the **CYD Retro Mini TV by DynaMight1124**.

---

## 🛠️ Hardware Specifications

| Component           | Specification                                                               |
| ------------------- | --------------------------------------------------------------------------- |
| **Microcontroller** | Seeed Studio XIAO ESP32-C3 — RISC-V 32-bit single-core, Wi-Fi & Bluetooth 5 |
| **Display**         | 1.69-inch TFT, ST7789 driver, 240 × 280 resolution                          |
| **Rotary Input**    | ALPS Rotary Encoder with push button                                        |
| **Push Button**     | 6 × 6 × 10 mm tactile push button                                           |
| **Storage**         | MicroSD card reader, supporting 16 GB SDHC                                  |
| **Battery**         | 1S 3.7 V, 1,200 mAh LiPo battery                                            |

---

## 📁 Repository Structure

```text
Retrospectr/
├── 3DDesign/
│   └── Mechanical CAD files and 3D-printable enclosure designs
│
├── src/
│   └── Python tools for image processing, Bible verse processing,
│       and component testing
│
├── main/
│   └── Main program and firmware uploaded to the ESP32-C3
│
└── docs/
    └── Project documentation and SD card formatting information
```

### Directory Details

* **`3DDesign/`** — Contains the mechanical CAD files for the enclosure, including Fusion 360 files such as `Casing design.f3z` and `.f3d` files.
* **`src/`** — Contains Python tools for processing images and Bible verses, along with component test code.
* **`main/`** — Contains the main firmware/program uploaded to the ESP32-C3.
* **`docs/`** — Contains project documentation, including the required SD card format and related files.

---

## 🚀 Getting Started

### 1. Clone the Repository

```bash
git clone https://github.com/your-username/Retrospectr.git
cd Retrospectr
```

### 2. 3D Print the Enclosure

Navigate to the [`3DDesign/`](./3DDesign/) directory and select the appropriate CAD files to create your own enclosure.

The enclosure is a modified remix of the **CYD Retro Mini TV by DynaMight1124**.

### 3. Assemble the Hardware

Connect the components according to the project specifications:

* Seeed Studio XIAO ESP32-C3
* ST7789 1.69-inch TFT display
* ALPS rotary encoder
* Push button
* MicroSD card reader
* 1S 3.7 V 1,200 mAh LiPo battery

Refer to the [`docs/`](./docs/) directory for additional project documentation and configuration details.

---

## 🔧 Project Overview

Retrospectr combines a compact custom enclosure with an ESP32-C3-based embedded system, providing:

* 📺 Compact TFT display
* 🎛️ Rotary encoder navigation
* 🔘 Physical push-button input
* 💾 MicroSD-based storage
* 🔋 Portable LiPo battery power
* 🖨️ 3D-printable custom enclosure
* 🧩 Open-source hardware and software

---

## 📄 License

### Firmware & Code

The firmware and software are **open-source**. You are free to fork, modify, and build upon the software.

### 3D Design Files

The 3D enclosure design is licensed under the **Creative Commons Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)** license.

The enclosure is a modification of the **CYD Retro Mini TV by DynaMight1124**. You are free to share and remix the design for **non-commercial purposes**, provided that appropriate attribution is given to the original creator and this project.

---

## 🙏 Credits

* **Seeed Studio** — XIAO ESP32-C3
* **DynaMight1124** — Original CYD Retro Mini TV enclosure design
* **Retrospectr** — Hardware modifications, firmware, and project development
