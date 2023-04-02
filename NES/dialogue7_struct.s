

.section .prg_rom_26.dpcm,"aR",@progbits
  .align 64
  .incbin "dialogue7.dmc"
  
.section .prg_rom_26.dialogue7_data,"a",@progbits
.globl dialogue7_data
dialogue7_data:
  .include "dialogue7_data.s"
