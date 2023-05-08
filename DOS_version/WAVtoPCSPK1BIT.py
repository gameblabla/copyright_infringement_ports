import sys
import wave
import numpy as np
from scipy.signal import butter, filtfilt, iirfilter, lfilter

# Define the ISO 226:2003 equal-loudness contours
ISO_226_FREQUENCIES = [20, 25, 31.5, 40, 50, 63, 80, 100, 125, 160, 200, 250,
                      315, 400, 500, 630, 800, 1000, 1250, 1600, 2000, 2500,
                      3150, 4000, 5000, 6300, 8000, 10000, 12500]

ISO_226_LEVELS = [
    [30, 18, 12, 8, 7, 6, 6, 7, 7, 8, 9, 10, 11, 12, 12, 12, 11, 11, 11, 11, 11,
     11, 11, 11, 11, 11, 12, 12, 13],
    [60, 48, 42, 38, 37, 36, 36, 37, 37, 38, 39, 40, 41, 42, 42, 42, 41, 41, 41,
     41, 41, 41, 41, 41, 41, 41, 42, 42, 43]
]

# Equal-loudness filter
def equal_loudness_filter(data, sample_rate):
    equal_loudness_data = np.zeros_like(data, dtype=np.float32)
    
    nyquist_freq = sample_rate / 2
    valid_frequencies = [f for f in ISO_226_FREQUENCIES if f < nyquist_freq]
    
    for idx, center_freq in enumerate(valid_frequencies):
        lower_edge = center_freq * 2 ** (-1 / 6)
        upper_edge = center_freq * 2 ** (1 / 6)
        
        if upper_edge > nyquist_freq:
            upper_edge = nyquist_freq
        
        b, a = iirfilter(2, [lower_edge, upper_edge],
                         btype='band', ftype='butter', fs=sample_rate)
        filtered_data = lfilter(b, a, data)
        
        loudness_offset = ISO_226_LEVELS[1][idx] - ISO_226_LEVELS[0][idx]
        equal_loudness_data += 10 ** (loudness_offset / 20) * filtered_data
    
    return equal_loudness_data

# Low-pass filter
def low_pass_filter(data, cutoff_freq, sample_rate, order=5):
    nyquist_freq = 0.5 * sample_rate
    normalized_cutoff_freq = cutoff_freq / nyquist_freq
    b, a = butter(order, normalized_cutoff_freq, btype='low', analog=False)
    return filtfilt(b, a, data)

def comb_cascade(audio_data):
    prv = [0, 0, 0]
    output = np.zeros(len(audio_data), dtype=np.int32)

    for i, x in enumerate(audio_data):
        tmp = x - prv[0]
        prv[0] = x
        x = tmp

        tmp -= prv[1]
        prv[1] = x
        x = tmp

        tmp -= prv[2]
        prv[2] = x
        x = tmp

        output[i] = x

    return output

def integrator_cascade(audio_data):
    sum_int = [0, 0, 0]
    output = np.zeros(len(audio_data), dtype=np.int32)

    for i, x in enumerate(audio_data):
        sum_int[0] += x
        sum_int[1] += sum_int[0]
        sum_int[2] += sum_int[1]
        output[i] = sum_int[2]

    return output

def sigma_delta_modulation(audio_data, order=1):
    sum_mod = 0
    sum_mod2 = 0
    one_bit_pcm_data = np.zeros(len(audio_data), dtype=np.uint8)

    for i, x in enumerate(audio_data):
        if order == 1:
            sum_mod = x
        y = int(sum_mod >= 0)
        e = (y << 24) - (1 << 23)
        if order >= 2:
            sum_mod2 += x - e
            x = sum_mod2
        sum_mod += x - e
        one_bit_pcm_data[i] = y

    return one_bit_pcm_data

def wav_to_1bit_pcm(input_file, output_file):
    with wave.open(input_file, 'rb') as wav_file:
        params = wav_file.getparams()
        if params.sampwidth != 2:
            raise ValueError('Input WAV file must be 16-bit.')

        frames = wav_file.readframes(params.nframes)
        audio_data = np.frombuffer(frames, dtype=np.int16).astype(np.int32)

    # Apply filters
    audio_data = comb_cascade(audio_data)
    audio_data = integrator_cascade(audio_data)
    
    # Apply low-pass filter
    cutoff_freq = 4000
    audio_data = low_pass_filter(audio_data, cutoff_freq, params.framerate)
    
    # 3. Equal-loudness filter
    audio_data = equal_loudness_filter(audio_data, params.framerate)
    
    # Apply sigma-delta modulation
    order = 1
    one_bit_pcm_data = sigma_delta_modulation(audio_data, order)

    # Pack 8 bits into 1 byte
    num_bytes = (len(one_bit_pcm_data) + 7) // 8
    packed_data = np.zeros(num_bytes, dtype=np.uint8)

    for i in range(len(one_bit_pcm_data)):
        packed_data[i // 8] |= one_bit_pcm_data[i] << (i % 8)

    # Save as binary 1-bit PCM raw linear file
    with open(output_file, 'wb') as bin_file:
        bin_file.write(packed_data.tobytes())


if __name__ == '__main__':
    if len(sys.argv) != 3:
        print("Usage: python wav_to_1bit_pcm.py input.wav output.bin")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]

    try:
        wav_to_1bit_pcm(input_file, output_file)
        print(f"Successfully converted {input_file} to {output_file}")
    except Exception as e:
        print(f"Error: {e}")
