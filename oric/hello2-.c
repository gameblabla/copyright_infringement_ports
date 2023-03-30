#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <cc65.h>
#include <conio.h>
#include <ctype.h>
#include <joystick.h>

#include <atmos.h>

#include "img.h"

#define poke(addr, value) (*(volatile uint8_t *)(addr) = (value))

/**
void atmos_explode (void);
void atmos_ping (void);
void atmos_shoot (void);
void atmos_tick (void);
void atmos_tock (void);
atmos_zap
*/

volatile void* display = (volatile void*) 0xA000;

#define HIRES_START 0xA000
#define HIRES_SIZE  8000
#define ORIC1_STATUS_ADDR (*(volatile uint8_t *)0x026A)

/*
 * These were taken from CC65-Chess
*/

void hires_DrawCharA(void);
#define ROP_CPY                 0x40
#define ROP_INV                 0xc0
#define CHAR_HEIGHT             8

extern unsigned char hires_xpos;
extern unsigned char hires_ypos;
extern unsigned char hires_xsize;
extern unsigned char hires_ysize;
extern unsigned char hires_rop;
extern unsigned char hires_clr;
extern unsigned char* hires_src;
extern char hires_CharSet[96][CHAR_HEIGHT];


void plat_DrawChar(unsigned char xpos, unsigned char ypos,  unsigned char rop, char c)
{
	hires_xpos = xpos;
	hires_ypos = ypos;
	hires_rop = rop;
	hires_src = hires_CharSet[c-' '];
	hires_DrawCharA();
}

void plat_DrawString(unsigned char x, unsigned char y,  unsigned char rop, char *s)
{
	while(*s)
	{
		plat_DrawChar(x++,y, rop, *s++);
	}
}

void Clear_Text()
{
	plat_DrawString(0, 180, ROP_CPY, "                                       ");
	plat_DrawString(0, 188, ROP_CPY, "                                       ");	
}

int main (void)
{
	unsigned char c, i;
	
   // joy_install (joy_static_stddrv);

	// Enable HIRES mode
	//__asm__ ("jsr $EC33");
	// brk $1a, Telemon
	
	joy_install (joy_static_stddrv);
	
	__asm__ ("jsr $EC33");
		
	// Disable blinking cursor and sound
	ORIC1_STATUS_ADDR = (ORIC1_STATUS_ADDR | 0x08) & 0xFE;
	
	// Zero out the lower portion too
	bzero(0xA000 + 8000, 128);

	/*__asm__ ("jsr _DrawPict");
	//memcpy(display, _LabelPicture, sizeof(_LabelPicture));
	
	Clear_Text();
	plat_DrawString(0, 180, ROP_CPY, "Oh my, what do we have here?");
	plat_DrawString(0, 188, ROP_CPY, "You look so innocent...");
	
	while (!joy_read (JOY_1))
	;
	while (joy_read (JOY_1))
	;
	
	//cgetc ();
	
	Clear_Text();
	plat_DrawString(0, 180, ROP_CPY, "You'll be the perfect victim");
	plat_DrawString(0, 188, ROP_CPY, "for my games !");*/
	
	
	
    while(1)
    {
		memcpy(display, img, 5080);
		memcpy(display, img + 5080, 5080);
		memcpy(display, img + 5080*2, 5080);
		memcpy(display, img + 5080*3, 5080);
		memcpy(display, img + 5080*4, 5080);
	}

    return EXIT_SUCCESS;
}
