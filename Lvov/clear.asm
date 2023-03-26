SECTION code_8000

PUBLIC _lviv_clear_video_ram
_lviv_clear_video_ram:

    ; Enable video RAM
    ld  a, $fd
    out ($c2), a

    ; Clear video RAM
    ld  hl, $4000
    ld  de, $4001
    ld  bc, $3FFF
    ld  (hl), $00
    ldir

    ; Disable video RAM
    ld  a, $ff
    out ($c2), a

    ret
