#!/bin/sh
rm a_DATA.bin a_BSS.bin a_BANK_7.bin a.bin a.rom *.bin
wine zcc +zx -vn -O2 --opt-code-speed=all -Cc-unsigned -compiler=sccz80 --generic-console -create-app main.c data.asm lib/dzx1_mega.asm lib/dzx1_mega_callee.asm lib/dzx1_agile_rcs.asm lib/dzx1_agile_rcs_callee.asm 

#zcc +zx -vn -c effects.asm sham.mus.asm sound.c --codesegBANK_02 --constsegBANK_02 @zproject-07.lst
#zcc +zx -vn -c main.c data.asm lib/dzx1_mega.asm lib/dzx1_mega_callee.asm lib/dzx1_agile_rcs.asm lib/dzx1_agile_rcs_callee.asm  @zproject-m.lst
#zcc +zx -vn -subtype=sna  -o name @zproject.lst -create-app
