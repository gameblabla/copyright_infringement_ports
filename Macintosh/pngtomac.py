import sys
from PIL import Image

def plot(x, y, white, base_addr, row_bytes):
    byte = y * row_bytes + (x >> 3)
    mask = 1 << (7 - (x % 8))

    if white:
        base_addr[byte] &= ~mask
    else:
        base_addr[byte] |= mask


def png_to_cga(png_file, output_cga):
    image = Image.open(png_file).convert('1')
    w, h = image.size

    data = list(image.getdata())
    cga_buffer = bytearray(512 * h // 8)

    for y in range(h):
        for x in range(w):
            color = data[x + y * w]
            plot(x, y, color, cga_buffer, (512 + 7) // 8)

    with open(output_cga, 'wb') as f:
        f.write(cga_buffer)

if __name__ == '__main__':
    import sys

    if len(sys.argv) < 3:
        print("Usage: python png_to_mac.py <input_png> <output_mac>")
        exit(1)

    input_png = sys.argv[1]
    output_cga = sys.argv[2]
    png_to_cga(input_png, output_cga)
