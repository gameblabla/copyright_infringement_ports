/*
 * CGA Graphics mode
 * Supported :
 * - B&W 640x200
 * - 320x200 4 colors
 * - 160x200 16 colors composite (actually 640x200 with colorburst turned on)
 * 
 * Supports on the fly LZSA1 decompression
*/
#include "generic.h"
#include "generic_io.h"
#include "graph.h"
#include "cga.h"

#if defined(__386__)
static unsigned char *CGA = (unsigned char*)0xB8000;
#else
static unsigned char __far *CGA = (unsigned char __far*)0xB8000000L;
#endif

static uint8_t screen_width_in_chars = 80;


static unsigned char inverted_graphics = 0;

static unsigned char cga_mask[] = {0xC0, 0x30, 0x0C, 0x03};
static unsigned char cga_shift[] = {6, 4, 2, 0};

#if 0
#define CGA_COLOR_CONTROL_REGISTER 0x3D9
static inline void cga_plot(unsigned short x, unsigned char y, unsigned char color) {
    unsigned char far *p = (char far*)0xB8000000L; /* Base address for CGA video memory */
    unsigned char m = cga_mask[x & 0x3];  /* Masks for each position in a byte */
    /* Adjust pointer based on the even/odd scanline without branching */
    p += (0x2000 & (-(y & 0x1)));
    y &= 0xFFFE;
	/* move pointer to byte containing pixel (y<<3 + y<<5 == y*40) */
	p += ((y << 3) + (y << 5) + (x >> 2));
    /* Read/write video memory; color is the full bit pattern for solid color */
    *p = (*p & ~m) | ((color << cga_shift[x & 0x3]) & m);
}
void set_cga_palette(unsigned char palette) {
    union REGS regs;

    // Set palette and intensity
    regs.w.ax = 0x0B00;
    regs.w.bx = 0x0100 | (palette & 0x01);
    int86(0x10, &regs, &regs);

    // Set the background/border color (optional)
    // For this example, we set the border color to black (0x00)
    regs.w.ax = 0x0B00;
    regs.w.bx = 0x0000; // Change the value to set a different border color
    int86(0x10, &regs, &regs);
}


static void drawChar(char ch, unsigned short x, unsigned char y, unsigned char color_index)
{
	unsigned char i, j;
	unsigned char *charSprite;
	unsigned short plot_x, plot_y;
	unsigned char far *p;
	unsigned char m;
	
	charSprite = (ch << 3) + std_font; // Updated line
	// Draw charSprite as monochrome 8*8 image using given color
	i = 8;
	while (i--)
	{
		j = 8;
		while (j--)
		{
			// Make the conditional statement branchless
			unsigned char is_bit_set = (charSprite[i] >> j) & 1;
			
			plot_x = x + (7 - j);
			plot_y = y + i;

			p = (char far *)0xB8000000L; /* Base address for CGA video memory */
			m = cga_mask[plot_x & 0x3];   /* Masks for each position in a byte */

			/* Adjust pointer based on the even/odd scanline without branching */
			p += (0x2000 & (-(plot_y & 0x1)));
			plot_y &= 0xFFFE;

			/* move pointer to byte containing pixel (y<<3 + y<<5 == y*40) */
			p += ((plot_y << 3) + (plot_y << 5) + (plot_x >> 2));

			/* Read/write video memory; color is the full bit pattern for solid color */
			*p = (*p & ~m) | ((is_bit_set * color_index << cga_shift[plot_x & 0x3]) & m);
		}
	}
	x += 8;
}
#endif

unsigned char Init_CGA(unsigned short width, unsigned short height, unsigned short flags, int argc, char** argv)
{
	union REGS regs;
	
	switch(width)
	{
		case 320:
			set_mode(0x04);
			screen_width_in_chars = 40;
			//set_cga_palette(flags);
		break;
		// Composite CGA mode
		// dosbox-x -set machine=cga_composite main.exe 
		case 160:
			regs.w.ax = 0x06;
			INT_86(0x10, (union REGS *)&regs, &regs);
			outp(0x3D8, 0x1A);  // Enable color burst
			//outp(0x3D8, 0x1E);  // Disable color burst
			screen_width_in_chars = 80;
		break;
		case 640:
			// Black & White
			set_mode(0x06);
			//set_cga_palette(flags);
			screen_width_in_chars = 80;
			
			inverted_graphics = flags;
		break;
	}

	screen_width = width;
	screen_height = height;

	memset(CGA, 0, 16384);
	
	return 0;
}


