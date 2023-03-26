PUBLIC _sound_play
PUBLIC _confirm_sound_play
PUBLIC _fuck_sound_play

_sound_play:
    lxi h, 512 ; Load immediate value 100 into register pair HL
    mvi d, 0


sound_loop:
    in a, ($c2) ; Read the current value of Port C ($c2)
    xor a, d ; Toggle bit 0 (Tape Out) of Port C (00000001 in binary)
    out ($c2), a ; Write the updated value back to Port C

    ; Add a delay to control the frequency of the sound
    call delay
    
    ; Toggle the flag in register D
    mov a, d
    cma ; Complement accumulator (NOT operation)
    mov d, a

    ; Continue the loop
    dcx h ; Decrement register pair HL
    mov a, l ; Move the content of register L to register A
    ora h ; Logical OR of register A and register H
    jnz sound_loop ; Jump to sound_loop if the result of OR is not zero

    ret ; Return from the subroutine when HL reaches 0

; Delay subroutine
delay:
    ; Customize the delay value to adjust the frequency of the sound
    ; Smaller values will result in a higher frequency, and larger values will result in a lower frequency
    ld b, 255 ; Load a smaller delay value into register B
	mvi d, 0
delay_loop:
    nop ; Do nothing (waste a CPU cycle)
    djnz delay_loop ; Decrement B and loop until B is zero
    ret ; Return from the subroutine


_confirm_sound_play:
    lxi h, 255 ; Load immediate value 100 into register pair HL

sound_loop1:
    in a, ($c2) ; Read the current value of Port C ($c2)
    xor a, d ; Toggle bit 0 (Tape Out) of Port C (00000001 in binary)
    out ($c2), a ; Write the updated value back to Port C

    ; Add a delay to control the frequency of the sound
    call delay1
    
    ; Toggle the flag in register D
    mov a, d
    cma ; Complement accumulator (NOT operation)
    mov d, a

    ; Continue the loop
    dcx h ; Decrement register pair HL
    mov a, l ; Move the content of register L to register A
    ora h ; Logical OR of register A and register H
    jnz sound_loop1 ; Jump to sound_loop if the result of OR is not zero

    ret ; Return from the subroutine when HL reaches 0

; Delay subroutine
delay1:
    ; Customize the delay value to adjust the frequency of the sound
    ; Smaller values will result in a higher frequency, and larger values will result in a lower frequency
    ld b, 100 ; Load a smaller delay value into register B
delay_loop1:
    nop ; Do nothing (waste a CPU cycle)
    djnz delay_loop1 ; Decrement B and loop until B is zero
    ret ; Return from the subroutine



_fuck_sound_play:
    lxi h, 200 ; Load immediate value 100 into register pair HL

sound_loop2:
    in a, ($c2) ; Read the current value of Port C ($c2)
    xor a, $00 ; Toggle bit 0 (Tape Out) of Port C (00000001 in binary)
    out ($c2), a ; Write the updated value back to Port C

    ; Add a delay to control the frequency of the sound
    call delay1
    
    ; Continue the loop
    dcx h ; Decrement register pair HL
    mov a, l ; Move the content of register L to register A
    ora h ; Logical OR of register A and register H
    jnz sound_loop1 ; Jump to sound_loop if the result of OR is not zero

    ret ; Return from the subroutine when HL reaches 0

; Delay subroutine
delay2:
    ; Customize the delay value to adjust the frequency of the sound
    ; Smaller values will result in a higher frequency, and larger values will result in a lower frequency
    ld b, 20 ; Load a smaller delay value into register B
delay_loop2:
    nop ; Do nothing (waste a CPU cycle)
    djnz delay_loop1 ; Decrement B and loop until B is zero
    ret ; Return from the subroutine
