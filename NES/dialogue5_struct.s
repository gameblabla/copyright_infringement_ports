

.section .prg_rom_22.dpcm,"aR",@progbits
  .align 64
  .incbin "dialogue5c.dmc"
  
.section .prg_rom_22.dialogue5_data,"a",@progbits
.globl dialogue5_data
dialogue5_data:
  .include "dialogue5_data.s"
