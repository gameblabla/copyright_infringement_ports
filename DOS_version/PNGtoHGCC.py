import sys
import numpy as np
from PIL import Image
from sklearn.cluster import KMeans


PALETTE = [
    (0, 0, 0),
    (0, 0, 170),
    (0, 170, 0),
    (0, 170, 170),
    (170, 0, 0),
    (170, 0, 170),
    (170, 170, 0),
    (170, 170, 170),
    (85, 85, 85),
    (85, 85, 255),
    (85, 255, 85),
    (85, 255, 255),
    (255, 85, 85),
    (255, 85, 255),
    (255, 255, 85),
    (255, 255, 255)
]

# EGA 64-color palette, also applies to HGCC it appears !
ega_palette_64 = [
(0x00,0x00,0x00),
(0x00,0x00,0xAA),
(0x00,0xAA,0x00),
(0x00,0xAA,0xAA),
(0xAA,0x00,0x00),
(0xAA,0x00,0xAA),
(0xAA,0xAA,0x00),
(0xAA,0xAA,0xAA),
(0x00,0x00,0x55),
(0x00,0x00,0xFF),
(0x00,0xAA,0x55),
(0x00,0xAA,0xFF),
(0xAA,0x00,0x55),
(0xAA,0x00,0xFF),
(0xAA,0xAA,0x55),
(0xAA,0xAA,0xFF),
(0x00,0x55,0x00),
(0x00,0x55,0xAA),
(0x00,0xFF,0x00),
(0x00,0xFF,0xAA),
(0xAA,0x55,0x00),
(0xAA,0x55,0xAA),
(0xAA,0xFF,0x00),
(0xAA,0xFF,0xAA),
(0x00,0x55,0x55),
(0x00,0x55,0xFF),
(0x00,0xFF,0x55),
(0x00,0xFF,0xFF),
(0xAA,0x55,0x55),
(0xAA,0x55,0xFF),
(0xAA,0xFF,0x55),
(0xAA,0xFF,0xFF),
(0x55,0x00,0x00),
(0x55,0x00,0xAA),
(0x55,0xAA,0x00),
(0x55,0xAA,0xAA),
(0xFF,0x00,0x00),
(0xFF,0x00,0xAA),
(0xFF,0xAA,0x00),
(0xFF,0xAA,0xAA),
(0x55,0x00,0x55),
(0x55,0x00,0xFF),
(0x55,0xAA,0x55),
(0x55,0xAA,0xFF),
(0xFF,0x00,0x55),
(0xFF,0x00,0xFF),
(0xFF,0xAA,0x55),
(0xFF,0xAA,0xFF),
(0x55,0x55,0x00),
(0x55,0x55,0xAA),
(0x55,0xFF,0x00),
(0x55,0xFF,0xAA),
(0xFF,0x55,0x00),
(0xFF,0x55,0xAA),
(0xFF,0xFF,0x00),
(0xFF,0xFF,0xAA),
(0x55,0x55,0x55),
(0x55,0x55,0xFF),
(0x55,0xFF,0x55),
(0x55,0xFF,0xFF),
(0xFF,0x55,0x55),
(0xFF,0x55,0xFF),
(0xFF,0xFF,0x55),
(0xFF,0xFF,0xFF),
]

def closest_color(pixel, palette):
    distances = [(index, np.linalg.norm(np.array(pixel) - np.array(color))) for index, color in enumerate(palette)]
    return min(distances, key=lambda x: x[1])[0]
   

def select_16_colors(image, full_palette):
    pixels = np.array(image).reshape(-1, 3)
    kmeans = KMeans(n_clusters=16, init='k-means++', random_state=42)
    kmeans.fit(pixels)
    centroids = kmeans.cluster_centers_.round().astype(int)
    closest_colors = [tuple(full_palette[closest_color(tuple(centroid), full_palette)]) for centroid in centroids]
    return closest_colors


def plot_pixel(buffers, x, y, color):
    offset = ((y >> 2) * 90) + ((y & 3) * 0x2000) + (x >> 3)
    pixel_mask = 0x80 >> (x & 7)

    for plane in range(4):
        if color & (1 << plane):
            buffers[plane][offset] |= pixel_mask
        else:
            buffers[plane][offset] &= ~pixel_mask

def convert_image_to_ega(img, palette):
    width, height = img.size
    img_ega = Image.new('P', img.size)

    # Assign the selected palette to the output image
    img_ega.putpalette([component for color in palette for component in color])

    for y in range(height):
        for x in range(width):
            pixel = img.getpixel((x, y))
            index = closest_color(pixel, palette)
            img_ega.putpixel((x, y), index)

    return img_ega
    
def save_palette_to_binary_file(palette, full_palette, output_filename):
    with open(output_filename, 'wb') as f:
        for color in palette:
            index = full_palette.index(color)
            f.write(index.to_bytes(1, 'little'))

def png_to_hercules(input_file, output_file, output_palette=None, use_custom_palette=True):
    img = Image.open(input_file).convert('RGB')
    width, height = img.size
    assert width == 720, "Image width must be 720"

    # Calculate buffer size based on input height
    buffer_size = int(32768 * (height / 348))

    if use_custom_palette:
        # Select the best 16 colors from the 64-color EGA palette
        custom_16_color_palette = select_16_colors(img, ega_palette_64)
        chosen_palette = custom_16_color_palette
    else:
        chosen_palette = PALETTE
    
    # Convert input image to use the chosen palette
    img = convert_image_to_ega(img, chosen_palette)
    if output_palette and use_custom_palette:
        save_palette_to_binary_file(custom_16_color_palette, ega_palette_64, output_palette)

    buffers = [bytearray(buffer_size) for _ in range(4)]  # Initialize four buffers with calculated size

    for y in range(height):
        for x in range(width):
            color = img.getpixel((x, y))
            plot_pixel(buffers, x, y, color)

    for i, buf in enumerate(buffers):
        with open(f"{output_file}_plane_{i}", 'wb') as f:
            f.write(buf)

    with open(output_file, 'wb') as f:
        for buf in buffers:
            f.write(buf)


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="Convert a PNG image to Hercules InColor format")
    parser.add_argument("input_file", help="Input PNG file")
    parser.add_argument("output_file", help="Output Hercules InColor file")
    parser.add_argument("-p", "--palette", help="Output binary file for the custom palette (default: 'palette.bin')", default="palette.bin")
    parser.add_argument("-c", "--cga", help="Use CGA palette instead of custom palette and skip exporting the palette", action="store_true")

    args = parser.parse_args()

    if args.cga:
        use_custom_palette = False
        args.palette = None
    else:
        use_custom_palette = True

    png_to_hercules(args.input_file, args.output_file, args.palette, use_custom_palette)
