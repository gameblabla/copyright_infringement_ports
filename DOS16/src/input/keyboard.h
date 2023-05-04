#ifndef KEYBOARD_H
#define KEYBOARD_H


#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define KEY_SPACE 57
#define KEY_ESC 1

#define ISPRESSED 1
#define ISRELEASED 0


extern unsigned char keystate[256]; // Array to store key state;
extern void Keyboard_Init();
extern void Keyboard_Clean();

#endif
