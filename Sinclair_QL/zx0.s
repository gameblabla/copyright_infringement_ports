.extern _zx0_decompress

_zx0_decompress:
	movem.l 4(a7),a0-a1
	movem.l a2/d2,-(sp)
	moveq #-128,d1
	moveq #-1,d2
.literals:
	bsr .get_elias
	subq.l #1,d0
.copy_lits:
	move.b (a0)+,(a1)+
	dbf d0,.copy_lits
	add.b d1,d1
	bcs .get_offset
.rep_match:
	bsr .get_elias
.do_copy:
	subq.l #1,d0
.do_copy_offs:
	move.l a1,a2
	add.l d2,a2
.copy_match:
	move.b (a2)+,(a1)+
	dbf d0,.copy_match
	add.b d1,d1
	bcc .literals
.get_offset:
	moveq #-2,d0
	bsr .elias_loop
	addq.b #1,d0
	beq .done
	move.w d0,d2
	lsl.w #8,d2
	moveq #1,d0
	move.b (a0)+,d2
	asr.l #1,d2
	bcs .do_copy_offs
	bsr .elias_bt
	bra .do_copy_offs
.get_elias:
	moveq #1,d0
.elias_loop:
	add.b d1,d1
	bne .got_bit
	move.b (a0)+,d1
	addx.b d1,d1

.got_bit:
	bcs .got_elias
.elias_bt:
	add.b d1,d1
	addx.l d0,d0
	bra .elias_loop

.done:
	movem.l (sp)+,a2/d2
.got_elias:
	rts
