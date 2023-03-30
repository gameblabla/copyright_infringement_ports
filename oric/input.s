.export _getkey
.export _setup
.export _hires

hires_atmos   = $ec33
hires_oric1   = $e9bb

getkey_atmos  = $eb78
getkey_oric1  = $e905

irq           = $fffe

_setup:
              lda   irq
              cmp   #$28
              beq   isOric1

isAtmos:      lda   #<hires_atmos
              sta   _hires+1
              lda   #>hires_atmos
              sta   _hires+2
              lda   #<getkey_atmos
              sta   _getkey+1
              lda   #>getkey_atmos
              sta   _getkey+2
              jmp   _hires

isOric1:      lda   #<hires_oric1
              sta   _hires+1
              lda   #>hires_oric1
              sta   _hires+2
              lda   #<getkey_oric1
              sta   _getkey+1
              lda   #>getkey_oric1
              sta   _getkey+2
              
_getkey:      jsr   $f00d
              and   #$7f
              beq   test
              cmp   #$0d
test:
              ldx   #$00
              rts

_hires:       jsr   $f00d
              lda   #$08
              sta   $26a
              rts

