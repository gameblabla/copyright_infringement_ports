


PUBLIC _clear_plane

defc ALU_MODE_CTRL = 0x32
defc EXPANDED_ALU_CTRL = 0x34
defc EXPANDED_GVRAM_CTRL = 0x35
defc GVRAM_SEL_0 = 0x5c
defc GVRAM_SEL_1 = 0x5d
defc GVRAM_SEL_2 = 0x5e
defc MAINRAM_SEL = 0x5f

_ENABLE_GRAM:

	ret
	
_DISABLE_GRAM:

	ret

_clear_plane:
	ld	hl,49152 ; 0xC000
	ld	de,49153 ; 0xC001
	
	ld	bc,15999	;80x200 - 1
	ld	(hl),a
	ldir
	ret
