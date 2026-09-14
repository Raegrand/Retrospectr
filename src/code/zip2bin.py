import os
import struct
import argparse
import zipfile
from PIL import Image

def convert_image_to_rgb565(img, output_path, swap_endian=False):
    img = img.convert('RGB')
    pixels = img.load()
    width, height = img.size

    with open(output_path, 'wb') as f:
        for y in range(height):
            for x in range(width):
                r, g, b = pixels[x, y]
                
                # Mask and shift 8-bit channels into 565 format
                rgb565 = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)
                
                # Pack as 16-bit unsigned short
                endian_format = '>H' if swap_endian else '<H'
                f.write(struct.pack(endian_format, rgb565))

def process_zip_file(zip_path, output_dir, swap_endian=False):
    os.makedirs(output_dir, exist_ok=True)
    
    with zipfile.ZipFile(zip_path, 'r') as zf:
        for file_info in zf.infolist():
            # Skip directories and non-PNG files
            if file_info.is_dir() or not file_info.filename.lower().endswith('.png'):
                continue
                
            with zf.open(file_info) as file:
                img = Image.open(file)
                
                # Strip original paths/extensions to create the new .bin name
                base_name = os.path.basename(file_info.filename)
                name_without_ext = os.path.splitext(base_name)[0]
                out_path = os.path.join(output_dir, f"{name_without_ext}.bin")
                
                print(f"Converting {base_name} -> {out_path}...")
                convert_image_to_rgb565(img, out_path, swap_endian)
                
    print("Batch conversion complete.")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Convert PNGs in a ZIP to raw RGB565 binaries.")
    parser.add_argument("zip_file", help="Input ZIP file containing PNGs")
    parser.add_argument("-o", "--outdir", default=".", help="Output directory for .bin files (default: current directory)")
    parser.add_argument("--swap", action="store_true", help="Use big-endian byte order")
    
    args = parser.parse_args()
    process_zip_file(args.zip_file, args.outdir, args.swap)