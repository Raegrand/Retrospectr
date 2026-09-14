import struct
import argparse
from PIL import Image

def convert_png_to_rgb565_bin(input_path, output_path, swap_endian=False):
    # Open image and ensure it has 3 color channels (RGB)
    img = Image.open(input_path).convert('RGB')
    pixels = img.load()
    width, height = img.size

    with open(output_path, 'wb') as f:
        for y in range(height):
            for x in range(width):
                r, g, b = pixels[x, y]
                
                # Mask and shift 8-bit channels into 565 format: R(5), G(6), B(5)
                rgb565 = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)
                
                # Pack as 16-bit unsigned short
                # '<H' is little-endian (standard), '>H' is big-endian (common for SPI displays)
                endian_format = '>H' if swap_endian else '<H'
                f.write(struct.pack(endian_format, rgb565))

    print(f"Success: Converted {input_path} ({width}x{height}) to {output_path}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Convert PNG to raw RGB565 binary.")
    parser.add_argument("input", help="Input PNG file path")
    parser.add_argument("output", help="Output BIN file path")
    parser.add_argument("--swap", action="store_true", help="Use big-endian byte order (often needed for SPI TFT displays)")
    
    args = parser.parse_args()
    convert_png_to_rgb565_bin(args.input, args.output, args.swap)