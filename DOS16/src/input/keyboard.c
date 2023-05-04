#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <ctype.h>
#include <fcntl.h>
#include <math.h>
#include <dos.h>
#include "generic.h"
#include "keyboard.h"

#define KEYBOARD_IRQ_VECTOR 9
#define KEYBOARD_BUFFER 0x0040 << 4 + 0x001A
#define KEYBOARD_STATUS 0x0064
#define KEYBOARD_DATA 0x0060

#define KEY_PRESSED_MASK_JR 0x00 // For whatever reason this is different for PCjr
#define KEY_PRESSED_MASK 0x01
#define KEY_RELEASED_MASK 0x80

unsigned char keystate[256]; // Array to store key state

void (__interrupt __far *old_int09_handler)(); // Pointer to old int09 handler
void __interrupt __far handle_keyboard_interrupt() {
    unsigned char status = inp(KEYBOARD_STATUS);
    unsigned char keycode = inp(KEYBOARD_DATA);
    // Clear keyboard buffer by reading all characters from the buffer.
    // This prevents the BIOS from processing any keys that were pressed
    // while the interrupt was being handled.
    while ((inp(KEYBOARD_STATUS) & (KEY_PRESSED_MASK | KEY_RELEASED_MASK)) != 0) {
        inp(KEYBOARD_DATA);
    }
	keystate[keycode] = ISRELEASED;
    // Set the key state in the keystate array.
    if(!(status & KEY_PRESSED_MASK) || !(status & KEY_PRESSED_MASK_JR)) {
        keystate[keycode] = ISPRESSED;
    }

    // Pass control to the old int09 handler.
    (*old_int09_handler)();
    
#ifdef __DJGPP__
	//Acknowledge hardware interrupt
	outportb (0x20, 0x20);
#endif
}
void Keyboard_Init()
{
	unsigned long far *old_int09_handler_ptr;
	
	memset(keystate, 0, 256);
	
    _disable(); // Disable interrupts

    // Save the address of the old int09 handler.
	old_int09_handler_ptr = (unsigned long far *) MK_FP(0x0000, KEYBOARD_IRQ_VECTOR * 4);
    old_int09_handler = (void (__interrupt __far *)()) *old_int09_handler_ptr;

    // Install the new int09 handler.
    _dos_setvect(KEYBOARD_IRQ_VECTOR, handle_keyboard_interrupt);

    _enable(); // Enable interrupts
}

void Keyboard_Clean()
{
    _disable(); // Disable interrupts

    // Restore the original int09 handler.
    _dos_setvect(KEYBOARD_IRQ_VECTOR, old_int09_handler);

    _enable(); // Enable interrupts
}
