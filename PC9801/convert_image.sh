#!/bin/sh
python convert_to_pc98.py "$1" TEMP.BIN
./crop.sh TEMP.BIN
lzsa -r -f1 TEMP.BIN_part1 T1.LZ
lzsa -r -f1 TEMP.BIN_part2 T2.LZ
lzsa -r -f1 TEMP.BIN_part3 T3.LZ
lzsa -r -f1 TEMP.BIN_part4 T4.LZ

echo "The size of T1 $(stat -c%s "T1.LZ") bytes"
echo "The size of T2 $(stat -c%s "T2.LZ")  bytes"
echo "The size of T3 $(stat -c%s "T3.LZ") bytes"
echo "The size of T4 $(stat -c%s "T4.LZ") bytes"

cat T1.LZ T2.LZ T3.LZ T4.LZ > "$2".BIN
rm T1.LZ T2.LZ T3.LZ T4.LZ TEMP.BIN TEMP.BIN_part4 TEMP.BIN_part3 TEMP.BIN_part2 TEMP.BIN_part1
