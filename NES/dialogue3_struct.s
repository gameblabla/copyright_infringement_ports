

.section .prg_rom_18.dpcm,"aR",@progbits
  .align 64
  .incbin "dialogue3c.dmc"
  
.section .prg_rom_18.dialogue3_data,"a",@progbits
.globl dialogue3_data
dialogue3_data:
  .include "dialogue3_data.s"
