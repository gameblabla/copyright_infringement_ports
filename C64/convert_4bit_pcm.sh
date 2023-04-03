#!/bin/bash

#14489
#15040

if [ "$#" -ne 3 ]; then
    echo "Usage: $0 input_file output_file sample_rate"
    exit 1
fi

input_file="$1"
output_file="$2"
sample_rate="$3"

temp_file="temp_8bit.wav"

# Downconvert the input file with sox
sox "$input_file" -c 1 -r "$sample_rate" "$temp_file"

# Convert the 8-bit downconverted file to 4-bit PCM
python convert_8bit_to_4bit.py "$temp_file" "$output_file"

# Remove the temporary file
rm "$temp_file"
