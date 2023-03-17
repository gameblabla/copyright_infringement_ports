import argparse
from PIL import Image

# Parse command-line arguments
parser = argparse.ArgumentParser(description='Export image as binary file with color mapping.')
parser.add_argument('input_file', help='path to input PNG file')
parser.add_argument('output_file', help='path to output binary file')
args = parser.parse_args()

# Open the image file
img = Image.open(args.input_file)

# Define the color mapping using the image's palette
palette = img.getpalette()
color_map = {
    tuple(palette[0:3]): 32,   # black
    tuple(palette[3:6]): 219,   # dark gray
    tuple(palette[6:9]): 219,   # light gray
    tuple(palette[9:12]): 219   # white
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

# Convert the binary array to bytes
byte_array = bytes(binary_array)

# Write the bytes to the output binary file
with open(args.output_file, 'wb') as f:
    f.write(byte_array)
