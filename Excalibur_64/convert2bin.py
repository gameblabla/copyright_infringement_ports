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

# Define the Excalibur 64 palette
excalibur_palette = [
    (0, 0, 0), (255, 0, 0), (0, 0, 255), (255, 0, 255),
    (0, 255, 0), (255, 255, 0), (0, 255, 255), (255, 255, 255),
    (217, 0, 0), (255, 85, 38), (255, 0, 38), (217, 85, 0),
    (38, 0, 38), (73, 255, 73), (255, 0, 144), (38, 85, 38)
]
color_map = {color: i for i, color in enumerate(excalibur_palette)}

# Define the Excalibur palette mapping
excalibur_palette_mapping = [0x00, 0x12, 0x24, 0x36, 0x48, 0x5A, 0x6C, 0x7E, 0x80, 0x92, 0xA4, 0xB6, 0xC8, 0xDA, 0xEC, 0xFE]

# Get the width and height of the image
width, height = img.size

# Create an array to hold the binary values of each pixel
binary_array = []

# Loop through each pixel in the image and convert it to binary
for y in range(height):
    for x in range(width):
        pixel = img.getpixel((x, y))
        color_index = color_map[tuple(palette[pixel * 3:pixel * 3 + 3])]
        binary_value = excalibur_palette_mapping[color_index]
        binary_array.append(binary_value)

# Convert the binary array to bytes
byte_array = bytes(binary_array)

# Write the bytes to the output binary file
with open(args.output_file, 'wb') as f:
    f.write(byte_array)
