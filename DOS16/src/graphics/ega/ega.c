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
#include "ega.h"

extern unsigned short screen_width, screen_height ;
unsigned char precalc_offset;

unsigned char __far *vmem = (unsigned char __far*)0xA0000000;
unsigned short PLANE_SIZE;

/* some video BIOS stuff */
#define _BIOS_SET_MODE			0x0000
#define _BIOS_SET_CURPOS		0x0200
#define _BIOS_GET_MODE			0x0F00
#define _BIOS_SET_PALETTE		0x1000

// Vsync
#define INPUT_STATUS      	0x03da
#define VRETRACE 			0x08

/* some video register stuff */
#define EGA_SC			0x3C4   /* EGA sequencer control port */
#define EGA_GC			0x3CE   /* EGA graphics control port  */
#define EGA_SC_WPEN     0x02    /* plane write enable register
                                 *  bit 0   - plane 0 write enable
                                 *  bit 1   - plane 1 write enable
                                 *  bit 2   - plane 2 write enable
                                 *  bit 3   - plane 3 write enable
                                 *  bit 4-7 - reserved, always 0
                                 */
#define EGA_GC_MODE     0x05    /* mode register
                                 *  bit 0-1 - write mode
                                 *  bit 2   - reserved, always 0
                                 *  bit 3   - color compare enable
                                 *  bit 4-7 - reserved, always 0
                                 */
#define EGA_GC_BMASK	0x08	/* bitmask register */


/* this is what'll be used to call BIOS interrupt 10 to set the video mode */
extern short VideoInt(short, short, short, short);

#pragma aux     VideoInt = \
                "push   bp      ", \
                "int    10h     ", \
                "pop    bp      ", \
                parm caller [ax] [bx] [cx] [dx] value [ax];

// (width >> 3) for 640
// 80 for 320

/*
void set_pixel(uint8_t *pixels, uint16_t x, uint16_t y, uint16_t color, uint32_t plane_size, uint16_t width) 
{
    uint32_t base_offset = ((y * (width >> 3)) + (x >> 3)) & 0xFFFF;

    uint8_t mask = 0x80 >> (x & 7);
    for (int plane = 0; plane < 4; plane++) {
        if (color & (1 << plane)) {
            pixels[base_offset + (plane_size * plane)] |= mask;
        } else {
            uint8_t latch = pixels[base_offset + (plane_size * plane)];
            pixels[base_offset + (plane_size * plane)] = latch & ~mask;
        }
    }
}
*/

unsigned char Init_EGA(unsigned short width, unsigned short height, unsigned short flags, int argc, char** argv)
{
	union REGS regs;
	
	switch(width)
	{
		// EGA 320x200 16 colors
		case 320:
			set_mode(0x0D);
			PLANE_SIZE = 8000;
			screen_width = 320;
			screen_height = 200;
			precalc_offset = 4;
		break;
		
		case 640:
			// EGA 640x350 16 colors
			if (height == 350)
			{
				set_mode(0x10);
				PLANE_SIZE = 28000;
				screen_width = 640;
				screen_height = 350;
			}
			// EGA 640x200 16 colors
			else
			{
				set_mode(0x0E);
				PLANE_SIZE = 16000;
				screen_width = 640;
				screen_height = 200;
			}
			precalc_offset = 3;
		break;
	}
	
    outp( EGA_SC, EGA_SC_WPEN );
    outp( EGA_SC+1, 0xF );
    outp( EGA_GC, EGA_GC_MODE );
    outp( EGA_GC+1, 0 );
	
	return 0;
}

static void EGA_Load_SIF(const char *file, BITMAP *b, unsigned short s_width, unsigned short s_height, unsigned char dontdecompress)
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
		if (b->plane_size[i] < 2) b->plane_size[i] = PLANE_SIZE * (header.height / screen_height);
	}

#ifdef __DJGPP__
	if (b->data[0])
	{
		free(b->data[0]);
		b->data[i] = NULL;
	}
	b->data[0] = malloc(header.filesize);
	_dos_read(fd, b->data[0], header.filesize, &totalsize);
   	
   	b->data[1] = b->data[0] + (b->plane_size[0]);
	b->data[2] = b->data[0] + (b->plane_size[0] + b->plane_size[1]);
	b->data[3] = b->data[0] + (b->plane_size[0] + b->plane_size[1] + b->plane_size[2]);
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
			b->data[i] = malloc(PLANE_SIZE * (header.height / screen_height));
			_dos_read(fd, tmp, b->plane_size[i], &totalsize);
			CAL_LZSAExpand(tmp, b->data[i]);
		}
		
		for(i=0;i<4;i++)
		{
			b->plane_size[i] = PLANE_SIZE * (header.height / screen_height);
		}
		
		if (tmp)
		{
			free(tmp);
			tmp = NULL;
		}
	}
   	else
   	{
		for( i=0; i<4; i++ )
		{
			if (b->data[i])
			{
				free(b->data[i]);
				b->data[i] = NULL;
			}
			b->data[i] = malloc(b->plane_size[i]);
			_dos_read(fd, b->data[i], b->plane_size[i], &totalsize);
		}
	}
