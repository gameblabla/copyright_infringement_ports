

.section .prg_rom_30.dpcm,"aR",@progbits
  .align 64
  .incbin "results.dmc"
  
.section .prg_rom_30.results_data,"a",@progbits
.globl results_data
results_data:
  .include "results_data.s"
