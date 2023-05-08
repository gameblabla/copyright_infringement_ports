import sys
import struct

def main(input_file, output_file):
    with open(input_file, 'rb') as infile:
        raw_data = infile.read()

    samples = struct.unpack(f"{len(raw_data)}B", raw_data)
    packed_samples = []

    for i in range(0, len(samples), 2):
        sample1 = samples[i] & 0x0F
        if i + 1 < len(samples):  # Add this check
            sample2 = samples[i + 1] & 0x0F
        else:
            sample2 = 0
        packed_sample = (sample1 << 4) | sample2
        packed_samples.append(packed_sample)

    packed_data = struct.pack(f"{len(packed_samples)}B", *packed_samples)

    with open(output_file, 'wb') as outfile:
        outfile.write(packed_data)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python convert_audio.py <input_file> <output_file>")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]
    main(input_file, output_file)
