import sys
from PIL import Image

def plot(x, y, color, cga_buffer):
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

def png_to_cga(png_file, output_cga):
    image = Image.open(png_file).convert('1')
    w, h = image.size
    if w > 640 or h > 200:
        raise ValueError("Image dimensions must be 640x200 or smaller")
    data = list(image.getdata())
    cga_buffer = bytearray(0x4000)

    for y in range(h):
        for x in range(w):
            color = data[x + y * w]
            plot(x, y, color, cga_buffer)

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
