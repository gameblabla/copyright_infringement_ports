import sys
import wave
import numpy as np
import struct

def convert_to_4bit_pcm(input_filename, output_filename):
    with wave.open(input_filename, 'rb') as input_wave:
        assert input_wave.getsampwidth() == 2, "Input WAV file must be 16-bit."
        num_samples = input_wave.getnframes()
        audio_data = input_wave.readframes(num_samples)
        audio_samples = np.frombuffer(audio_data, dtype=np.int16)

        # Normalize audio samples to range [0, 15]
        audio_samples = np.interp(audio_samples, (audio_samples.min(), audio_samples.max()), (0, 15)).astype(np.uint8)

        # Pad with an extra zero if the number of samples is odd
        if num_samples % 2 == 1:
            audio_samples = np.append(audio_samples, 0)
            num_samples += 1

        packed_data = np.zeros(num_samples // 2, dtype=np.uint8)
        for i in range(0, num_samples, 2):
            packed_data[i // 2] = (audio_samples[i] << 4) | audio_samples[i + 1]

        with open(output_filename, 'wb') as output_file:
            output_file.write(packed_data.tobytes())

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} input.wav output.bin")
        sys.exit(1)

    input_filename = sys.argv[1]
    output_filename = sys.argv[2]

    convert_to_4bit_pcm(input_filename, output_filename)
