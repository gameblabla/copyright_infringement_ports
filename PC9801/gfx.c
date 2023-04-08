#include <stdio.h>
#include <conio.h> /* this is where Open Watcom hides the outp() etc. functions */
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <dos.h>
#include <ctype.h>
#include <i86.h>
#include <fcntl.h>
#include <math.h>
#include "gdc.h"
#include "bios.h"
#include "gfx.h"

#define MISC_WAIT    0x5F     // Waits 600ns when written to via outp().
static unsigned char old_palette[16 * 3];

static void gfx_store_palette(unsigned char* palette, size_t len)
{
	uint_fast8_t i;
	for (i = 0; i < len; i++) {
		outp(GDC_PALETTE_INDEX, i);
		*palette++ = inp(GDC_PALETTE_R);
		*palette++ = inp(GDC_PALETTE_G);
		*palette++ = inp(GDC_PALETTE_B);
	}
}

void Cursor_SetXY(unsigned char x, unsigned char y) 
{
	union REGS regs;
	regs.h.cl = 0x10;
	regs.h.ah = 3;
	regs.h.dh = y;
	regs.h.dl = x;
	int86(0xDC, &regs, &regs);
}

void Print_text(const char *str)
{
	union REGS regs;
	regs.h.cl = 0x10;
	regs.h.ah = 0;
	while(*str) {
		regs.h.dl = *(str++);
		int86(0xDC, &regs, &regs);
	}
	outp(0x64, 0); // VSync interrupt trigger	
}


void gfx_init()
{
	union REGS in, out;
	union REGS regs;
	
	gfx_store_palette(old_palette, 16);
	
	in.h.ah = VGA_MODE;
	in.h.ch = (3) << 6;
	int86(INTERRUPT_VGA, &in, &out);
	in.h.ch = 0;
	
	/* Unlock settings. */
	outp(GDC_CONTROL2, GDC_CTL2_LOCK   | 1);
	/* Colors. 0: colors, 1: no colors */
	outp(GDC_CONTROL,  GDC_CTL_COLORS  | 0);
	/* Plane size. 0: 400 line, 1: 200 line */
	outp(GDC_CONTROL,  GDC_CTL_LINES   | 0);
	/* 16 color mode. */
	outp(GDC_CONTROL2, GDC_CTL2_COLORS | 1);
	/* Lock settings. */
	outp(GDC_CONTROL2, GDC_CTL2_LOCK   | 0);
	
	/* Enable graphics. */
	in.h.ah = VGA_START;
	int86(INTERRUPT_VGA, &in, &out);
	/* Start graphical co-GPU. */
	outp(GDC_GFX_COMMAND, GDC_CMD_START);
			
	#if 0
	/* Disable text. */
	in.h.ah = VGA_HIDE_TEXT;
	int86(INTERRUPT_VGA, &in, &out);
	/* Stop text co-GPU. */
	outp(GDC_TEXT_COMMAND, GDC_CMD_STOP);
	#endif
	
	/* Set graphical co-GPU as vsync master and text co-GPU as vsync slave. */
	outp(GDC_GFX_COMMAND,  GDC_CMD_VSYNC_MASTER);
	outp(GDC_TEXT_COMMAND, GDC_CMD_VSYNC_SLAVE);
	
	/* Clock GPUs to 5mhz */
	outp(GDC_CONTROL2, GDC_CTL2_LOCK | 1);
	outp(GDC_CONTROL2, GDC_CTL2_TEXT_CLOCK | 1);
	outp(GDC_CONTROL2, GDC_CTL2_GFX_CLOCK  | 1);
	outp(GDC_CONTROL2, GDC_CTL2_LOCK | 0);
	
	// Set Planes
	gfx_set_plane(GFX_PLANE_DISPLAY,   0);
	gfx_set_plane(GFX_PLANE_RENDERING, 1);	
	
	CLEAR_TEXT_VRAM();
	
	// Hides text cursor
	__asm
	{
		mov	DX,'5h'
		
		mov	AL,27	; ESC
		int	29h
		mov	AL,'['	; '['
		int	29h
		mov	AL,'>'	; '>'
		int	29h
		mov	AL,DH
		int	29h
		mov	AL,DL
		int	29h
	}
}

void gfx_end()
{
	union REGS in, out;
	
	gfx_fill_palette(old_palette, 16);
	
	/* Clock both GPUs back to 2.5MHz. */
	outp(GDC_CONTROL2, GDC_CTL2_LOCK | 1);
	outp(GDC_CONTROL2, GDC_CTL2_TEXT_CLOCK | 0);
	outp(GDC_CONTROL2, GDC_CTL2_TEXT_CLOCK | 0);
	outp(GDC_CONTROL2, GDC_CTL2_LOCK | 0);
	
	/* Enable text. */
	in.h.ah = VGA_SHOW_TEXT;
	int86(INTERRUPT_VGA, &in, &out);
	/* Start text co-GPU. */
	outp(GDC_TEXT_COMMAND, GDC_CMD_START);

	/* Disable graphics. */
	in.h.ah = VGA_STOP;
	int86(INTERRUPT_VGA, &in, &out);
	/* Stop graphical co-GPU. */
	outp(GDC_GFX_COMMAND, GDC_CMD_STOP);

	/* Set text co-GPU as vsync master and graphical co-GPU as vsync slave. */
	outp(GDC_TEXT_COMMAND, GDC_CMD_VSYNC_MASTER);
	outp(GDC_GFX_COMMAND,  GDC_CMD_VSYNC_SLAVE);
	
	CLEAR_TEXT_VRAM();
	
	// Show cursor
	__asm
	{
		mov	DX,'1l'
		
		mov	AL,27	; ESC
		int	29h
		mov	AL,'['	; '['
		int	29h
		mov	AL,'>'	; '>'
		int	29h
		mov	AL,DH
		int	29h
		mov	AL,DL
		int	29h
	}
	
	gfx_fill_palette(old_palette, 16);
}

void gfx_wait_vsync()
{
	/* Wait until vsync occurs. */
	while (!(inp(GDC_GFX_STATUS) & GDC_STATUS_VSYNC)) {}
	
	/* Now wait until it's over. */
	while (inp(GDC_GFX_STATUS)   & GDC_STATUS_VSYNC) {}
}

void gdc_wait_until_ready(unsigned short mode)
{
	unsigned short target;

	/* Figure out which GPU to bother. */
	if (mode == GFX_MODE_TEXT) {
		target = GDC_TEXT_STATUS;
	} else {
		target = GDC_GFX_STATUS;
	}
	
	while (inp(target) & GDC_STATUS_FULL) {
		/* Wait 1.2ms to not bother the GPU too much. */
		outp(MISC_WAIT, 0);
		outp(MISC_WAIT, 0);
	}
}

void gfx_set_plane(gfx_plane_t plane, unsigned char which)
{
	outp(GDC_SET_DISP_PLANE + plane, which);
}

/**
 * Get plane.
 */
unsigned short gfx_get_plane(gfx_plane_t plane)
{
	return inp(GDC_GET_DISP_PLANE + plane);
}

void gfx_fill_palette(const unsigned char* palette, size_t len)
{
	uint_fast8_t i;
	for (i = 0; i < len; i++) {
		_outp(GDC_PALETTE_INDEX, i);
		_outp(GDC_PALETTE_R, palette[(i*3)]);
		_outp(GDC_PALETTE_G, palette[(i*3)+1]);
		_outp(GDC_PALETTE_B, palette[(i*3)+2]);
	}
}
