import wave
import numpy as np
import sys

def convert_to_4bit_pcm(input_file, output_file):
    with wave.open(input_file, 'rb') as wav_in:
        params = wav_in.getparams()
        n_channels, sampwidth, framerate, n_frames = params[:4]

        if n_channels != 1:
            raise ValueError('The input WAV file should be mono')

        audio_data = wav_in.readframes(n_frames)
        audio_data = np.frombuffer(audio_data, dtype=np.uint8)

    # Normalize to 4-bit
    audio_data = audio_data >> 4

    # If the number of samples is odd, add a zero-padding sample
    if len(audio_data) % 2 != 0:
        audio_data = audio_data[:-1]

    # Pack two 4-bit samples into one byte
    packed_data = np.zeros(len(audio_data) // 2, dtype=np.uint8)
    packed_data = (audio_data[::2] << 4) | audio_data[1::2]

    with open(output_file, 'wb') as pcm_out:
        pcm_out.write(packed_data.tobytes())


if __name__ == '__main__':
    if len(sys.argv) < 3:
        print(f'Usage: python {sys.argv[0]} input_file output_file')
        sys.exit(1)
    input_file = sys.argv[1]
    output_file = sys.argv[2]

    convert_to_4bit_pcm(input_file, output_file)
    print(f'4-bit PCM conversion completed. Output file: {output_file}')
