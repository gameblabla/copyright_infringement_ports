#!/bin/sh

if [ $# -ne 4 ]; then
    echo "Usage: $0 input.png output_prefix width height"
    exit 1
fi

python PNGtoEGA.py "$1" "$2" "$2".PAL
lzsa -r -f1 --prefer-ratio "$2"_plane0.bin "$2"_plane_0.lz
lzsa -r -f1 --prefer-ratio "$2"_plane1.bin "$2"_plane_1.lz
lzsa -r -f1 --prefer-ratio "$2"_plane2.bin "$2"_plane_2.lz
lzsa -r -f1 --prefer-ratio "$2"_plane3.bin "$2"_plane_3.lz
cat "$2"_plane_0.lz "$2"_plane_1.lz "$2"_plane_2.lz "$2"_plane_3.lz > "$2".lz1

echo "File size of ${2}_plane_0.lz: $(wc -c < "${2}_plane_0.lz") bytes"
echo "File size of ${2}_plane_1.lz: $(wc -c < "${2}_plane_1.lz") bytes"
echo "File size of ${2}_plane_2.lz: $(wc -c < "${2}_plane_2.lz") bytes"
echo "File size of ${2}_plane_3.lz: $(wc -c < "${2}_plane_3.lz") bytes"

./convert_tool.elf "$2".lz1 "$2".SIF "$3" "$4" "$3" "$4" $(wc -c < "${2}_plane_0.lz") $(wc -c < "${2}_plane_1.lz") $(wc -c < "${2}_plane_2.lz") $(wc -c < "${2}_plane_3.lz")


rm *_plane* *.lz "$2" *.lz1 "$2".RAW
