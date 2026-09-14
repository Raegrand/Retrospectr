# Gallery

This folder contains the images and corresponding text displayed by Retrospectr.

## 📁 File Format

Each gallery item consists of **two files** with the same number:

```text
gallery/
├── 1.bin
├── 1.txt
├── 2.bin
├── 2.txt
├── 3.bin
├── 3.txt
└── ...
```

* **`.bin`** — Image file to be displayed on the TFT.
* **`.txt`** — Text associated with the image.
* Files must use **matching numbers** so the image and text are paired correctly.

### Example

```text
1.bin  →  Image 1
1.txt  →  Text for Image 1

2.bin  →  Image 2
2.txt  →  Text for Image 2

3.bin  →  Image 3
3.txt  →  Text for Image 3
```

## ⚠️ Naming Rules

* Use numbers starting from **`1`**.
* The `.bin` and `.txt` files must have the **same number**.
* Do not use spaces or additional characters in the filenames.
* Keep the numbering sequential.

**Valid:**

```text
1.bin
1.txt
2.bin
2.txt
3.bin
3.txt
```

**Invalid:**

```text
image1.bin
1_image.bin
1.txt
2.bin
3.txt
```
