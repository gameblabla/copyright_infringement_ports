#!/bin/sh
head -c 8000 "$1" > temp_bitmap.bin
tail -c +8001 "$1" > temp_color.bin

dd if=temp_bitmap.bin of="$2"_bitmap.bin bs=1 count=6400
dd if=temp_color.bin of="$2"_color.bin bs=1 count=1600
