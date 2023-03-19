#!/bin/bash

# Check that the target file name is provided as a parameter
if [ $# -ne 1 ]; then
    echo "Usage: $0 <target>"
    exit 1
fi

# Files to add to floppy disk images/folder etc...
FILES_TO_ADD=("titlelz4w baklz4w frame1 frame2 frame3 frame4 frame5")
#Default device to load files from
DEFAULT_DEVICE="flp1"

# Get the target file name from the command line parameter
TARGET=$1

rm -f boot $TARGET
rm -f *.o

# Find all .s files in the current directory and compile them to .o files
for f in *.s; do
  as68 "$f" "${f%.s}.o"
done

echo "Device to load from : "$DEFAULT_DEVICE

# Compile main.c and inline all .o files
OUTPUT=$(qdos-gcc -Os -fomit-frame-pointer -nostartfiles -ffixed-a6 -DDEVICE_LOADFROM=\"$DEFAULT_DEVICE\" -o $TARGET -Wl,-ms -Wl,-screspr.o main.c $(find . -maxdepth 1 -name "*.o") 2>&1)

if ! test -f "$TARGET"; then
    echo $OUTPUT
    echo "Error: target file '$TARGET' not found"
    exit 1
fi

echo $OUTPUT

echo "Patching executable to work without EXEC"
printf "+%d" 0x`tail -c 4 $TARGET | xxd -l 32 -p|sed 's/^0*//'` | xargs -I X truncate -s X $TARGET

# Extract the data space size from the qcc output
DATASPACE=$(echo "$OUTPUT" | grep -oP '(?<=dataspace )[0-9a-fA-F]+')

# Get the size of the file "MAIN"
filesize=$(stat -c %s ${TARGET})

echo "30 mem=RESPR($filesize)" >> boot
echo "40 LBYTES \"${DEFAULT_DEVICE}_${TARGET}\",mem" >> boot
echo "50 CALL mem" >> boot


# Create a "GAME.QCF" file with the appropriate QPC2 configuration
echo "Ram=128K" > GAME.QCF
echo "MainRom=QL ROMs\QL_ROM_JS" >> GAME.QCF
echo "BackRomActive=No" >> GAME.QCF
echo "ExpRomActive=No" >> GAME.QCF
echo "UseFloppyName=Yes" >> GAME.QCF
echo "FloppyName=flp" >> GAME.QCF
echo "UseHardDiskName=No" >> GAME.QCF
echo "HardDiskName=win" >> GAME.QCF
echo "HasRamDisk=Yes" >> GAME.QCF
echo "RamDiskName=RAM" >> GAME.QCF
echo "HasParPort=No" >> GAME.QCF
echo "Subdirs=Off" >> GAME.QCF
echo "Speed=QL" >> GAME.QCF
echo "FastStartup=Yes" >> GAME.QCF
echo "AutoStartSession=Yes" >> GAME.QCF
echo "FirstKey=None" >> GAME.QCF
echo "AcceleratedGraphics=Yes" >> GAME.QCF
echo "Sound=On" >> GAME.QCF
echo "Slot1=PAK:" >> GAME.QCF
echo "Slot2=Empty" >> GAME.QCF
echo "Slot3=Empty" >> GAME.QCF
echo "Slot4=Empty" >> GAME.QCF
echo "Slot5=Empty" >> GAME.QCF
echo "Slot6=Empty" >> GAME.QCF
echo "Slot7=Empty" >> GAME.QCF
echo "Slot8=Empty" >> GAME.QCF
echo "PakDir1=PAK:" >> GAME.QCF
echo "WindowHeight=376" >> GAME.QCF
echo "PakDir1=" >> GAME.QCF

# Create a ZIP archive containing the game files and QCF file
qlzip -r GAME.qlpak GAME.QCF boot $TARGET $FILES_TO_ADD
#qlzip -r GAME.zip boot MAIN

rm -r GAME
mkdir GAME

cp $TARGET GAME/$TARGET
cp boot GAME/
cp $FILES_TO_ADD GAME/

rm game.img
qltools game.img -fdd GAME
qltools game.img -w boot $FILES_TO_ADD
qltools game.img -W $TARGET
qltools game.img -x $TARGET $DATASPACE
qltools game.img -s

# Remove the temporary game directory
rm -rf GAME.QCF
rm -f *.o $TARGET boot BOOT
