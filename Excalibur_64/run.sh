rm *.img *.com *.COM main_HIMEM.bin main F1.BIN F2.BIN F3.BIN F4.BIN F5.BIN FRAMES.BIN

python convert2bin.py F1.png F1.BIN
python convert2bin.py F2.png F2.BIN
python convert2bin.py F3.png F3.BIN
python convert2bin.py F4.png F4.BIN
python convert2bin.py F5.png F5.BIN

cat F1.BIN F2.BIN F3.BIN F4.BIN F5.BIN > FRAMES.BIN

zcc +cpm -SO3 -subtype=excali64 -o main  \
main.c clear.asm \
-create-app \
-Cz--add-file=BAKURA.BIN -Cz--add-file=FRAMES.BIN \
-pragma-define:CRT_INITIALIZE_BSS=0 -pragma-define:CLIB_EXIT_STACK_SIZE=0  -pragma-define:CLIB_CONIO_NATIVE_COLOUR=1 -pragma-redirect:fputc_cons=fputc_cons_generic -D__EXCALIBUR64__ 

# Done compiling, running game on MAME
#-autoboot_delay 4 -autoboot_command "1:APP.COM\n"
mame excali64 -window -uimodekey DEL -flop1 "$PWD/excalibur_cpm_system.raw" -flop2 "$PWD/main.img" -autoboot_delay 3 -autoboot_command "B:MAIN\n" 