#endif
	 _dos_close(fd);

    if (s_width == 0) b->sprite_width = b->width;
    if (s_height == 0) b->sprite_height = b->height;
}

extern unsigned short lzsa1_decompress(void far *src, void far *dst);

void EGA_DrawBMP_static(BITMAP *bmp, short x, short y, unsigned long off)
{
	unsigned char i;
	unsigned long offset;
	offset = ((y * screen_width) + x) >> (precalc_offset) & 0xFFFF;
	
	outp( EGA_SC, EGA_SC_WPEN );
	
	// Probably far related
#if 0
	if (bmp->encoding == 5)
	{
		for( i=0; i<4; i++ )
		{
			outp( EGA_SC+1, 0x1<<i );
			CAL_LZSAExpand(bmp->data[i],vmem);

		}
		return;
	}
#endif
	
    for( i=0; i<4; i++ )
    {
        outp( EGA_SC+1, 0x1<<i );
        memcpy(vmem + offset, bmp->data[i], bmp->plane_size[i]);
    }
}

void EGA_DrawBMP_Decompress(BITMAP *bmp, short x, short y)
{
	unsigned char i;
	unsigned long offset;
	offset = ((y * screen_width) + x) >> (precalc_offset) & 0xFFFF;
	
	outp( EGA_SC, EGA_SC_WPEN );
	
	for( i=0; i<4; i++ )
	{
		outp( EGA_SC+1, 0x1<<i );
		CAL_LZSAExpand(bmp->data[i],vmem);
	}
}

static void EGA_Free_bmp(BITMAP *b)
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

void EGA_stub()
{
}

void EGA_Clear()
{
    /* enable all planes for writing */
    outp( EGA_SC, EGA_SC_WPEN );
    outp( EGA_SC+1, 0xF );

    /* clear the graphics memory */
    _fmemset( vmem, 0, PLANE_SIZE );

    /* reset cursor position for printf() */
	VideoInt( _BIOS_SET_CURPOS, 0, 0, (0&0xFF)|((0&0xFF)<<8) );
}

void EGA_Load_palette(const char *file)
{
	// Note that this is only supported for 640x350 mode (That in itself requires an EGA monitor or converter to VGA/HDMI)
	// 640x200 and 320x200 only support the default CGA palette
	int fd;
	unsigned totalsize;
	
	// Don't apply custom palette except on higher resolutions
	if (PLANE_SIZE != 28000) return;
	
	_dos_open(file, O_RDONLY, &fd);
	_dos_read(fd, VGA_8514_GAMEPAL, 16, &totalsize);
	_dos_close(fd);
}

void EGA_Apply_palette()
{
    unsigned char i;

	// Don't apply custom palette except on higher resolutions
	if (PLANE_SIZE != 28000) return;

    /* enable 0x3c0 flip-flop */
    inp(0x3da);

    /* and set the palette */
    for (i = 0; i < 16; ++i) {
        outp(0x3c0, i);
        outp(0x3c0, VGA_8514_GAMEPAL[i]);
    }

	// Apply color changes
    outp(0x3CE, 0x0F);
    outp(0x3CF, 0xFF);
    
    /* Disable Attribute Controller access. Required otherwise drawing will be impossible */
    outp(0x3C0, 0x20);
}

void EGA_Vsync()
{
	// This is like VGA i believe
	while ((inp(INPUT_STATUS) & VRETRACE)) ;
	while (!(inp(INPUT_STATUS) & VRETRACE)) ;	
}

static void EGA_Draw_Text(const char* str, unsigned short x, unsigned short y, unsigned short color_index)
{
    uint16_t screen_width_in_chars = 40;

    x = x >> 3;
    y = y >> 3;
	
	VideoInt( _BIOS_SET_CURPOS, 0, 0, (x&0xFF)|((y&0xFF)<<8) );
	puts(str);
}

static void EGA_Draw_Text_Center(const char* str, unsigned short y, unsigned short color_index)
{
    uint16_t x;
    uint16_t screen_width_in_chars = screen_width >> 3;

    x = (screen_width_in_chars - strlen(str)) >> 1;
    y = y >> 3;
	
	VideoInt( _BIOS_SET_CURPOS, 0, 0, (x&0xFF)|((y&0xFF)<<8) );
	puts(str);
}

void Kill_EGA()
{
	outp(0x3D8, 0x1E);
	set_mode(TEXT_MODE);
}


VideoDevice Normal_EGA = {
	"EGA video",
	Init_EGA,
	EGA_DrawBMP_static,
	EGA_DrawBMP_Decompress,
	EGA_stub, //Draw Sprite trans
	EGA_stub, //Draw sprite non trans
	EGA_Load_SIF,
	EGA_Load_palette,
	EGA_Apply_palette, // Set palette
	EGA_Free_bmp,
	EGA_Clear,
	EGA_stub, // Flip
	EGA_Vsync,
	Kill_EGA,
	EGA_Draw_Text, // Draw Text
	EGA_Draw_Text_Center
};

