INCLUDE common.ld
INCLUDE c-in-ram.ld
INCLUDE prg-rom-banked-mode-1.ld

SECTIONS {
  PROVIDE(__dpcm_offset = 0);
}

SECTIONS {
    .ft2 : {
        *libfamitone2.a:*(.text .text.* .rodata .rodata.*)
    } >prg_rom_6_vma AT>prg_rom_6_lma
}
