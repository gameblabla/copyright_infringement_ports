

.section .prg_rom_24.dpcm,"aR",@progbits
  .align 64
  .incbin "dialogue6.dmc"
  
.section .prg_rom_24.dialogue6_data,"a",@progbits
.globl dialogue6_data
dialogue6_data:
  .include "dialogue6_data.s"
