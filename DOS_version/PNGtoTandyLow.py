
import sys
from PIL import Image
import numpy as np

def create_custom_palette():
    return [
        0, 0, 0,
        14, 0, 169,
        0, 171, 5,
        0, 171, 171,
        170, 0, 0,
        171, 1, 169,
        172, 84, 0,
        169, 171, 168,
        84, 86, 83,
        83, 84, 255,
        85, 255, 82,
        79, 255, 255,
        255, 84, 83,
        255, 81, 255,
        255, 254, 80,
        255, 255, 255
    ]

def apply_custom_palette(img):
    palette = create_custom_palette()
    img_with_palette = Image.new("P", img.size)
    img_with_palette.putpalette(palette)
    return img.quantize(palette=img_with_palette)

def convert_image_to_tandy_raw(input_file):
    # Load the input image
    img = Image.open(input_file).convert("RGB")

    # Apply the custom palette
    img = apply_custom_palette(img)

    # Get the pixel data
    pixel_data = np.array(img)

    # Create the Tandy 16384-byte interlaced buffer
    tandy_buffer = bytearray(16384)

    # Write the Tandy interlaced pixel data to the buffer
    for y in range(200):
        for x in range(0, 160, 2):
            offset = (y // 2) * 80 + (y % 2) * 8192 + x // 2
            tandy_buffer[offset] = (pixel_data[y, x] << 4) | pixel_data[y, x + 1]

    return tandy_buffer

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python png_to_tandy.py input.png output.tandy")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]

    tandy_buffer = convert_image_to_tandy_raw(input_file)
    with open(output_file, "wb") as f:
        f.write(tandy_buffer)
