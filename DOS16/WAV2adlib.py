import sys
import numpy as np
import wavio
from scipy import signal
from scipy.signal import butter, lfilter
from scipy.io.wavfile import read, write

def gamma_correction(samples, gamma=2.2):
    normalized_samples = samples / np.max(np.abs(samples))
    corrected_samples = np.sign(normalized_samples) * np.power(np.abs(normalized_samples), 1 / gamma)
    return corrected_samples

def resample_audio(audio, input_rate, output_rate):
    return signal.resample_poly(audio, output_rate, input_rate)

def low_pass_filter(audio, cutoff_frequency, sample_rate, order=5):
    nyquist_frequency = 0.5 * sample_rate
    if cutoff_frequency >= nyquist_frequency:
        cutoff_frequency = nyquist_frequency - 1
    normal_cutoff = cutoff_frequency / nyquist_frequency
    b, a = butter(order, normal_cutoff, btype='low', analog=False)
    return lfilter(b, a, audio)

def apply_delta_modulation(samples):
    delta_samples = np.diff(samples, prepend=samples[0])
    return delta_samples

def convert_wav_to_log6bit(input_filename, output_filename, resampling_rate=4000, low_pass_cutoff=2000):
    input_rate, samples = read(input_filename)

    # Handle mono audio
    if len(samples.shape) > 1:
        samples = samples[:, 0]

    # Resample audio
    resampled_samples = resample_audio(samples, input_rate, resampling_rate)

    # Apply low-pass filter
    filtered_samples = low_pass_filter(resampled_samples, low_pass_cutoff, resampling_rate)

    # Apply delta modulation
    delta_samples = apply_delta_modulation(filtered_samples)

    # Normalize audio to range [-1, 1]
    max_val = np.max(np.abs(delta_samples))
    normalized_samples = delta_samples / max_val

    # Apply gamma correction
    corrected_samples = gamma_correction(normalized_samples)

    # Convert to 6-bit samples
    scaled_samples = np.round((corrected_samples + 1) * 31.5).astype(np.uint8)

    # Save the 6-bit samples
    with open(output_filename, 'wb') as f:
        f.write(scaled_samples.tobytes())

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print("Usage: python wav_to_log6bit.py <input_wav_file> <output_log6bit_file>")
        sys.exit(1)

    input_filename = sys.argv[1]
    output_filename = sys.argv[2]

    convert_wav_to_log6bit(input_filename, output_filename)
