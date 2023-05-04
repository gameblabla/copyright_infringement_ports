#include "generic.h"
#include "generic_io.h"
#include "graph.h"
#include "hgcc.h"

#define BYTES_PER_ROW 90

void HGCC_Clear();

/*
* 0 (0xEF) : Blue
* 1 (0xDF) : Green
* 2 (0xBF) : Red
* 3 (0x7F) : Intensity
*/
static const uint8_t plane_sel[] = { 0xEF, 0xDF, 0xBF, 0x7F};

// Actually it's not the original palette but it's good enough to not mess it up too much on exit
const unsigned char original_hgcc_pal[16] =
{
    0x00, 0x01, 0x02, 0x00, 0x3f, 0x3f, 0x3f, 0x3f,
    0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f
};


unsigned char __far *HGCC = (unsigned char __far*)0xB0000000;
#define PLANE_SIZE 32768

/* HGC values for HGC graphics */
const unsigned char hgcc_graphics_crtc[12] = {
	0x35,0x2D,0x2E,0x07,
	0x5B,0x02,0x57,0x57,
	0x02,0x03,0x00,0x00
};


const unsigned char hgcc_text_crtc[12] = {
	0x61,0x50,0x52,0x0F,
	0x19,0x06,0x19,0x19,
	0x02,0x0D,0x0D,0x0C
};

static void HGCC_Turn_on() {
	unsigned char c;

	outp(0x3B8,0x00); /* turn off video */
	outp(0x3BF,0x01); /* enable setting graphics mode */
	outp(0x3B8,0x02); /* turn on graphics */
	for (c=0;c < 12;c++) {
		outp(0x3B4,c);
		outp(0x3B5,hgcc_graphics_crtc[c]);
	}
	
	// Don't use RAM fonts
	outp(0x3B4,14);
	outp(0x3B5,0x00);
	
	outp(0x3B4, 0x17);
	outp(0x3B5, 0x00); 
	
	// 0x00 to use default CGA-like palette
	// 0x10 to Enable custom palette
	
	// Set Draw plane mode
	outp(0x3B4, 0x18); // Register 18h: Plane mask
	outp(0x3B5, 0xF); // Draw to all planes
	
	// Set Write mode
	outp(0x3B4,0x19);
	outp(0x3B5,0x5F); // Bits 5-4: Write mode is 0, Bit 0-3 Don't care for planes comparison bits
}

static void HGCC_Turn_off() {
	unsigned char c;

	outp(0x3B8,0x00); /* turn off video */
	outp(0x3BF,0x00); /* disable setting graphics mode */
	for (c=0;c < 12;c++) {
		outp(0x3B4,c);
		outp(0x3B5,hgcc_text_crtc[c]);
	}
	outp(0x3B8,0x28); /* turn on video and text */
}

/*
static void plot_pixel(unsigned short x, unsigned short y, uint8_t color) 
{
    unsigned int offset;
    unsigned char pixel_mask;
    uint8_t plane;
    offset = ((y >> 2) * BYTES_PER_ROW) + ((y & 3) * 0x2000) + (x >> 3);
    pixel_mask = 0x80 >> (x & 7);
    
	outp(0x3B4, 0x18); // Register 18h: Plane mask
	outp(0x3B5, 0x0F); // Select all planes
    outp(0x3B4, 0x1A);
	outp(0x3B5, color);
	
	vmem[offset] |= pixel_mask;
}
*/

// It's worth noting that you need to set these before using pixel (unless you are doing it already)
/*
	outp(0x3B4, 0x18); // Register 18h: Plane mask
	outp(0x3B5, 0xF); // Draw to all planes
	outp(0x3B4, 0x1A); // Register 1Ah: Read/write colour register
	outp(0x3B5, color_index); // Set color (from 0 to 15)
	 
	You may also need to declare them (as unsigned y_div_4; unsigned y_mod_4;)
 */
#define setPixel(x,y,color) y_div_4 = y >> 2; y_mod_4 = y & 3; HGCC[(y_div_4 << 6) + (y_div_4 << 4) + (y_div_4 << 3) + (y_div_4 << 1) + (y_mod_4 << 13) + (x >> 3)] |= 0x80 >> (x & 7);

static void drawChar(char ch, unsigned short x, unsigned char y, unsigned char color_index)
{
	unsigned char i, j;
	unsigned char *charSprite;
	unsigned short plot_y;
	
	unsigned y_div_4;
	unsigned y_mod_4;
	
	charSprite = (ch << 3) + std_font; // Updated line
	// Draw charSprite as monochrome 8*8 image using given color
	i = 8;
	while (i--)
	{
		j = 8;
		plot_y = y + i;
		while (j--)
		{
			if((charSprite[i] >> j) & 1)
			{
				setPixel(x + (7 - j), plot_y, color_index);
			}
		}
	}
	x += 8;
}


