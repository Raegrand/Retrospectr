# Retrospectr — SD Card Structure

This folder contains the files and folder structure required by the Retrospectr firmware.

> ⚠️ **Important:** Do not change the folder names, file names, or file formats. The firmware expects the SD card data to follow the structure described below.

---

## 📁 SD Card Structure

The SD card must contain the following folders:

```text
SD Card/
├── Gallery/
│   ├── 1.bin
│   ├── 1.txt
│   ├── 2.bin
│   ├── 2.txt
│   └── ...
│
└── Bible/
    ├── Kejadian/
    │   ├── 1.txt
    │   ├── 2.txt
    │   └── ...
    │
    ├── Keluaran/
    │   ├── 1.txt
    │   ├── 2.txt
    │   └── ...
    │
    └── ...
```

## 🖼️ Gallery

The `Gallery/` folder contains the images and corresponding text displayed by Retrospectr.

Each item must contain a matching `.bin` image file and `.txt` text file:

```text
Gallery/
├── 1.bin
├── 1.txt
├── 2.bin
├── 2.txt
└── ...
```

### File Requirements

* Image files must use the **`.bin`** format.
* Text files must use the **`.txt`** format.
* Both files must use the **same number**.
* Numbering starts from **`1`**.
* Keep the numbering sequential.
* Do not rename `Gallery/`.

For example:

```text
1.bin  → Image 1
1.txt  → Text for Image 1

2.bin  → Image 2
2.txt  → Text for Image 2
```

See [`../gallery/README.md`](../gallery/README.md) for more information.

---

## 📖 Bible

The `Bible/` folder contains the Indonesian Bible text organized by **book and chapter**.

Each Bible book must have its own folder:

```text
Bible/
├── Kejadian/
├── Keluaran/
├── Imamat/
└── ...
```

Each chapter is stored as a numbered `.txt` file:

```text
Bible/
└── Kejadian/
    ├── 1.txt
    ├── 2.txt
    ├── 3.txt
    └── ...
```

The chapter number must correspond to the chapter.

For example:

```text
Kejadian/1.txt  → Kejadian Chapter 1
Kejadian/2.txt  → Kejadian Chapter 2
Kejadian/3.txt  → Kejadian Chapter 3
```

---

## ⚠️ Do Not Change the Structure

The following must remain unchanged:

| Item                   | Required Format                |
| ---------------------- | ------------------------------ |
| **Gallery folder**     | `Gallery/`                     |
| **Gallery images**     | `1.bin`, `2.bin`, `3.bin`, ... |
| **Gallery text**       | `1.txt`, `2.txt`, `3.txt`, ... |
| **Bible folder**       | `Bible/`                       |
| **Bible book folders** | One folder per book            |
| **Bible chapters**     | `1.txt`, `2.txt`, `3.txt`, ... |
| **Image format**       | RGB565 `.bin`                  |
| **Text format**        | `.txt`                         |

Changing the folder names or expected file formats may cause the firmware to **fail to find or load the corresponding data**.

---

## 📌 SD Card Preparation

Before inserting the SD card into Retrospectr:

1. Format the SD card using the required filesystem.
2. Create the required folders.
3. Copy the gallery files into `Gallery/`.
4. Copy the Bible book folders into `Bible/`.
5. Verify that filenames and folder names exactly match the required structure.
6. Insert the SD card into Retrospectr.

Keep the SD card structure consistent with this README when adding or replacing content.
