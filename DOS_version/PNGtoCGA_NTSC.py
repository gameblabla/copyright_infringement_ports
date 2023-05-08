import sys
from PIL import Image
from PIL import ImagePalette
import numpy as np

COLOR_MAPPING = [
	(0,0,0,0),
	(0,1,0,0),
	(1,0,0,0),
	(1,1,0,0),
	(0,0,0,1),
	(0,1,0,1),
	(1,0,0,1),
	(1,1,0,1),
	(0,0,1,0),
	(0,1,1,0),
	(1,0,1,0),
	(1,1,1,0),
	(0,0,1,1),
	(0,1,1,1),
	(1,0,1,1),
	(1,1,1,1),
]

CGA_COMP_PALETTE = [
    0, 0, 0,        # 0000 Black
    166, 0, 94,     # 0100 Dark red
    77, 64, 0,      # 1000 Dark Brown
    255, 68, 0,     # 1100 Orange
    0, 99, 0,       # 1110 Dark Green
    119, 115, 122,  # 1010 Dark Grey
    0, 185, 0,      # 1001 Green
    223, 196, 0,    # 1110 Yellow
    0, 66, 226,     # 0010 Blue
    209, 77, 255,   # 0111 Pink
    119, 115, 121,  # 1010 Light Brown
    255, 133, 240,  # 1110 Light Pink
    0, 159, 253,    # 0011 Cyan
    153, 172, 255,  # 0111 Light blue
    0, 235, 145,    # 1011 Light green
    255, 252, 255,  # 1111 White
]


def plot(x, y, color_index, cga_buffer):
    bw_pixels = COLOR_MAPPING[color_index]
    for i, color in enumerate(bw_pixels):
        plot_single(x * 4 + i, y, color, cga_buffer)


def plot_single(x, y, color, cga_buffer):
    bank = y & 1
    y = (y & 0xFE) * 40
    if bank:
        y += 0x2000
    y += (x >> 3)
    x = x & 0x7
    color = (color & 0x1) << 7

    mask = 0x7F
    while x:
        mask = (mask >> 1) | 0x80
        color = color >> 1
        x -= 1

    offset = y
    c = cga_buffer[offset]
    c = (c & mask) | color
    cga_buffer[offset] = c



def find_closest_palette_color(color, palette):
    palette = np.asarray(palette).reshape(-1, 3)
    color = np.asarray(color)
    dists = np.linalg.norm(palette - color, axis=1)
    return np.argmin(dists)

def png_to_cga(png_file, output_cga):
    image = Image.open(png_file).convert('RGB')
    w, h = image.size
    if w != 160 or h != 200:
        raise ValueError("Image dimensions must be 160x200")

    # Check if the image has more than 16 colors
    unique_colors = image.getcolors(w * h)
    if len(unique_colors) > 16:
        raise ValueError("Image must have no more than 16 colors")

    data = list(image.getdata())

    # Reshape the CGA_COMP_PALETTE into a list of color tuples
    cga_palette = [(CGA_COMP_PALETTE[i], CGA_COMP_PALETTE[i + 1], CGA_COMP_PALETTE[i + 2]) for i in range(0, len(CGA_COMP_PALETTE), 3)]

    # Create a mapping from input image colors to color indices in cga_palette
    color_to_index = {color: cga_palette.index(color) for _, color in unique_colors}

    cga_buffer = bytearray(16384)

    for y in range(h):
        for x in range(w):
            color = data[x + y * w]
            color_index = color_to_index[color]
            plot(x, y, color_index, cga_buffer)

    with open(output_cga, 'wb') as f:
        f.write(cga_buffer)


if __name__ == '__main__':
    import sys

    if len(sys.argv) < 3:
        print("Usage: python png_to_cga.py <input_png> <output_cga>")
        exit(1)

    input_png = sys.argv[1]
    output_cga = sys.argv[2]
    png_to_cga(input_png, output_cga)
    