unsigned char Init_HGCC(unsigned short width, unsigned short height, unsigned short flags, int argc, char** argv)
{
	HGCC_Turn_on();
	HGCC_Clear();
	
	screen_width = 720;
	screen_height = 348;

	return 0;
}


static void HGCC_Load_SIF(const char *file, BITMAP *b, unsigned short s_width, unsigned short s_height, unsigned char dontdecompress)
{
    int fd;
    char str[16];
    unsigned char* temp_mem;
    unsigned totalsize;
    unsigned char planes, i;
    unsigned short toread;
    unsigned char *tmp;
	BMPHeader header;

    _dos_open(file, O_RDONLY, &fd);
	_dos_read(fd, &header, sizeof(BMPHeader), &totalsize);

	b->width = header.width;
	b->height = header.height;
	b->sprite_width = s_width;
	b->sprite_height = s_height;
	b->encoding = header.encoding;
	b->filesize = header.filesize;
	for(i=0;i<4;i++)
	{
		b->plane_size[i] = header.plane_size[i];
		if (b->plane_size[i] < 1) b->plane_size[i] = PLANE_SIZE;
	}

#ifdef __DJGPP__
	if (b->data[0])
	{
		free(b->data[0]);
		b->data[i] = NULL;
	}
	b->data[0] = malloc(header.filesize);
	_dos_read(fd, b->data[0], header.filesize, &totalsize);
   	
   	b->data[1] = b->data[0] + PLANE_SIZE;
	b->data[2] = b->data[0] + PLANE_SIZE*2;
	b->data[3] = b->data[0] + PLANE_SIZE*3;
#else
   	if (b->encoding == 5 && dontdecompress == 0)
   	{
		tmp = malloc(PLANE_SIZE);
		for( i=0; i<4; i++ )
		{
			if (b->data[i])
			{
				free(b->data[i]);
				b->data[i] = NULL;
			}
			b->data[i] = malloc(PLANE_SIZE * (header.height / 348));
			_dos_read(fd, tmp, b->plane_size[i], &totalsize);
			CAL_LZSAExpand(tmp, b->data[i]);
		}
		
		for(i=0;i<4;i++)
		{
			b->plane_size[i] = PLANE_SIZE * (header.height / 348);
		}
	}
   	else
   	{
		toread = header.filesize >> 2;
		for( i=0; i<4; i++ )
		{
			if (b->data[i])
			{
				free(b->data[i]);
				b->data[i] = NULL;
			}
			b->data[i] = malloc(PLANE_SIZE);
			_dos_read(fd, b->data[i], PLANE_SIZE, &totalsize);
		}
	}
#endif
	 _dos_close(fd);
	if (tmp)
	{
		free(tmp);
		tmp = NULL;
	}
    
    if (s_width == 0) b->sprite_width = b->width;
    if (s_height == 0) b->sprite_height = b->height;
}

// void CAL_LZSAExpand (uint8_t *source, uint8_t *dest)

void HGCC_DrawBMP_static(BITMAP *bmp, short x, short y, unsigned long offset)
{
	unsigned long off;
	unsigned char i;
	unsigned y_div_4 = y >> 2;
	unsigned y_mod_4 = y & 3;
	
	off = (y_div_4 << 6) + (y_div_4 << 4) + (y_div_4 << 3) + (y_div_4 << 1) + (y_mod_4 << 13) + (x >> 3);
	outp(0x3B4, 0x1A); // Register 1Ah: Read/write colour register
	outp(0x3B5, 0x0F); // Set Color to Lit/White (???)

	for(i=0;i<4;i++)
    {
		outp(0x3B4, 0x18); // Which plane to select
		outp(0x3B5, plane_sel[i]);
		memcpy(HGCC + off , bmp->data[i] + offset, bmp->plane_size[i]);
	}
}

void HGCC_DrawBMP_compressed(BITMAP *bmp, short x, short y)
{
	unsigned long off;
	unsigned char i;
	unsigned y_div_4 = y >> 2;
	unsigned y_mod_4 = y & 3;
	
	off = (y_div_4 << 6) + (y_div_4 << 4) + (y_div_4 << 3) + (y_div_4 << 1) + (y_mod_4 << 13) + (x >> 3);
	outp(0x3B4, 0x1A); // Register 1Ah: Read/write colour register
	outp(0x3B5, 0x0F); // Set Color to Lit/White (???)

	for(i=0;i<4;i++)
    {
		outp(0x3B4, 0x18); // Which plane to select
		outp(0x3B5, plane_sel[i]);
		CAL_LZSAExpand(bmp->data[i], HGCC + off);
	}
}

