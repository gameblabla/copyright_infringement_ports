#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

void copy_binary(FILE *input, FILE *output) {
    fseek(input, 0, SEEK_END);
    long length = ftell(input);
    fseek(input, 0, SEEK_SET);

    uint8_t *buffer = (uint8_t *)malloc(length);
    if (!buffer) {
        fprintf(stderr, "Memory allocation error.\n");
        return;
    }

    fread(buffer, 1, length, input);
    fwrite(buffer, 1, length, output);

    free(buffer);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    FILE *input = fopen(argv[1], "rb");
    FILE *output = fopen(argv[2], "wb");

    if (!input || !output) {
        fprintf(stderr, "Error opening files.\n");
        return 1;
    }

    // Write the 2-byte load address (little-endian)
    uint16_t load_address = 001000;
    fwrite(&load_address, sizeof(uint16_t), 1, output);

    // Write the 2-byte length (little-endian)
    fseek(input, 0, SEEK_END);
    uint16_t length = ftell(input);
    fseek(input, 0, SEEK_SET);
    uint16_t length_uint16 = (uint16_t)length;
    fwrite(&length_uint16, sizeof(uint16_t), 1, output);

    // Copy the rest of the raw binary file
    copy_binary(input, output);

    fclose(input);
    fclose(output);
    return 0;
}
