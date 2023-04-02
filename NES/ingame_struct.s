

.section .prg_rom_28.dpcm,"aR",@progbits
  .align 64
  .incbin "ingame.dmc"
  
.section .prg_rom_28.dialogue6_data,"a",@progbits
.globl ingame_data
ingame_data:
  .include "ingame_data.s"
