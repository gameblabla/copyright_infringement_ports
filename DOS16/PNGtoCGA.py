import sys
from PIL import Image

def find_closest_color(color, color_list):
    min_distance = float("inf")
    closest_color = None
    for c in color_list:
        distance = sum((c[i] - color[i])**2 for i in range(3))
        if distance < min_distance:
            min_distance = distance
            closest_color = c
    return closest_color


def plot(x, y, color, cga_buffer):
    if y & 0x1:
        p = 0x2000
    else:
        p = 0x0000
    y >>= 1

    b, m = {
        0: (6, 0xC0),
        1: (4, 0x30),
        2: (2, 0x0C),
        3: (0, 0x03)
    }[x & 0x3]

    x >>= 2

    offset = p + (80 * y) + x
    c = cga_buffer[offset]
    c = (c & ~m) | (color << b)
    cga_buffer[offset] = c

def png_to_cga(png_file, output_cga):
    # Load the input image and reduce the number of colors to 4
    image = Image.open(png_file).convert('P', palette=Image.ADAPTIVE, colors=4)
    w, h = image.size
    if w > 320 or h > 200:
        raise ValueError("Image dimensions must be 320x200 or smaller")
    
    # Get the original palette
    palette = image.getpalette()[:12]
    palette = [tuple(palette[i:i+3]) for i in range(0, len(palette), 3)]

    # Replace each color in the palette with the closest color from the given list
    closest_color_list = [
        (0, 0, 0),
        (85, 85, 85),
        (170, 170, 170),
        (255, 255, 255)
    ]
    new_palette = [find_closest_color(color, closest_color_list) for color in palette]

    # Set the new palette to the input image
    new_palette = [item for sublist in new_palette for item in sublist]
    image.putpalette(new_palette)

    data = list(image.getdata())
    cga_buffer = bytearray(0x4000)

    for y in range(h):
        for x in range(w):
            color = data[x + y * w] & 0x3
            plot(x, y, color, cga_buffer)

    with open(output_cga, 'wb') as f:
        f.write(cga_buffer)
        
if __name__ == '__main__':
    import sys

    if len(sys.argv) < 3:
        print("Usage: python png_to_cga.py <input_png> <output_cga>")
        exit(1)

    input_png = sys.argv[1]
    output_cga = sys.argv[2]
    png_to_cga(input_png, output_cga)
