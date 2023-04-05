import sys
import wave
import struct

def clamp(n, smallest, largest):
    return max(smallest, min(n, largest))

def convert_wav_to_adpcm(input_file, output_file):
    step_table = [
        16, 17, 19, 21, 23, 25, 28, 31, 34, 37, 41, 45, 50, 55, 60, 66,
        73, 80, 88, 97, 107, 118, 130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
        337, 371, 408, 449, 494, 544, 598, 658, 724, 796, 876, 963, 1060, 1166, 1282, 1411,
        1552, 1707, 1878, 2066, 2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358, 5894, 6484,
        7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899, 15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794,
        32767
    ]

    index_table = [
        -1, -1, -1, -1, 2, 4, 6, 8,
        -1, -1, -1, -1, 2, 4, 6, 8
    ]

    with wave.open(input_file, 'rb') as wav_file:
        n_channels, sampwidth, framerate, n_frames, _, _ = wav_file.getparams()
        assert n_channels == 1, "Input file must be mono"
        assert sampwidth == 1, "Input file must be 8-bit"

        wav_data = wav_file.readframes(n_frames)

    sample = 0
    step_index = 0
    adpcm_data = bytearray()

    for i in range(0, len(wav_data), 2):
        nibble1 = 0
        nibble2 = 0

        for j in range(2):
            diff = struct.unpack('b', wav_data[i + j:i + j + 1])[0] - sample

            sign = 0

            if diff < 0:
                sign = 8
                diff = -diff

            step = step_table[step_index]
            delta = step >> 3

            if diff > step_table[step_index] >> 1:
                delta |= step >> 2
                diff -= step_table[step_index] >> 1

            if diff > step_table[step_index] >> 2:
                delta |= step >> 3
                diff -= step_table[step_index] >> 2

            step_index = clamp(step_index + index_table[delta & 0x0F], 0, len(step_table) - 1)

            delta |= sign
            sample += struct.unpack('b', struct.pack('B', (delta << 4) & 0xFF))[0] * step // 64

            sample = clamp(sample, -128, 127)

            if j == 0:
                nibble1 = delta
            elif j == 1:
                nibble2 = delta

        adpcm_byte = ((nibble1 << 4) | nibble2) & 0xFF

        adpcm_data.append(adpcm_byte)

    with open(output_file, 'wb') as adpcm_file:
        adpcm_file.write(adpcm_data)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python wav_to_okim6258_adpcm.py <input_wav_file> <output_adpcm_file>")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]

    convert_wav_to_adpcm(input_file, output_file)
