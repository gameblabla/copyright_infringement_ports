#!/bin/sh

if [ $# -ne 2 ]; then
    echo "Usage: $0 input.png output_prefix"
    exit 1
fi

python PNGtoCGA_NTSC.py "$1" "$2".RAW
lzsa -r -f1 --prefer-ratio "$2".RAW "$2".lz1

./convert_tool.elf "$2".lz1 "$2".SIF 320 200 320 200
rm *_plane* *.lz "$2" *.lz1 "$2".RAW
