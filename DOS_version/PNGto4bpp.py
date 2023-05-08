from PIL import Image
import sys

def convert_to_4bpp(img):
    """
    Converts a PIL image to 4bpp linear RAW format.
    Each byte contains two 4bpp pixels (one pixel, and the one after it).
    """

    # Get the pixel values as a list of integers
    pixels = list(img.getdata())

    # Convert the pixels to 4bpp format
    byte_list = []
    for i in range(0, len(pixels), 2):
        byte = (pixels[i] << 4) | pixels[i+1]
        byte_list.append(byte)

    # Convert the list of bytes to a bytes object
    bytes_obj = bytes(byte_list)

    # Return the bytes object
    return bytes_obj

if __name__ == '__main__':
    # Get input and output file names from argv
    input_file = sys.argv[1]
    output_file = sys.argv[2]

    # Open the input file
    with Image.open(input_file) as img:
        # Check if the image has 16 colors
        if img.mode != 'P':
            raise ValueError("Input PNG file must have a mode of 'P' and a 16 color palette")

        # Convert the image to 4bpp format
        bytes_obj = convert_to_4bpp(img)

        # Write the bytes to the output file
        with open(output_file, 'wb') as f:
            f.write(bytes_obj)