static void CGA_Load_SIF(const char *file, BITMAP *b, unsigned short s_width, unsigned short s_height, unsigned char dontdecompress)
{
	uint16_t i;
	unsigned int size;
	unsigned char* data;
    Generic_Load_SIF(file, b, s_width, s_height, dontdecompress, inverted_graphics);
}

void CGA_DrawBMP_static(BITMAP *bmp, short x, short y, unsigned long offset)
{
	unsigned off = (((y << 8) + (y << 6)) + x);
	void *dst = CGA + off;
    const void *src = bmp->data[0] + offset;
    size_t count = bmp->filesize;
#if defined(__386__)
	memcpy(CGA + off, bmp->data[0] + offset, 16384);
#else
    __asm {
        les di, dst
        lds si, src

        mov ax, count
        shr ax, 1
        mov cx, ax
        rep movsw
    }
#endif
}

void CGA_DrawBMP_compressed(BITMAP *bmp, short x, short y)
{
	unsigned off;
	off = (((y << 8) + (y << 6)) + x);
#if defined(__386__)
	apl_decompress
#else
	CAL_LZSAExpand
#endif
	(bmp->data[0], CGA + off);
}

static void CGA_Free_bmp(BITMAP *bmp)
{
	if (bmp->data[0])
	{
		free(bmp->data[0]);
		bmp->data[0] = NULL;
	}
}

void CGA_stub()
{
}

void CGA_Clear()
{
	memset(CGA, 0, 16384);
}

// CGA palette is stuck, cannot be reset (except perhaps on some CGA clone cards)
void CGA_LoadPalette(const char *f)
{
}

/* some video BIOS stuff */
#define _BIOS_SET_MODE			0x0000
#define _BIOS_SET_CURPOS		0x0200
#define _BIOS_GET_MODE			0x0F00
#define _BIOS_SET_PALETTE		0x1000

/* this is what'll be used to call BIOS interrupt 10 to set the video mode */
extern short VideoInt(short, short, short, short);

#pragma aux     VideoInt = \
                "push   bp      ", \
                "int    10h     ", \
                "pop    bp      ", \
                parm caller [ax] [bx] [cx] [dx] value [ax];

void CGA_Draw_Text(const char* str, unsigned short x, unsigned short y, unsigned short color_index)
{
    uint16_t screen_width_in_chars = 40;

    x = x >> 3;
    y = y >> 3;
	
	VideoInt( _BIOS_SET_CURPOS, 0, 0, (x&0xFF)|((y&0xFF)<<8) );
	puts(str);
}

static void CGA_Draw_Text_Center(const char* str, unsigned short y, unsigned short color_index)
{
    uint16_t x;

    x = (screen_width_in_chars - strlen(str)) >> 1;
    y = y >> 3;
	
	VideoInt( _BIOS_SET_CURPOS, 0, 0, (x&0xFF)|((y&0xFF)<<8) );
	puts(str);
}

#define INPUT_STATUS 0x03DA
#define VRETRACE_BIT 3

void CGA_Vsync()
{
	unsigned char status;
    // Wait until the CPU can touch the CGA buffer without causing snow.
    while (!(inp(INPUT_STATUS) & 0x01));

    // Wait until vertical retrace is finished (bit 3 == 0).
    do {
        status = inp(INPUT_STATUS);
    } while (status & (1 << VRETRACE_BIT));
    
    // Wait until vertical retrace is active (bit 3 == 1).
    do {
        status = inp(INPUT_STATUS);
    } while (!(status & (1 << VRETRACE_BIT)));
}

void Kill_CGA()
{
	CGA_Clear();
	outp(0x3D8, 0x1E); // Disable colorburst
	set_mode(TEXT_MODE);
}



VideoDevice Normal_CGA = {
	"CGA video",
	Init_CGA,
	CGA_DrawBMP_static,
	CGA_DrawBMP_compressed,
	CGA_stub, //Draw Sprite trans
	CGA_stub, //Draw sprite non trans
	CGA_Load_SIF,
	CGA_LoadPalette,
	CGA_stub, // Set palette
	CGA_Free_bmp,
	CGA_Clear,
	CGA_stub,
	CGA_Vsync,
	Kill_CGA,
	CGA_Draw_Text, // Draw Text
	CGA_Draw_Text_Center
};

