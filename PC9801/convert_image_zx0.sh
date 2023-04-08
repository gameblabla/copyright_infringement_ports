#!/bin/sh
python convert_to_pc98.py "$1" TEMP.BIN
./crop.sh TEMP.BIN
zx0 TEMP.BIN_part1 T1.ZX0
zx0 TEMP.BIN_part2 T2.ZX0
zx0 TEMP.BIN_part3 T3.ZX0
zx0 TEMP.BIN_part4 T4.ZX0

echo "The size of T1 $(stat -c%s "T1.ZX0") bytes"
echo "The size of T2 $(stat -c%s "T2.ZX0")  bytes"
echo "The size of T3 $(stat -c%s "T3.ZX0") bytes"
echo "The size of T4 $(stat -c%s "T4.ZX0") bytes"

cat T1.ZX0 T2.ZX0 T3.ZX0 T4.ZX0 > "$2".ZX0
rm T1.ZX0 T2.ZX0 T3.ZX0 T4.ZX0 TEMP.BIN TEMP.BIN_part4 TEMP.BIN_part3 TEMP.BIN_part2 TEMP.BIN_part1
