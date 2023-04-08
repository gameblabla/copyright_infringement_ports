#!/bin/bash

if [ "$#" -ne 1 ]; then
  echo "Usage: $0 inputfile"
  exit 1
fi

inputfile="$1"

if [ ! -e "$inputfile" ]; then
  echo "Error: File not found: $inputfile"
  exit 1
fi

filesize=$(stat -c%s "$inputfile")
partsize=$((filesize / 4))

for i in {0..3}; do
  skip=$((i * partsize))
  outputfile="${inputfile}_part$(($i + 1))"
  dd if="$inputfile" of="$outputfile" bs="$partsize" count=1 skip="$i" 2>/dev/null
  echo "Created $outputfile"
done
