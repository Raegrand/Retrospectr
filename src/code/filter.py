import zipfile
import os
from PIL import Image, ImageEnhance
from io import BytesIO

def apply_vintage_color(img):
    # Ensure image is in standard RGB format
    img_rgb = img.convert('RGB')
    
    # Standard vintage/sepia color matrix
    sepia_matrix = (
        0.393, 0.769, 0.189, 0,
        0.349, 0.686, 0.168, 0,
        0.272, 0.534, 0.131, 0
    )
    
    # Apply the color transformation matrix
    sepia_img = img_rgb.convert("RGB", sepia_matrix)
    
    # Blend 60% of the vintage tone with 40% of the original color 
    blended_img = Image.blend(img_rgb, sepia_img, alpha=0.6)
    
    # Slightly reduce contrast to mimic older, washed-out analog displays
    return ImageEnhance.Contrast(blended_img).enhance(0.9)

def process_zip_to_zip(input_zip, output_zip):
    print(f"Reading {input_zip}...")
    try:
        with zipfile.ZipFile(input_zip, 'r') as zin, zipfile.ZipFile(output_zip, 'w', compression=zipfile.ZIP_DEFLATED) as zout:
            # Exclude macOS hidden files and directories
            valid_files = [f for f in zin.infolist() 
                           if not f.is_dir() 
                           and not f.filename.startswith('__MACOSX')
                           and not f.filename.startswith('.')
                           and f.filename.lower().endswith(('.png', '.jpg', '.jpeg'))]
            
            if not valid_files:
                print("Error: No valid images found in the input ZIP.")
                return

            processed_count = 0
            for i, file_info in enumerate(valid_files):
                print(f"[{i+1}/{len(valid_files)}] Processing Color: {file_info.filename}...")
                try:
                    with zin.open(file_info) as file:
                        img = Image.open(file)
                        
                        # Apply ONLY the color filter
                        color_effect_img = apply_vintage_color(img)
                        
                        # Extract DPI to retain original print resolution if it exists
                        save_kwargs = {'format': 'PNG'}
                        if 'dpi' in img.info:
                            save_kwargs['dpi'] = img.info['dpi']
                        
                        img_byte_arr = BytesIO()
                        color_effect_img.save(img_byte_arr, **save_kwargs)
                        img_bytes = img_byte_arr.getvalue()
                        
                        # Strip subfolders, output the image directly into the zip
                        base_name = os.path.splitext(os.path.basename(file_info.filename))[0]
                        new_filename = f"{base_name}.png"
                        zout.writestr(new_filename, img_bytes)
                        processed_count += 1
                except Exception as e:
                    print(f"  -> Failed to process {file_info.filename}: {e}")
                    
        print(f"Batch conversion complete: {processed_count}/{len(valid_files)} saved to {output_zip}")
        
    except zipfile.BadZipFile:
        print(f"Error: {input_zip} is not a valid ZIP file.")
    except Exception as e:
        print(f"A fatal error occurred: {e}")

if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description="Batch apply vintage color to images in a ZIP at original resolution.")
    parser.add_argument("zip_file", help="Input ZIP file containing images")
    parser.add_argument("out_zip", help="Output ZIP file name")
    
    args = parser.parse_args()
    process_zip_to_zip(args.zip_file, args.out_zip)