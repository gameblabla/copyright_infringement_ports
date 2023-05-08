import sys
from PIL import Image
import numpy as np

def png_to_raw_vga(input_filename, output_filename, output_file_prefix, palette_filename):
    img = Image.open(input_filename)
    width, height = img.size
    img_array = np.array(img)

    planes = [bytearray(width * height // 8) for _ in range(4)]

    for y in range(height):
        for x in range(width):
            color = img_array[y, x]
            base_offset = (y * (width // 8)) + (x >> 3)
            mask = 0x80 >> (x & 7)

            for plane in range(4):
                if (color >> plane) & 1:
                    planes[plane][base_offset] |= mask
                else:
                    latch = planes[plane][base_offset]
                    planes[plane][base_offset] = latch & ~mask

    # Export the palette
    raw_palette = np.array(img.getpalette()).reshape((-1, 3))
    vga_palette = (raw_palette // 4).astype(np.uint8)
    
    with open(palette_filename, 'wb') as f:
        f.write(vga_palette.tobytes())

    # Save all planes combined to the output file
    with open(output_filename, "wb") as f:
        for plane in planes:
            f.write(plane)

    # Save each plane to its own file
    for i, plane in enumerate(planes):
        output_file_individual = f"{output_file_prefix}_plane{i}.bin"
        with open(output_file_individual, 'wb') as f:
            f.write(plane)

if __name__ == "__main__":
    import sys

    if len(sys.argv) != 5:
        print(f"Usage: {sys.argv[0]} input.png output.raw output_prefix palette.bin")
        sys.exit(1)

    input_filename = sys.argv[1]
    output_filename = sys.argv[2]
    output_file_prefix = sys.argv[3]
    palette_filename = sys.argv[4]

    png_to_raw_vga(input_filename, output_filename, output_file_prefix, palette_filename)
