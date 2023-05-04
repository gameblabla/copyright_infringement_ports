from PIL import Image
import numpy as np
import sys

def img_to_linear(img, output_file, palette_file):
    img = img.convert('P', palette=Image.ADAPTIVE, colors=256)
    img_data = np.array(img)
    width, height = img.size

    linear_data = np.zeros(width * height, dtype=np.uint8)

    for y in range(height):
        for x in range(width):
            pixel = img_data[y, x]
            linear_data[y * width + x] = pixel

    with open(output_file, 'wb') as f:
        f.write(linear_data.tobytes())

    # Export the palette
    raw_palette = np.array(img.getpalette()[:256*3]).reshape((256, 3))
    vga_palette = (raw_palette // 4).astype(np.uint8)

    with open(palette_file, 'wb') as f:
        for color in vga_palette:
            f.write(color.tobytes())

if len(sys.argv) != 4:
    print(f"Usage: {sys.argv[0]} input.png output.bin palette.bin")
    sys.exit(1)

input_file = sys.argv[1]
output_file = sys.argv[2]
palette_file = sys.argv[3]
image = Image.open(input_file)
img_to_linear(image, output_file, palette_file)
