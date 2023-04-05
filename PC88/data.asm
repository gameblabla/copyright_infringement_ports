;; data.asm

SECTION rodata_user

PUBLIC _frame1_b
PUBLIC _frame1_r
PUBLIC _frame1_g

PUBLIC _title_b
PUBLIC _title_g
PUBLIC _title_r

PUBLIC _anim1_g
PUBLIC _anim1_r

PUBLIC _anim2_g
PUBLIC _anim2_r

PUBLIC _anim3_g
PUBLIC _anim3_r

PUBLIC _anim4_g
PUBLIC _anim4_r

_title_b:
   BINARY "titlescreen_blue.zx1"
_title_r:
   BINARY "titlescreen_red.zx1"
_title_g:
   BINARY "titlescreen_green.zx1"

_frame1_b:
   BINARY "bakurapc88_blue.zx1"
_frame1_r:
   BINARY "bakurapc88_red.zx1"
_frame1_g:
   BINARY "bakurapc88_green.zx1"

_anim1_r:
   BINARY "frame1_red.lz"
_anim1_g:
   BINARY "frame1_green.lz"
   

_anim2_r:
   BINARY "frame2_red.lz"
_anim2_g:
   BINARY "frame2_green.lz"
   
_anim3_r:
   BINARY "frame3_red.lz"
_anim3_g:
   BINARY "frame3_green.lz"
   
_anim4_r:
   BINARY "frame4_red.lz"
_anim4_g:
   BINARY "frame4_green.lz"
