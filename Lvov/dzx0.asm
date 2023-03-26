
PUBLIC _dzx0_decompress

_dzx0_decompress:
		lxi h,0FFFFh
		push h
		inx h
		mvi a,080h
dzx0_literals:
		call dzx0_elias
		call dzx0_ldir
		jc dzx0_new_offset
		call dzx0_elias
dzx0_copy:
		xchg
		xthl
		push h
		dad b
		xchg
		call dzx0_ldir
		xchg
		pop h
		xthl
		xchg
		jnc dzx0_literals
dzx0_new_offset:
		call dzx0_elias

		mov h,a
		pop psw
		xra a
		sub l
		rz
		push h
		rar\ mov h,a
		ldax d
		rar\ mov l,a
		inx d

		xthl
		mov a,h
		lxi h,1
		cnc dzx0_elias_backtrack

		inx h
		jmp dzx0_copy
dzx0_elias:
		inr l
dzx0_elias_loop:	
		add a
		jnz dzx0_elias_skip
		ldax d
		inx d
		ral
dzx0_elias_skip:
		rc
dzx0_elias_backtrack:
		dad h
		add a
		jnc dzx0_elias_loop
		jmp dzx0_elias

dzx0_ldir:
		push psw
dzx0_ldir1:
		ldax d
		stax b
		ld a, $FF
		out ($C2), a ; Disable video RAM mode to access the source data
		inx d
		ld a, $FD
		out ($C2), a ; Enable video RAM mode to access the destination address
		inx b
		dcx h
		mov a,h
		ora l
		jnz dzx0_ldir1
		pop psw
		add a
		ret
