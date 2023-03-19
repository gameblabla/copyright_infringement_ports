	.text
	.even
	.extern	_WAITVBLANK
;	The interrupt code is already disabled
;	game code wise. Uncomment them if you need them
_WAITVBLANK:
	;or.w		#0x0700,sr		;Disable interrupts, except NMI
	move.b	#255,$18021    		;Clear any current interrupt bits
_WAITVBLANKAGAIN:
	move.b	$18021,d1			;Read in vblank state
	tst.b		d1				;Wait for an interrupt
	beq		_WAITVBLANKAGAIN	;Vblank hasn't occurred yet
	;and.w	#0xF8FF,sr			;Enable interrupts again
	rts						;Return to caller

