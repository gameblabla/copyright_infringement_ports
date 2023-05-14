	.text

	.globl _set_dma_sound
	.globl _stop_dma_sound
	.globl _set_dma_sound_noloop

_set_dma_sound:
	movem.l	d5-d6,-(sp)
	move.l	16(sp),d6
	move.l	12(sp),d5
	add.l	d5,d6

	pea		setdma(pc)
	move	#38,-(sp)
	trap	#14
	addq.l	#6,sp

	movem.l	(sp)+,d5-d6
	rts

_set_dma_sound_noloop:
	movem.l	d5-d6,-(sp)
	move.l	16(sp),d6
	move.l	12(sp),d5
	add.l	d5,d6

	pea		setdma_noloop(pc)
	move	#38,-(sp)
	trap	#14
	addq.l	#6,sp

	movem.l	(sp)+,d5-d6
	rts

_stop_dma_sound:
	clr.b	0xFFFF8901
	rts

setdmaaddrsub:
	swap	d1
	move.b	d1,1(a0)
	swap	d1
	clr.l	d2
	move.b	d1,d2
	lsr.w	#8,d1
	move.b	d1,3(a0)
	move.b	d2,5(a0)
	rts

setdma:
	clr.b	0xFFFF8901.w
	move.l	d5,d1
	lea		0xFFFF8902.w,a0
	bsr.s	setdmaaddrsub

	move.l   d6,d1
	lea		12(a0),a0
	bsr.s	setdmaaddrsub


	moveq.l	#1,d0
	ori.b	#0x80,d0
	move.b	d0,0xFFFF8921.w

	move.b  #3,0xFFFF8901.w
	rts


setdma_noloop:
	clr.b	0xFFFF8901.w
	move.l	d5,d1
	lea		0xFFFF8902.w,a0
	bsr.s	setdmaaddrsub

	move.l   d6,d1
	lea		12(a0),a0
	bsr.s	setdmaaddrsub


	moveq.l	#1,d0
	ori.b	#0x80,d0
	move.b	d0,0xFFFF8921.w

	move.b  #1,0xFFFF8901.w
	rts
