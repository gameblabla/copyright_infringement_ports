rm *.wav *.srr *.ZX7
python png2spr.py bakura_main.png
python png2spr.py TITLE_SORCERER.png
python convert2bin.py frame1.png frame1.bin
python convert2bin.py frame2.png frame2.bin
python convert2bin.py frame3.png frame3.bin
python convert2bin.py frame4.png frame4.bin
python convert2bin.py frame5.png frame5.bin
z88dk-zx7 frame1.bin FRAME1.ZX7
z88dk-zx7 frame2.bin FRAME2.ZX7
z88dk-zx7 frame3.bin FRAME3.ZX7
z88dk-zx7 frame4.bin FRAME4.ZX7
z88dk-zx7 frame5.bin FRAME5.ZX7
zcc +srr -vn -compiler=sdcc -startup=31 -SO3 --opt-code-speed=all -Cc-unsigned --max-allocs-per-node200000 -create-app main.c data.asm  -Cz--audio -pragma-define:CRT_INITIALIZE_BSS=0 -pragma-define:CLIB_EXIT_STACK_SIZE=0 -pragma-define:CRT_ENABLE_STDIO=0 \
-pragma-define:CLIB_MALLOC_HEAP_SIZE=0 -pragma-define:CLIB_FOPEN_MAX=0 -pragma-define:CLIB_OPEN_MAX=0 -pragma-define:CRT_ENABLE_RESTART=0 
mame sorcererd -cassette1 $PWD/a.wav  -autoboot_delay 2 -autoboot_command "LO\nGO 0100\n" -window
