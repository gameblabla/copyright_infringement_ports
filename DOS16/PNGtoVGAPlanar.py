from PIL import Image
import numpy as np
import sys

def img_to_mode_x(img, output_file, output_file_prefix, palette_file):
    img = img.convert('P', palette=Image.ADAPTIVE, colors=256)
    img_data = np.array(img)
    width, height = img.size

    planes = []
    for _ in range(4):
        planes.append(np.zeros(width * height // 4, dtype=np.uint8))

    for y in range(height):
        for x in range(width):
            pixel = img_data[y, x]
            plane = x & 3
            byte_idx = (width * y + x) // 4
            planes[plane][byte_idx] = pixel

    # Save the combined file
    with open(output_file, 'wb') as f:
        for plane in planes:
            f.write(plane.tobytes())

    # Save each plane to its own file
    for i, plane in enumerate(planes):
        output_file_individual = f"{output_file_prefix}_plane{i}.bin"
        with open(output_file_individual, 'wb') as f:
            f.write(plane.tobytes())

    # Export the palette
    raw_palette = np.array(img.getpalette()).reshape((-1, 3))
    vga_palette = (raw_palette // 4).astype(np.uint8)

    with open(palette_file, 'wb') as f:
        for color in vga_palette:
            f.write(color.tobytes())

if len(sys.argv) != 5:
    print(f"Usage: {sys.argv[0]} input.png output.bin output_prefix palette.bin")
    sys.exit(1)

input_file = sys.argv[1]
output_file = sys.argv[2]
output_file_prefix = sys.argv[3]
palette_file = sys.argv[4]
image = Image.open(input_file)
img_to_mode_x(image, output_file, output_file_prefix, palette_file)
