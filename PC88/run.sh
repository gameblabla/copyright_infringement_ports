#!/bin/sh
rm *.d88
python main.py bakurapc88.png
python main.py frame1.png
python main.py frame2.png
python main.py frame3.png
python main.py frame4.png
python main.py titlescreen.png

#zx1 frame1_red.bin frame1_red.zx1
#zx1 frame1_green.bin frame1_green.zx1
#zx1 frame2_red.bin frame2_red.zx1
#zx1 frame2_green.bin frame2_green.zx1
#zx1 frame3_red.bin frame3_red.zx1
#zx1 frame3_green.bin frame3_green.zx1
#zx1 frame4_red.bin frame4_red.zx1
#zx1 frame4_green.bin frame4_green.zx1

zx1 titlescreen_blue.bin titlescreen_blue.zx1
zx1 titlescreen_red.bin titlescreen_red.zx1
zx1 titlescreen_green.bin titlescreen_green.zx1

zx1 bakurapc88_blue.bin bakurapc88_blue.zx1
zx1 bakurapc88_red.bin bakurapc88_red.zx1
zx1 bakurapc88_green.bin bakurapc88_green.zx1

./lz48.elf -i frame1_red.bin -o frame1_red.lz
./lz48.elf -i frame1_green.bin -o frame1_green.lz

./lz48.elf -i frame2_red.bin -o frame2_red.lz
./lz48.elf -i frame2_green.bin -o frame2_green.lz

./lz48.elf -i frame3_red.bin -o frame3_red.lz
./lz48.elf -i frame3_green.bin -o frame3_green.lz

./lz48.elf -i frame4_red.bin -o frame4_red.lz
./lz48.elf -i frame4_green.bin -o frame4_green.lz

zcc -compiler=sdcc -SO3 --max-allocs-per-node200000 +pc88 -subtype=disk main.c data.asm draw.s dzx1.asm lz48.asm -create-app -pragma-define:CLIB_PC8800_V2_ENABLED=1 -pragma-define:CRT_ENABLE_STDIO=0 -pragma-define:CLIB_EXIT_STACK_SIZE=0 -o copyright_infringement
zcc -compiler=sdcc -SO3 --max-allocs-per-node200000 +pc88 -Cz--audio main.c data.asm draw.s dzx1.asm lz48.asm -create-app -pragma-define:CLIB_PC8800_V2_ENABLED=1 -pragma-define:CRT_ENABLE_STDIO=0 -pragma-define:CLIB_EXIT_STACK_SIZE=0 -o copyright_infringement
