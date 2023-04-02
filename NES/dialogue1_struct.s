

.section .prg_rom_14.dpcm,"aR",@progbits
  .align 64
  .incbin "dialogue1.dmc"
  
.section .prg_rom_14.dialogue1_data,"a",@progbits
.globl dialogue1_data
dialogue1_data:
  .include "dialogue1_data.s"