static void HGCC_Free_bmp(BITMAP *b)
{
	unsigned char i;
#ifdef __DJGPP__
	i = 0;
#else
	for( i=0; i<4; i++ )
#endif
	{
		if (b->data[i])
		{
			free(b->data[i]);
			b->data[i] = NULL;
		}
   	}
}

void HGCC_stub()
{
}

void HGCC_Clear()
{
	unsigned char i;
	outp(0x3B4, 0x18);
	outp(0x3B5, 0x0F);
    outp(0x3B4, 0x1A);
	outp(0x3B5, 0xF);
	outp(0x3B4, 0x1A); // Register 1Ah: Read/write colour register
	outp(0x3B5, 0x00); // Set Color to black
	for(i=0;i<4;i++)
	{
		outp(0x3B4, 0x18); // Which plane to select
		outp(0x3B5, plane_sel[i]);
		memset(HGCC, 0x00, 720*348);
	}
}

// EGA-based apparently, although i actually may need to check if the 64 color palettes match ?
void HGCC_LoadPalette(const char *f)
{
	unsigned char i;
	int fd;
	unsigned totalsize;
	_dos_open(f, O_RDONLY, &fd);
	_dos_read(fd, VGA_8514_GAMEPAL, 16, &totalsize);
	_dos_close(fd);
}

void HGCC_Apply_palette()
{
	unsigned char i;
	outp(0x3B4, 0x17);
	outp(0x3B5, 0x10); // Enable custom palette
	
	inp(0x1C); // Set whole palette to 0, according to documentation
	for(i=0;i<16;i++)
    {
		outp(0x3B4, 0x1C);
		outp(0x3B5, VGA_8514_GAMEPAL[i]);
	}
}

void HGCC_Draw_Text(const char* str, unsigned short x, unsigned short y, unsigned short color_index)
{
	unsigned char i;
	
	// Do it outside of drawChar for speed
	outp(0x3B4, 0x18); // Register 18h: Plane mask
	outp(0x3B5, 0xF); // Draw to all planes
	// Set the foreground and background colors
	outp(0x3B4, 0x1A); // Register 1Ah: Read/write colour register
	outp(0x3B5, color_index);
	
	for(i = 0; str[i]; i++)
	{
		drawChar(str[i], x + (i << 3), y, color_index);
	}
}

void HGCC_Draw_Text_Center(const char* str, unsigned short y, unsigned short color_index)
{
	unsigned char i;
	uint16_t x;
	x = (screen_width - (strlen(str) << 3)) >> 1;
	
	// Do it outside of drawChar for speed
	// Otherwise it will be way too slow (outp can be 47 CPU cycles)
	outp(0x3B4, 0x18); // Register 18h: Plane mask
	outp(0x3B5, 0xF); // Draw to all planes
	// Set the foreground and background colors
	outp(0x3B4, 0x1A); // Register 1Ah: Read/write colour register
	outp(0x3B5, color_index);
	
	for(i = 0; str[i]; i++)
	{
		drawChar(str[i], x + (i << 3), y, color_index);
	}
}

#define INPUT_STATUS 0x03BA
#define VRETRACE_BIT 7


void HGCC_Vsync()
{
	// Wait until vertical retrace is active.
	while (!(inp(INPUT_STATUS) & (1 << VRETRACE_BIT)));

	// Wait until vertical retrace is inactive.
	while (inp(INPUT_STATUS) & (1 << VRETRACE_BIT));
}

void Kill_HGCC()
{
	unsigned char i;
	
	// Clear planes
	HGCC_Clear();
	
	// Set palette upon exit
	outp(0x3B4, 0x17);
	outp(0x3B5, 0x10); // Enable custom palette
	inp(0x1C); // Set whole palette to 0, according to documentation
	for(i=0;i<16;i++)
    {
		outp(0x3B4, 0x1C);
		outp(0x3B5, original_hgcc_pal[i]);
	}
	// Turn off graphics mode
	HGCC_Turn_off();
	
	// Go back to CGA text mode
	set_mode(TEXT_MODE);
}


VideoDevice Normal_HGCC = {
	"Hercules InColor",
	Init_HGCC,
	HGCC_DrawBMP_static,
	HGCC_DrawBMP_compressed,
	HGCC_stub, //Draw Sprite trans
	HGCC_stub, //Draw sprite non trans
	HGCC_Load_SIF,
	HGCC_LoadPalette,
	HGCC_Apply_palette, // Set palette
	HGCC_Free_bmp,
	HGCC_Clear,
	HGCC_stub,
	HGCC_Vsync,
	Kill_HGCC,
	HGCC_Draw_Text // Draw Text
};

