.global Draw_firstframe
.global Draw_firstframe_color
.global Draw_frame2
.global Draw_frame3
.global Draw_frame4
.global Draw_Bakura

baku_bin: .incbin "bakura_bitmapc.bin"
baku_c: .incbin "bakura_c.bin"

source: .incbin "frame1_bitmap.bin"
source1: .incbin "frame1_color.bin"
source2: .incbin "frame2_bitmap.bin"
source3: .incbin "frame3_bitmap.bin"
source4: .incbin "frame4_bitmap.bin"
;source2c: .incbin "frame2_color.bin"
;source3c: .incbin "frame3_color.bin"
;source4c: .incbin "frame4_color.bin"

len			= 6400
len1		= 1600

from     = $fb
to       = $fd
tmpx     = $a6

Draw_Bakura:

         lda #<baku_c
         sta from
         lda #>baku_c
         sta from+1
         lda #<$c000
         sta to
         lda #>$c000
         sta to+1

         ldy #0
         ldx #>len1
		 jsr fast_memcpy
		 
         lda #<baku_bin
         sta from
         lda #>baku_bin
         sta from+1
         lda #<$e000
         sta to
         lda #>$e000
         sta to+1
         ldy #0
         ldx #>len
		 jsr fast_memcpy
		 
		 rts

Draw_firstframe_color:

         lda #<source1
         sta from
         lda #>source1
         sta from+1
         lda #<$c000
         sta to
         lda #>$c000
         sta to+1

         ldy #0
         ldx #>len1
		 jsr fast_memcpy
		 
		 rts
		 
Draw_firstframe:

		 

         lda #<source
         sta from
         lda #>source
         sta from+1
         lda #<$e000
         sta to
         lda #>$e000
         sta to+1
         ldy #0
         ldx #>len
		 jsr fast_memcpy
		 

		 rts

Draw_frame2:


         lda #<source2
         sta from
         lda #>source2
         sta from+1

         lda #<$e000
         sta to
         lda #>$e000
         sta to+1
         ldy #0
         ldx #>len
		 jsr fast_memcpy

		 
		 rts
		 
Draw_frame3:


         lda #<source3
         sta from
         lda #>source3
         sta from+1

         lda #<$e000
         sta to
         lda #>$e000
         sta to+1
         ldy #0
         ldx #>len
		 jsr fast_memcpy
		 

		 rts
		 
Draw_frame4:

         lda #<source4
         sta from
         lda #>source4
         sta from+1

         lda #<$e000
         sta to
         lda #>$e000
         sta to+1
         ldy #0
         ldx #>len
		 jsr fast_memcpy
		 

		 
		 rts
		 
		 
fast_memcpy:

         beq remain
next:     lda (from),y
         sta (to),y
         iny
         bne next
         inc from+1
         inc to+1
         dex
         bne next
remain:   ldx #<len      
         beq done
nextr:    lda (from),y
         sta (to),y
         ;iny
         dex
         bne nextr
done:     rts
