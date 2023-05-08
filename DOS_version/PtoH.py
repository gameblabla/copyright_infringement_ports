import sys
from PIL import Image

def plot_pixel(buffers, x, y, color):
    offset = ((y >> 2) * 90) + ((y & 3) * 0x2000) + (x >> 3)
    pixel_mask = 0x80 >> (x & 7)

    for plane in range(4):
        if color & (1 << plane):
            buffers[plane][offset] |= pixel_mask
        else:
            buffers[plane][offset] &= ~pixel_mask

def save_buffers_to_file(buffers, output_filename):
    with open(output_filename, 'wb') as f:
        for buffer in buffers:
            f.write(buffer)

def save_palette_to_file(palette, output_filename):
    with open(output_filename, 'wb') as f:
        for color in palette:
            r, g, b = color
            r = r >> 6
            g = g >> 6
            b = b >> 6
            palette_byte = (r << 4) | (g << 2) | b
            f.write(palette_byte.to_bytes(1, 'little'))

def png_to_hercules_incolor(input_file, output_file, output_palette):
    img = Image.open(input_file).convert("P")
    width, height = img.size

    input_palette = img.getpalette()[:16*3]
    input_palette = [tuple(input_palette[i:i+3]) for i in range(0, len(input_palette), 3)]

    img.putpalette(sum(input_palette, ()))

    buffers = [bytearray(0x8000) for _ in range(4)]

    for y in range(height):
        for x in range(width):
            color = img.getpixel((x, y))
            plot_pixel(buffers, x, y, color)

    save_buffers_to_file(buffers, output_file)
    save_palette_to_file(input_palette, output_palette)

if __name__ == "__main__":
    if len(sys.argv) < 4:
        print("Usage: python png_to_hercules_incolor.py input.png output.buffers output_palette.bin")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]
    output_palette = sys.argv[3]
    png_to_hercules_incolor(input_file, output_file, output_palette)
