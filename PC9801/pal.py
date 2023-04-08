import sys
from PIL import Image

def png_to_pc98_palette(png_file):
    img = Image.open(png_file)
    if img.mode != "P":
        raise ValueError("Image must be in palette mode (indexed color)")

    rgb8_palette = img.getpalette()[:16*3]
    rgb8_palette = [tuple(rgb8_palette[i:i+3]) for i in range(0, len(rgb8_palette), 3)]

    pc98_palette = []

    for color in rgb8_palette:
        r, g, b = color
        r4 = round(r / 255 * 15)
        g4 = round(g / 255 * 15)
        b4 = round(b / 255 * 15)
        pc98_palette.extend([r4, g4, b4])

    return pc98_palette

def pc98_palette_to_c_array(pc98_palette, array_name="game_palette"):
    c_array = f"unsigned {array_name}[] = {{\n"

    for i in range(0, len(pc98_palette), 3):
        c_array += f"    0x0{pc98_palette[i]:X}, 0x0{pc98_palette[i+1]:X}, 0x0{pc98_palette[i+2]:X},\n"

    c_array += "};"
    return c_array

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python png_to_pc98_palette.py <input_png_file> <output_c_file>")
        sys.exit(1)

    input_png_file = sys.argv[1]
    output_c_file = sys.argv[2]

    pc98_palette = png_to_pc98_palette(input_png_file)
    c_array = pc98_palette_to_c_array(pc98_palette)

    with open(output_c_file, "w") as f:
        f.write(c_array)

    print(f"PC-98 palette C array has been saved to {output_c_file}")
