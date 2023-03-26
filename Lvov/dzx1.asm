; -----------------------------------------------------------------------------
; ZX1 8080 decoder by Ivan Gorodetsky
; Based on ZX1 z80 decoder by Einar Saukas
; Compile with The Telemark Assembler (TASM) 3.2
; v1 (2021-02-16) - 131 bytes forward / 135 bytes backward
; v2 (2021-02-17) - 129 bytes forward / 133 bytes backward
; v3 (2021-02-22) - 124 bytes forward / 128 bytes backward
; v4 (2021-02-25) - 124 bytes forward / 128 bytes backward (faster version)
; v5 (2021-02-25) - 128 bytes forward / 132 bytes backward (+4 bytes, bug fix)
; -----------------------------------------------------------------------------
; Parameters (forward):
;   DE: source address (compressed data)
;   BC: destination address (decompressing)
;
; Parameters (backward):
;   DE: last source address (compressed data)
;   BC: last destination address (decompressing)
; -----------------------------------------------------------------------------

PUBLIC _dzx1_decompress

_dzx1_decompress:
		lxi h,0FFFFh
		shld Offset
		mvi a,080h
cdzx1s_literals:
		call cdzx1s_elias
		push psw
		dcx h
		inr l
cdzx1s_ldir1:
		ldax d
		stax b
		ld a, $FF
		out ($C2), a ; Disable video RAM mode to access the source data
        inx d
		ld a, $FD
		out ($C2), a ; Enable video RAM mode to access the destination address
        inx b
		dcr l
		jnz cdzx1s_ldir1
		xra a
		ora h
		jz $+7
		dcr h
		jmp cdzx1s_ldir1
		pop psw
		add a
		jc cdzx1s_new_offset
		call cdzx1s_elias
cdzx1s_copy:
		push d
		xchg
		lhld Offset
		dad b
		push psw
		dcx d
		inr e
cdzx1s_ldir2:
		mov a,m
		stax b
        inx h
        inx b
		dcr e
		jnz cdzx1s_ldir2
		xra a
		ora d
		jz $+7
		dcr d
		jmp cdzx1s_ldir2
		pop psw
		xchg
		pop d
		add a
		jnc cdzx1s_literals
cdzx1s_new_offset:
		dcr h
		push psw
		ldax d
        inx d
		rar
        mov l,a
		jnc cdzx1s_msb_skip
		ldax d
        inx d
		rar
        inr a
		jz cdzx1s_exit
		mov h,a
		mov a,l
        ral 
        mov l,a
cdzx1s_msb_skip:
		pop psw
		shld Offset
		call cdzx1s_elias
		inx h
		jmp cdzx1s_copy
cdzx1s_elias:
		lxi h,1
cdzx1s_elias_loop:	
		add a
		rnc
		jnz cdzx1s_elias_skip
		ldax d
        inx d
		ral
		rnc
cdzx1s_elias_skip:
		dad h
		add a
		jnc cdzx1s_elias_loop
		inr l
		jmp cdzx1s_elias_loop
cdzx1s_exit:
		pop psw
		ret

Offset:
		defw 0
		
		
