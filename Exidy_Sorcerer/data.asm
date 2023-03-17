;; data.asm

SECTION rodata_user

PUBLIC _bakura_spr
PUBLIC _title_spr
PUBLIC _frame1zx7
PUBLIC _frame2zx7
PUBLIC _frame3zx7
PUBLIC _frame4zx7
PUBLIC _frame5zx7

_bakura_spr:
   BINARY "bakura_main.bin"
  
_title_spr:
	BINARY "TITLE_SORCERER.bin"
	
_frame1zx7:
	BINARY "FRAME1.ZX7"

_frame2zx7:
	BINARY "FRAME2.ZX7"

_frame3zx7:
	BINARY "FRAME3.ZX7"

_frame4zx7:
	BINARY "FRAME4.ZX7"

_frame5zx7:
	BINARY "FRAME5.ZX7"
