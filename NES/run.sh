rm *.elf *.nes *.mlb
mos-nes-mmc3-clang -O0 -flto -Tmainscript.ld main.c chr-rom.s \
dialogue1_struct.s dialogue2_struct.s dialogue3_struct.s dialogue4_struct.s \
dialogue5_struct.s dialogue6_struct.s dialogue7_struct.s ingame_struct.s  results_struct.s \
 -o main.nes -lneslib -lnesdoug -lfamitone2 -flto
