import sys
import numpy as np
from PIL import Image
from sklearn.cluster import KMeans

DEFAULT_EGA_PALETTE = [
    (0, 0, 0),
    (0, 0, 170),
    (0, 170, 0),
    (0, 170, 170),
    (170, 0, 0),
    (170, 0, 170),
    (170, 85, 0),
    (170, 170, 170),
    (85, 85, 85),
    (85, 85, 255),
    (85, 255, 85),
    (85, 255, 255),
    (255, 85, 85),
    (255, 85, 255),
    (255, 255, 85),
    (255, 255, 255),
]


CPALETTE = [
  (0,   0,   0),
 (85,   0,   0),
 (85,  85,   0),
 (85,  85,  85),
 (85,  85, 170),
(170,  85,  85),
(170,  85, 170),
(255,  85,  85),
(170, 170,  85),
(170, 170, 170),
(170, 170, 255),
(255, 170,  85),
(255, 170, 170),
(255, 170, 255),
(255, 255, 170),
(255, 255, 255),
]


# EGA 64-color palette
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

def ega_plot(pixels, x, y, color, plane_size, width):
    if width <= 320:
        base_offset = ((y * 320) + x) >> 3
    else:
        base_offset = ((y * 80) + (x >> 3)) & 0xFFFF

    mask = 0x80 >> (x & 7)
    for plane in range(4):
        if color & (1 << plane):
            pixels[base_offset + (plane_size * plane)] |= mask
        else:
            latch = pixels[base_offset + (plane_size * plane)]
            pixels[base_offset + (plane_size * plane)] = latch & ~mask


def save_planes_to_files(ega_pixels, plane_size, base_filename):
    for plane in range(4):
        output_filename = f"{base_filename}_plane{plane}.bin"
        with open(output_filename, "wb") as f:
            f.write(ega_pixels[plane_size * plane : plane_size * (plane + 1)])


def png_to_ega(input_file, output_file, output_palette=None):
    img = Image.open(input_file)
    width, height = img.size

    if width <= 320 and height <= 200:
        plane_size = 8000
        img = img.convert(mode="P", palette=Image.ADAPTIVE, colors=16)
        img.putpalette(sum(DEFAULT_EGA_PALETTE, ()))
    elif width <= 640 and height <= 200:
        plane_size = 16000
        img = img.convert(mode="P", palette=Image.ADAPTIVE, colors=16)
        img.putpalette(sum(DEFAULT_EGA_PALETTE, ()))
    elif width <= 640 and height <= 350:
        plane_size = 28000
        img_rgb = img.convert('RGB')
        custom_16_color_palette = select_16_colors(img_rgb, ega_palette_64)
        custom_16_color_indices = [ega_palette_64.index(color) for color in custom_16_color_palette]

        img = convert_image_to_ega(img_rgb, custom_16_color_palette)
        if output_palette:
            save_palette_to_binary_file(custom_16_color_palette, ega_palette_64, output_palette)
    else:
        raise ValueError("Invalid image size")

    ega_pixels = bytearray(plane_size * 4)

    for y in range(height):
        for x in range(width):
            color = img.getpixel((x, y))
            ega_plot(ega_pixels, x, y, color, plane_size, width)

    with open(output_file, "wb") as f:
        f.write(ega_pixels)

    save_planes_to_files(ega_pixels, plane_size, output_file)

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python png_to_ega.py input.png output.ega [output_palette.bin]")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]
    output_palette = sys.argv[3] if len(sys.argv) > 3 else None
    png_to_ega(input_file, output_file, output_palette)
