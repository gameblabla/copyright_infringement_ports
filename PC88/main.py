import sys
from PIL import Image

def pc88_v2_color_mode(png_file):
    custom_palette = [
        0, 0, 0,       # BLACK
        0, 0, 255,     # BLUE
        255, 0, 0,     # RED
        255, 0, 255,   # PINK
        0, 255, 0,     # GREEN
        0, 255, 255,   # CYAN
        255, 255, 0,   # YELLOW
        255, 255, 255, # WHITE
    ]

    image = Image.open(png_file).convert("RGB")
    width, height = image.size

    if width != 640:
        print("The input image width must be 640 pixels.")
        sys.exit(1)

    palette_image = Image.new("P", (1, 1))
    palette_image.putpalette(custom_palette)

    image = image.quantize(palette=palette_image).convert("P")

    plane_size = (height * 80)
    planes = {
        'green': bytearray(plane_size),
        'red': bytearray(plane_size),
        'blue': bytearray(plane_size),
    }


    for y in range(height):
        for x in range(width):
            pixel = image.getpixel((x, y))
            c = 0

            if pixel == 0:    # Black
                c = 0b000
            elif pixel == 1:  # Blue
                c = 0b001
            elif pixel == 2:  # Red
                c = 0b010
            elif pixel == 3:  # Magenta
                c = 0b011
            elif pixel == 4:  # Green
                c = 0b100
            elif pixel == 5:  # Cyan
                c = 0b101
            elif pixel == 6:  # Yellow
                c = 0b110
            elif pixel == 7:  # White
                c = 0b111

            po = 7 - (x % 8)
            byte_offset = (y * 80) + (x // 8)

            if c & 0b001:
                planes['blue'][byte_offset] |= 1 << po
            if c & 0b010:
                planes['red'][byte_offset] |= 1 << po
            if c & 0b100:
                planes['green'][byte_offset] |= 1 << po

    return planes

def main():
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <input_png>")
        sys.exit(1)

    input_png = sys.argv[1]

    output_data = pc88_v2_color_mode(input_png)
    base_filename = input_png.rsplit('.', 1)[0]

    for color, data in output_data.items():
        output_file = f"{base_filename}_{color}.bin"
        with open(output_file, 'wb') as f:
            f.write(data)
        print(f"Saved {color} plane to {output_file}")

if __name__ == "__main__":
    main()
