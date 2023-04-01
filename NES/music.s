

.section .dpcm,"a",@progbits
  .incbin "mygame.dmc"
  
.section .prg_rom_14.music_data,"a",@progbits
.globl music_data
music_data:
  .include "dmusic.s"

.section .prg_rom_14.sounds_data,"a",@progbits
.globl sounds_data
sounds_data:
