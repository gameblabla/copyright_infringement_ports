#!/bin/bash

# input file
infile="$1"

# output file for stripped data
outfile="$2"

# output file for palette
palette_file="$3"

# strip out the first 34 bytes, starting from 3rd byte
dd if="$infile" of="$outfile" bs=1 skip=34

# extract the first 32 bytes after the 2nd byte
dd if="$infile" of="$palette_file" bs=1 skip=2 count=32

# run lz4w.jar
java -jar lz4w.jar p "$outfile" "${outfile}.lz"
