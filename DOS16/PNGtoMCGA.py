import sys
import numpy as np
from PIL import Image

def convert_png_to_raw(png_file, raw_file, palette_file):
    img = Image.open(png_file).convert('P', palette=Image.ADAPTIVE, colors=256)

    # Generate the palette for VGA 6-bit DAC
    raw_palette = np.array(img.getpalette()).reshape((-1, 3))
    vga_palette = np.right_shift(raw_palette, 2).astype(np.uint8)

    with open(palette_file, 'wb') as f:
        for color in vga_palette:
            f.write(color.tobytes())

    # Save the 8bpp indexed image data as a raw file
    img_data = np.array(img, dtype=np.uint8)
    with open(raw_file, 'wb') as f:
        img_data.tofile(f)

if __name__ == '__main__':
    if len(sys.argv) != 4:
        print(f'Usage: {sys.argv[0]} [input 8bpp PNG file] [output RAW 8bpp file] [output palette file]')
        sys.exit(1)

    png_file = sys.argv[1]
    raw_file = sys.argv[2]
    palette_file = sys.argv[3]

    convert_png_to_raw(png_file, raw_file, palette_file)
