import sys
from PIL import Image
import struct
import numpy as np

def rgb_to_rgb333(color):
    r, g, b = color
    # Scale the 8-bit RGB components to 3-bit
    r >>= 5
    g >>= 5
    b >>= 5
    # Shift to correct positions
    r <<= 8
    g <<= 4
    # Combine the components
    return (r & 0x0700) | (g & 0x0070) | (b & 0x0007)



def set320x200(x, y, colour, buffer):
    ptr = (y * 80) + (x // 16) * 4
    pixel = x % 16
    for i in range(4):
        if colour & (1 << i):
            buffer[ptr + i] |= 1 << (15 - pixel)
        else:
            buffer[ptr + i] &= ~(1 << (15 - pixel))

def png_to_pi1(png_path, pi1_path):
    # Load the PNG image
    img = Image.open(png_path).convert('P')
    width, height = img.size
    pixels = list(img.getdata())
    palette = img.getpalette()[:16*3]

    # Translate the color data into the Degas PI1 format
    pi1_palette = [rgb_to_rgb333(palette[i:i+3]) for i in range(0, len(palette), 3)]

    # Translate the pixel data into the Degas PI1 format
    buffer = [0] * (width * height // 4)
    for i, pixel in enumerate(pixels):
        x = i % width
        y = i // width
        set320x200(x, y, pixel, buffer)

    # Write the translated data into a PI1 file
    with open(pi1_path, 'wb') as f:
        f.write(struct.pack('>H', 0))  # screen resolution
        f.write(struct.pack('>16H', *pi1_palette))  # palette
        f.write(struct.pack('>{}H'.format(len(buffer)), *buffer))  # pixel data

# Example usage
if len(sys.argv) != 3:
    print("Usage: python script.py input.png output.pi1")
else:
    png_to_pi1(sys.argv[1], sys.argv[2])
