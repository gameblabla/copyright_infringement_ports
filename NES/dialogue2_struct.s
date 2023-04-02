

.section .prg_rom_16.dpcm,"aR",@progbits
  .align 64
  .incbin "dialogue2c.dmc"
  
.section .prg_rom_16.dialogue2_data,"a",@progbits
.globl dialogue2_data
dialogue2_data:
  .include "dialogue2_data.s"
