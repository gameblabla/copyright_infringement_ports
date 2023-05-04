from PIL import Image
import sys

# Define the 16 color palette
palette = [
    (0, 0, 0), (0, 0, 170), (0, 170, 0), (0, 170, 170),
    (170, 0, 0), (170, 0, 170), (170, 170, 0), (170, 170, 170),
    (85, 85, 85), (85, 85, 255), (85, 255, 85), (85, 255, 255),
    (255, 85, 85), (255, 85, 255), (255, 255, 85), (255, 255, 255)
]

def convert_to_png(bytes):
    """
    Converts 4bpp linear RAW bytes to a PNG image with a width of 720 pixels and the 16 color palette.
    """
    # Calculate the image size based on the length of the input data
    width = 720
    height = len(bytes) // (width // 2)

    # Create a new PIL image with the specified size and mode
    img = Image.new('P', (width, height), color=0)

    # Set the image palette to the 16 color palette
    img.putpalette([c for color in palette for c in color])

    # Convert the bytes to pixel values
    pixels = []
    for byte in bytes:
        pixels.append(byte >> 4)
        pixels.append(byte & 0x0F)

    # Set the image pixels
    img.putdata(pixels)

    # Return the image
    return img

if __name__ == '__main__':
    # Get input and output file names from argv
    input_file = sys.argv[1]
    output_file = sys.argv[2]

    # Open the input file
    with open(input_file, 'rb') as f:
        # Read the bytes from the input file
        bytes = f.read()

        # Convert the bytes to a PNG image
        img = convert_to_png(bytes)

        # Save the PNG image to the output file
        img.save(output_file)
