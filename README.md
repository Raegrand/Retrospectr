Retrospectr
An open-source hardware project powered by the Seeed Studio XIAO ESP32-C3, featuring a compact display, interactive controls, and portable battery support. The 3D-printable enclosure is a modified remix based on the CYD Retro Mini TV by DynaMight1124.

🛠️ Hardware Specifications
Microcontroller: Seeed Studio XIAO ESP32-C3 (RISC-V 32-bit single-core microprocessor with Wi-Fi and Bluetooth 5)

Display: 1.69-inch TFT Display (Driver: ST7789, Resolution: 240×280)

Input Controls:

- ALPS Rotary Encoder Button

- Push Button (6x6x10mm)

Storage: Micro SD Card Reader with 16 GB SDHC card support

Power Management: 1S 3.7V 1,200mAh LiPo Battery

📁 Repository Structure
/3DDesign/ - Contains mechanical CAD files (including Fusion 360 models like Casing design.f3z / .f3d) for 3D printing the enclosure.

/src/ - Source code for python tools to process image and bible verses and test code for the components

/main/ - Main program uploaded to the ESP32C3

/docs/ - File used in project including sd card format used in the project

🚀 Getting Started
Clone the repository:

Bash
git clone https://github.com/your-username/Retrospectr.git
Hardware Assembly & 3D Printing:

Head over to the 3DDesign/ folder to check out the casing files for printing your own enclosure.

Wire the components according to the specifications (XIAO ESP32-C3 connected to the ST7789 display, ALPS rotary encoder, push button, and micro SD card reader).



📄 License
Firmware & Code: Open-source. Feel free to fork, modify, and build upon the software.

3D Design Files: Licensed under the Creative Commons Attribution-NonCommercial (CC BY-NC 4.0) license. This design is a modification of the CYD Retro Mini TV by DynaMight1124. You are free to share and remix the enclosure for non-commercial purposes with proper attribution.