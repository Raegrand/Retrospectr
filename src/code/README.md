# Image & Data Processing Tools

This folder contains Python utilities used to prepare images and Bible text for the Retrospectr ESP32-C3 system.

## 📦 Requirements

Install the required Python package:

```bash
pip install Pillow
```

The scripts use Python's standard libraries along with **Pillow (PIL)** for image processing.

---

## 🛠️ Scripts

### `filter.py`

Applies a vintage/sepia color effect to images contained in a ZIP file and produces a new ZIP file.

The filter:

* Converts images to RGB.
* Applies a sepia/vintage color transformation.
* Blends the effect with the original image.
* Slightly reduces contrast.
* Preserves the original DPI when available.
* Supports `.png`, `.jpg`, and `.jpeg` images.
* Removes macOS hidden files from the output.

**Usage:**

```bash
python filter.py input.zip output.zip
```

Example:

```bash
python filter.py original_images.zip filtered_images.zip
```

---

### `png2bin.py`

Converts a single PNG image into a raw **RGB565 `.bin` file** for use with the TFT display.

The image is converted to RGB565 using:

* 5 bits for Red
* 6 bits for Green
* 5 bits for Blue

The script supports both little-endian and big-endian output.

**Usage:**

```bash
python png2bin.py input.png output.bin
```

For big-endian output:

```bash
python png2bin.py input.png output.bin --swap
```

---

### `zip2bin.py`

Batch-converts all PNG images inside a ZIP file into RGB565 `.bin` files.

The original filename is retained, with the `.png` extension replaced by `.bin`.

**Usage:**

```bash
python zip2bin.py images.zip
```

Specify an output directory:

```bash
python zip2bin.py images.zip -o output
```

For big-endian output:

```bash
python zip2bin.py images.zip -o output --swap
```

---

### `slice.py`

Processes an Indonesian TB Bible text file and separates it into individual chapter files.

The script:

* Reads `indo_tb (1).txt`.
* Detects the Bible book, chapter, and verse using a regular expression.
* Creates a folder for each Bible book.
* Creates a separate `.txt` file for each chapter.
* Formats each verse as:

```text
1. Verse text
2. Verse text
3. Verse text
```

The resulting structure is suitable for copying to the Retrospectr SD card.
**Usage:**

Place the source Bible file in the same directory as `slice.py`, then run:

```bash
python slice.py
```

The generated structure will look similar to:

```text
Kejadian/
├── 1.txt
├── 2.txt
├── 3.txt
└── ...

Keluaran/
├── 1.txt
├── 2.txt
└── ...
```

---

## 🔄 Recommended Image Workflow

For preparing gallery images for Retrospectr:

```text
Original Images
      │
      ▼
  filter.py
      │
      ▼
Filtered PNG Images
      │
      ▼
  zip2bin.py
      │
      ▼
RGB565 .bin Files
      │
      ▼
   SD Card
      │
      ▼
  Retrospectr
```

For a single image, `png2bin.py` can be used instead of `zip2bin.py`.

## 📋 Quick Reference

| Script       | Input         | Output                    | Purpose                        |
| ------------ | ------------- | ------------------------- | ------------------------------ |
| `filter.py`  | ZIP of images | ZIP of filtered PNGs      | Apply vintage color effect     |
| `png2bin.py` | PNG           | RGB565 `.bin`             | Convert one image              |
| `zip2bin.py` | ZIP of PNGs   | `.bin` files              | Batch image conversion         |
| `slice.py`   | Bible `.txt`  | Book/chapter `.txt` files | Prepare Bible text for SD card |

## ⚠️ Notes

* `png2bin.py` and `zip2bin.py` generate raw RGB565 binary data.
* Use `--swap` if the target TFT configuration requires big-endian RGB565 data.
* `slice.py` expects the input file to be named **`indo_tb (1).txt`** and located in the same directory as the script.
* Make sure the generated files match the format expected by the firmware before copying them to the SD card.
