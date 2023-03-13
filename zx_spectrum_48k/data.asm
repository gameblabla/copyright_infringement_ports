;; data.asm

SECTION rodata_user

PUBLIC _rikuto
PUBLIC _titleimg
PUBLIC _frame1
PUBLIC _frame2
PUBLIC _frame3
PUBLIC _frame4
PUBLIC _frame5
PUBLIC _frame1_clr
PUBLIC _frame2_clr
PUBLIC _frame3_clr
PUBLIC _frame4_clr
PUBLIC _frame5_clr

_rikuto:
   BINARY "rikuto.scr.rcs.zx1"

_titleimg:
   BINARY "title.scr.rcs.zx1"

_frame1:
   BINARY "frame1.scr.zx1"

_frame2:
   BINARY "frame2.scr.zx1"

_frame3:
   BINARY "frame3.scr.zx1"

_frame4:
   BINARY "frame4.scr.zx1"

_frame5:
   BINARY "frame5.scr.zx1"


_frame1_clr:
   BINARY "frame1clr.scr.zx1"
_frame2_clr:
   BINARY "frame2clr.scr.zx1"
_frame3_clr:
   BINARY "frame3clr.scr.zx1"
_frame4_clr:
   BINARY "frame4clr.scr.zx1"
_frame5_clr:
   BINARY "frame5clr.scr.zx1"
