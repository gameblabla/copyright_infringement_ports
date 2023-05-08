import sys
from PIL import Image

def plot_pixel(buffer, x, y, color):
    offset = ((y >> 2) * 90) + ((y & 3) * 0x2000) + (x >> 3)
    pixel_mask = 0x80 >> (x & 7)

    if color:
        buffer[offset] |= pixel_mask
    else:
        buffer[offset] &= ~pixel_mask

def png_to_hercules(input_file, output_file):
    image = Image.open(input_file).convert('1')
    width, height = image.size
    assert width == 720 and height == 348, "Image dimensions must be 720x348"

    buffer = bytearray(0x8000)  # Initialize a 32KB buffer

    for y in range(height):
        for x in range(width):
            color = image.getpixel((x, y))
            plot_pixel(buffer, x, y, color)

    with open(output_file, 'wb') as f:
        f.write(buffer)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python png_to_hercules.py input_file output_file")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]

    png_to_hercules(input_file, output_file)
