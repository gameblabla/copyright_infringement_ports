import argparse
from PIL import Image

def pixelOffset(x, y):
    base = (0x2000-40) * (y & 0x3)
    return (y << 5) + (y << 3) + (x >> 1) + base

def plot(buffer, x, y, color):
    b, m = (0, 0x0F) if x & 0x1 else (4, 0xF0)
    index = pixelOffset(x, y)
    buffer[index] = (buffer[index] & ~m) | (color << b)

parser = argparse.ArgumentParser(description='Convert a PNG image to Tandy 320x200 16-color graphics mode')
parser.add_argument('input_file', help='input PNG file')
parser.add_argument('output_file', help='output binary file')

args = parser.parse_args()

# Open the input image and convert it to 16-color mode
with Image.open(args.input_file) as img:
    img = img.convert('P', palette=Image.ADAPTIVE, colors=16)
    width, height = img.size
    pixels = img.load()

    # Create a buffer for the output graphics mode
    buffer = bytearray(32768)
    
    # Iterate over each pixel and plot it in the buffer
    for y in range(height):
        for x in range(width):
            color = pixels[x, y]
            plot(buffer, x, y, color)
    
    # Write the buffer to the output file
    with open(args.output_file, 'wb') as out_file:
        out_file.write(buffer)
