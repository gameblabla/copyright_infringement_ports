import sys
from PIL import Image
import numpy as np

def convert_image(image_path):
    # Open the image
    img = Image.open(image_path)

    # Resize the image to GBA size
    img.thumbnail((240, 160))

    # Convert the image to P mode (8-bit pixels, mapped to palette entries)
    img = img.convert('P', palette=Image.ADAPTIVE, colors=256)

    # Get the image data
    data = np.array(img)

    # Get the palette data and convert to BGR555
    palette = img.getpalette()[:256*3]  # We only want 256 colors
    bgr555_palette = [(r >> 3, g >> 3, b >> 3) for r, g, b in zip(palette[0::3], palette[1::3], palette[2::3])]
    palette_bytes = [color[2] | (color[1] << 5) | (color[0] << 10) for color in bgr555_palette]

    # Save the raw image data
    with open(image_path + '.raw', 'wb') as f:
        data.tofile(f)

    # Save the palette data
    with open(image_path + '.pal', 'wb') as f:
        for color in palette_bytes:
            f.write(color.to_bytes(2, 'little'))

def main(argv):
    if len(argv) != 2:
        print(f"Usage: python {argv[0]} <image>")
        return
    convert_image(argv[1])

if __name__ == "__main__":
    main(sys.argv)
