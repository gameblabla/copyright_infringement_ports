import argparse
from PIL import Image

# Parse command-line arguments
parser = argparse.ArgumentParser(description='Export image as RLE-encoded binary file.')
parser.add_argument('input_file', help='path to input PNG file')
parser.add_argument('output_file', help='path to output RLE-encoded binary file')
args = parser.parse_args()

# Open the image file
img = Image.open(args.input_file)

# Define the color mapping using the image's palette
palette = img.getpalette()
color_map = {
    tuple(palette[0:3]): 0,   # black
    tuple(palette[3:6]): 1,   # dark gray
    tuple(palette[6:9]): 2,   # light gray
    tuple(palette[9:12]): 3   # white
}

# Get the width and height of the image
width, height = img.size

# Create an array to hold the binary values of each pixel
binary_array = []

# Loop through each pixel in the image and convert it to binary
for y in range(height):
    for x in range(width):
        pixel = img.getpixel((x, y))
        binary_value = color_map[tuple(palette[pixel*3:pixel*3+3])]
        binary_array.append(binary_value)

# Perform RLE compression on the binary data
compressed_data = bytearray()

i = 0
while i < len(binary_array):
    count = 1
    while i + count < len(binary_array) and binary_array[i + count] == binary_array[i]:
        count += 1
    if count > 2:
        compressed_data.append(count)
        compressed_data.append(binary_array[i])
        i += count
    else:
        for j in range(count):
            compressed_data.append(binary_array[i + j])
        i += count

# Write the compressed data to the output binary file
with open(args.output_file, 'wb') as f:
    f.write(compressed_data)
