import sys
import numpy as np
from PIL import Image

def to_planar(image_data, width, height):
    planes = [np.zeros((height, width // 8), dtype=np.uint8) for _ in range(4)]

    for y in range(height):
        for x in range(width):
            color = image_data[y, x]
            pos = x // 8
            bit_mask = 1 << (7 - (x % 8))

            for plane_index in range(4):
                if color & (1 << plane_index):
                    planes[plane_index][y, pos] |= bit_mask
                else:
                    planes[plane_index][y, pos] &= ~bit_mask

    return np.concatenate(planes)

if len(sys.argv) != 3:
    print("Usage: python PNGtoPC98.py input.png output.bin")
    sys.exit(1)

input_file_path = sys.argv[1]
output_file_path = sys.argv[2]

# Load the PNG image
image = Image.open(input_file_path)

# Ensure image has 16 colors
if image.mode != 'P' or len(image.getpalette()) != 16 * 3:
    print("Error: Image must be a 16-color indexed PNG.")
    sys.exit(1)

width, height = image.size

# Convert image to planar format
image_data = np.array(image, dtype=np.uint8)
planar_data = to_planar(image_data, width, height)

# Write planar data to output file
with open(output_file_path, 'wb') as output_file:
    output_file.write(planar_data.tobytes())

print(f"Converted '{input_file_path}' to planar format and saved as '{output_file_path}'")
