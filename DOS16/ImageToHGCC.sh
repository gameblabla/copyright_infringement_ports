#!/bin/sh
python PNGtoHGCC.py "$1" "$2" --p "$2".PAL
lzsa -f1 -r "$2"_plane_0 "$2"_plane_0.lz
lzsa -f1 -r "$2"_plane_1 "$2"_plane_1.lz
lzsa -f1 -r "$2"_plane_2 "$2"_plane_2.lz
lzsa -f1 -r "$2"_plane_3 "$2"_plane_3.lz
cat "$2"_plane_0.lz "$2"_plane_1.lz "$2"_plane_2.lz "$2"_plane_3.lz > "$2".lz1

echo "File size of ${2}_plane_0.lz: $(wc -c < "${2}_plane_0.lz") bytes"
echo "File size of ${2}_plane_1.lz: $(wc -c < "${2}_plane_1.lz") bytes"
echo "File size of ${2}_plane_2.lz: $(wc -c < "${2}_plane_2.lz") bytes"
echo "File size of ${2}_plane_3.lz: $(wc -c < "${2}_plane_3.lz") bytes"

./convert_tool.elf "$2".lz1 "$2".SIF 720 348 720 348 $(wc -c < "${2}_plane_0.lz") $(wc -c < "${2}_plane_1.lz") $(wc -c < "${2}_plane_2.lz") $(wc -c < "${2}_plane_3.lz")

rm *_plane_* *.lz1 "$2"
