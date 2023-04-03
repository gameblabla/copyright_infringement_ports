#!/bin/sh
head -c 8000 "$1" > "$2"_bitmap.bin
tail -c +8001 "$1" > "$2"_color.bin
