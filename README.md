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
│   └── Python tools for image and Bible text processing
│
├── main/
│   └── Main firmware and ESP32-C3 configuration
│
├── gallery/
│   └── Gallery images (.bin) and associated text (.txt)
│
└── docs/
    └── Project documentation and SD card information
```

### Directory Details

* **`3DDesign/`** — Contains the mechanical CAD files and 3D-printable enclosure designs.
* **`src/`** — Contains Python utilities for image processing, image-to-RGB565 conversion, batch conversion, and Bible text processing.
* **`main/`** — Contains the main firmware and hardware configuration used by the ESP32-C3.
* **`gallery/`** — Contains the image and text files used by the Retrospectr gallery.
* **`docs/`** — Contains additional project documentation and SD card information.

---

## 🚀 Getting Started

### 1. Clone the Repository

```bash
git clone https://github.com/your-username/Retrospectr.git
cd Retrospectr
```

### 2. Prepare the Enclosure

Navigate to the [`3DDesign/`](./3DDesign/) directory and select the appropriate CAD files for 3D printing.

The enclosure is a modified remix of the **CYD Retro Mini TV by DynaMight1124**.

### 3. Prepare the Gallery

Gallery content consists of matching numbered `.bin` and `.txt` files:

```text
gallery/
├── 1.bin
├── 1.txt
├── 2.bin
├── 2.txt
└── ...
```

The `.bin` file contains the RGB565 image data, while the `.txt` file contains the corresponding text.

Refer to [`gallery/README.md`](./gallery/README.md) for the file naming requirements.

### 4. Prepare the Firmware

The main firmware is located in [`main/`](./main/).

Refer to [`main/README.md`](./main/README.md) for the firmware configuration and hardware wiring.

### 5. Prepare Additional Data

Refer to [`src/README.md`](./src/README.md) for the Python utilities used to prepare gallery images and Bible text for the SD card.

---

## 🔧 Project Features

Retrospectr combines a compact custom enclosure with an ESP32-C3-based embedded system, providing:

* 📺 Compact ST7789 TFT display
* 🎛️ Rotary encoder navigation
* 🔘 Physical push-button input
* 💾 MicroSD-based storage
* 🔋 Portable LiPo battery power
* 🖼️ Image gallery
* 📖 Bible text support
* 🖨️ 3D-printable custom enclosure
* 🧩 Open-source hardware and software

---

## 📄 License

### Firmware & Code

The firmware and software are **open-source**. You are free to fork, modify, and build upon the software.

### 3D Design Files

The 3D enclosure design is licensed under the **Creative Commons Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)** license.

The enclosure is a modification of the **CYD Retro Mini TV by DynaMight1124**. You are free to share and remix the design for **non-commercial purposes**, provided that appropriate attribution is given to the original creator.

---

## 🙏 Credits

* **Seeed Studio** — XIAO ESP32-C3
* **DynaMight1124** — Original CYD Retro Mini TV enclosure design
* **Retrospectr** — Hardware modifications, firmware, and project development
