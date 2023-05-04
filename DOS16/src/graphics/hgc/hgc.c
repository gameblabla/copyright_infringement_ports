/**************************************************************************
 * modes.c                                                                *
 * written by David Brackeen                                              *
 * http://www.brackeen.com/home/vga/                                      *
 *                                                                        *
 * Tab stops are set to 2.                                                *
 * This program compiles with DJGPP! (www.delorie.com)                    *
 * To compile in DJGPP: gcc modes.c -o modes.exe                          *
 *                                                                        *
 * This program will only work on DOS- or Windows-based systems with a    *
 * VGA, SuperVGA, or compatible video adapter.                            *
 *                                                                        *
 * Please feel free to copy this source code.                             *
 *                                                                        *
 * DESCRIPTION: This program demonstrates various unchained modes         *
 **************************************************************************/
#include "generic.h"
#include "generic_io.h"
#include "graph.h"
#include "hgc.h"

#define BYTES_PER_ROW 90

unsigned char __far *HGC = (unsigned char __far*)0xB0000000;

/* HGC values for HGC graphics */
const unsigned char hgc_graphics_crtc[12] = {
	0x35,0x2D,0x2E,0x07,
	0x5B,0x02,0x57,0x57,
	0x02,0x03,0x00,0x00
};

const unsigned char hgc_text_crtc[12] = {
	0x61,0x50,0x52,0x0F,
	0x19,0x06,0x19,0x19,
	0x02,0x0D,0x0D,0x0C
};

static void HGC_Turn_on() {
	unsigned char c;

	outp(0x3B8,0x00); /* turn off video */
	outp(0x3BF,0x01); /* enable setting graphics mode */
	outp(0x3B8,0x02); /* turn on graphics */
	for (c=0;c < 12;c++) {
		outp(0x3B4,c);
		outp(0x3B5,hgc_graphics_crtc[c]);
	}
	outp(0x3B8,0x0A); /* turn on graphics+video */
}

static void HGC_Turn_off() {
	unsigned char c;

	outp(0x3B8,0x00); /* turn off video */
	outp(0x3BF,0x00); /* disable setting graphics mode */
	for (c=0;c < 12;c++) {
		outp(0x3B4,c);
		outp(0x3B5,hgc_text_crtc[c]);
	}
	outp(0x3B8,0x28); /* turn on video and text */
}

static void plot_pixel(unsigned short x, unsigned short y, uint8_t color) {
    unsigned int offset;
    unsigned char pixel_mask;

    // Calculate memory offset
    //offset = ((y >> 2) * BYTES_PER_ROW) + ((y & 3) * 0x2000) + (x >> 3);
    // Faster alternative
	unsigned y_div_4 = y >> 2;
	unsigned y_mod_4 = y & 3;

	offset = (y_div_4 << 6) + (y_div_4 << 4) + (y_div_4 << 3) + (y_div_4 << 1) + (y_mod_4 << 13) + (x >> 3);

    // Calculate pixel mask
    pixel_mask = 0x80 >> (x & 7);

    // Write pixel to video memory
    if (color) HGC[offset] |= pixel_mask;
	else HGC[offset] &= ~pixel_mask;
}

static void drawChar(char ch, unsigned short x, unsigned short y, unsigned char color_index)
{
	unsigned char i, j;
	unsigned char *charSprite;
	unsigned short plot_x, plot_y;
	unsigned char far *p;
	unsigned char m;
    unsigned int offset;
    unsigned char pixel_mask;
    unsigned char is_bit_set;
    
	charSprite = (ch << 3) + std_font; // Updated line
	// Draw charSprite as monochrome 8*8 image using given color
	i = 8;
	while (i--)
	{
		j = 8;
		while (j--)
		{
			if((charSprite[i] >> j) & 1)
			{
				plot_pixel(x + (7 - j), y + i, color_index);
			}
			else
			{
				plot_pixel(x + (7 - j), y + i, 0);
			}
		}
	}
	x += 8;
}


unsigned char Init_HGC(unsigned short width, unsigned short height, unsigned short flags, int argc, char** argv)
{
	HGC_Turn_on();
	memset(HGC, 0x00, 720*348);
	
	screen_width = 720;
	screen_height = 348;
	
	return 0;
}


static void HGC_Load_SIF(const char *file, BITMAP *b, unsigned short s_width, unsigned short s_height, unsigned char dontdecompress)
{
    Generic_Load_SIF(file, b, s_width, s_height, dontdecompress);
}

void HGC_DrawBMP_static(BITMAP *bmp, short x, short y, unsigned long offset)
{
	unsigned long off;
	unsigned y_div_4 = y >> 2;
	unsigned y_mod_4 = y & 3;
	
	off = (y_div_4 << 6) + (y_div_4 << 4) + (y_div_4 << 3) + (y_div_4 << 1) + (y_mod_4 << 13) + (x >> 3);

	memcpy(HGC + off, bmp->data[0] + offset, bmp->filesize);
	//memcpy(HGC + ((y * 320) + x), bmp->data + offset, bmp->width*bmp->height);
}

void HGC_DrawBMP_compressed(BITMAP *bmp, short x, short y)
{
	unsigned long off;
	unsigned y_div_4 = y >> 2;
	unsigned y_mod_4 = y & 3;
	
	off = (y_div_4 << 6) + (y_div_4 << 4) + (y_div_4 << 3) + (y_div_4 << 1) + (y_mod_4 << 13) + (x >> 3);
	
	CAL_LZSAExpand(bmp->data[0], HGC + off);

}

static void HGC_Free_bmp(BITMAP *bmp)
{
	if (bmp->data[0])
	{
		free(bmp->data[0]);
		bmp->data[0] = NULL;
	}
}

void HGC_stub()
{
}

void HGC_Clear()
{
	memset(HGC, 0, 16384*2);
}

void HGC_LoadPalette(const char *f)
{
}

#define INPUT_STATUS 0x03BA
#define VRETRACE_BIT 7

void HGC_Vsync()
{
	// Wait until vertical retrace is active.
	while (!(inp(INPUT_STATUS) & (1 << VRETRACE_BIT)));

	// Wait until vertical retrace is inactive.
	while (inp(INPUT_STATUS) & (1 << VRETRACE_BIT));
}

void HGC_Draw_Text(const char* str, unsigned short x, unsigned short y, unsigned short color_index)
{
	unsigned char i;

	for(i = 0; str[i]; i++)
		drawChar(str[i], x + (i << 3), y, color_index);
}

void HGC_Draw_Text_Center(const char* str, unsigned short y, unsigned short color_index)
{
	unsigned char i;
	uint16_t x;
	x = (screen_width - (strlen(str) << 3)) >> 1;
	
	HGC_Draw_Text(str,  x,  y, color_index);
}

void Kill_HGC()
{
	HGC_Turn_off();
	// Go back to CGA text mode
	set_mode(TEXT_MODE);
}



VideoDevice Normal_HGC = {
	"HGC video",
	Init_HGC,
	HGC_DrawBMP_static,
	HGC_DrawBMP_compressed,
	HGC_stub, //Draw Sprite trans
	HGC_stub, //Draw sprite non trans
	HGC_Load_SIF,
	HGC_LoadPalette,
	HGC_stub, // Set palette
	HGC_Free_bmp,
	HGC_Clear,
	HGC_stub,
	HGC_Vsync,
	Kill_HGC,
	HGC_Draw_Text, // Draw Text
	HGC_Draw_Text_Center
};

