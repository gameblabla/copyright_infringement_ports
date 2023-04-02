

.section .prg_rom_20.dpcm,"aR",@progbits
  .align 64
  .incbin "dialogue4.dmc"
  
.section .prg_rom_20.dialogue4_data,"a",@progbits
.globl dialogue4_data
dialogue4_data:
  .include "dialogue4_data.s"
