INCLUDE link.ld
SECTIONS {
    .ft2 : {
        *libfamitone2.a:*(.text .text.* .rodata .rodata.*)
    } >prg_rom_14_vma AT>prg_rom_14_lma
}
