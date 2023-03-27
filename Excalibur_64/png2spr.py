import sys
import os
from PIL import Image, ImageOps
import struct

def convert_to_sprite(filename, invert=False):
    img = Image.open(filename).convert('L')
    if invert:
        img = ImageOps.invert(img)
    threshold = 128
    img = img.point(lambda x: 0 if x < threshold else 1, '1')
    width, height = img.size
    data = bytearray()
    data.extend(bytes([width, height]))
    for y in range(0, height, 1):
        byte = 0
        for x in range(0, width, 1):
            if x % 8 == 0 and x > 0:
                data.append(byte)
                byte = 0
            pixel = img.getpixel((x, y))
            byte |= pixel << (7 - (x % 8))
        data.append(byte)
    return data

if len(sys.argv) < 2:
    print('Usage: python sprite_converter.py <filename> [-i]')
else:
    filename = sys.argv[1]
    invert = False
    if len(sys.argv) == 3 and sys.argv[2] == '-i':
        invert = True
    
    # Get the base filename without extension
    base_filename = os.path.splitext(filename)[0]
    
    # Generate the C header file
    sprite_data = convert_to_sprite(filename, invert)
    with open(f'{base_filename}.h', 'w') as header_file:
        header_file.write(f'// Generated from {filename}\n')
        header_file.write(f'const unsigned char {base_filename}[] = {{\n')
        header_file.write(f'  {sprite_data[0]}, {sprite_data[1]},')
        for i, byte in enumerate(sprite_data[2:]):
            if i % 16 == 0:
                header_file.write('\n  ')
            header_file.write(f'0x{byte:02x}, ')
        header_file.write('\n};\n')

    # Generate the binary file
    with open(f'{base_filename}.bin', 'wb') as binary_file:
        binary_file.write(struct.pack(f'!{len(sprite_data)}B', *sprite_data))
