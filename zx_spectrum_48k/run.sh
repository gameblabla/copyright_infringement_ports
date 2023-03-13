rm a_DATA.bin a_BSS.bin a_BANK_7.bin a.bin a.rom
zcc  +zx -subtype=rom -create-app main.c data.asm dzx0_smartRCS.asm text.c
rm a_DATA.bin a_BSS.bin a_BANK_7.bin a.bin
