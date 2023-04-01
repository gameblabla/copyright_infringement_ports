rm *.elf *.nes *.mlb
mos-nes-mmc3-clang -O0 -flto -Tmainscript.ld main.c chr-rom.s music.s -o main.nes -lneslib -lnesdoug -lfamitone2 -flto
