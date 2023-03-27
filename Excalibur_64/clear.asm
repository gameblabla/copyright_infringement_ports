	PUBLIC _Copy_Buffer
	SECTION		code_graphics
	defc		DISPLAY = $2000
	defc		__console_w = 80
	defc 		CONSOLE_COLUMNS = 80
	defc 		CONSOLE_ROWS = 24
	defc		COLOUR_MAP = $2800
	defc		HIRES_MAP = $3000
	.__excali64_attr	defb	$55	; White on Black
	EXTERN _buffer_tocopy
_Copy_Buffer:
	in	a,($50)
	bit	4,a
	jr	z,_Copy_Buffer
	push	af
	res	1,a
	set	0,a
	out	($70),a
	
    ld    hl, _buffer_tocopy
    ld    de, COLOUR_MAP
    ld    bc, (CONSOLE_COLUMNS * CONSOLE_ROWS) - 1
    ldir

	pop	af
	out	($70),a
	
	ret

PUBLIC _buffer_tocopy

._buffer_tocopy
	defs 1920
