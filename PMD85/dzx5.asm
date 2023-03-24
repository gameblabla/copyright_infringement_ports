; -----------------------------------------------------------------------------
; ZX5v2 8080 decoder by Ivan Gorodetsky
; Based on ZX5 z80 decoder by Einar Saukas
; v1 (2021-10-05) - 150 bytes code + 5 bytes variables
; v2 (2022-05-07) - 147 bytes code + 4 bytes variables
; -----------------------------------------------------------------------------
; Parameters (forward):
;   HL: source address (compressed data)
;   BC: destination address (decompressing)
;
; Parameters (backward):
;   HL: last source address (compressed data)
;   BC: last destination address (decompressing)
; -----------------------------------------------------------------------------

;#define BACKWARD


PUBLIC _dzx5
GLOBAL _dzx5

_dzx5:
        lxi d,0FFFFh
		push d
		inx d
		mvi a,080h
dzx5_literals:
		call dzx5_elias
		call ldir
		jc dzx5_other_offset
dzx5_last_offset:
		call dzx5_elias
dzx5_copy:
		xthl
		push h
		dad b
		call ldir
		pop h
		xthl
		jnc dzx5_literals
dzx5_other_offset:
		add a
		jnz dzx5_other_offset_skip
		mov a,m
		inx h
		ral
dzx5_other_offset_skip:
		jnc dzx5_prev_offset 
dzx5_new_offset:
		xthl
		push h
		lhld dzx5_offset2
		shld dzx5_offset3
		pop h
		shld dzx5_offset2
		mov h,a
		add a
		xthl
		mvi e,0FEh
		call dzx5_elias_loop
		inr e
		jz dzx5_elias_exit
		mov d,e
		mov e,m
		inx h 
		xchg
		xthl
		dcr h
		xchg
		lxi d,1
		cp dzx5_elias_backtrack
		inx d
		jmp dzx5_copy
dzx5_prev_offset:
		add a
		xthl
		push h
		xchg
		xthl
		push h
		lhld dzx5_offset3
		xchg
		lhld dzx5_offset2
		jnc dzx5_second_offset
		xchg
dzx5_second_offset:
		xthl
		shld dzx5_offset2
		xchg
		shld dzx5_offset3
		pop h
		xthl
		xchg
		pop h
		xthl
        jmp dzx5_last_offset

dzx5_elias:
		inr e
dzx5_elias_loop:	
		add a
		jnz dzx5_elias_skip
		mov a,m
		inx h
		ral
dzx5_elias_skip:
		rc
dzx5_elias_backtrack:
		xchg\ dad h\ xchg
		add a
		jnc dzx5_elias_loop
		jmp dzx5_elias
		
ldir:
		push psw						
ldir_loop:
		mov a,m
		stax b
		inx h
		inx b
		dcx d
		mov a,d
		ora e
		jnz ldir_loop
dzx5_elias_exit:
		pop psw
		add a
		ret

dzx5_offset2:
		defw 0
dzx5_offset3:
		defw 0

		.end
