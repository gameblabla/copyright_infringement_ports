#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <conio.h>
#include <unistd.h>
#include "generic.h"
#include "generic_io.h"
#include "graph.h"
#include "vga.h"
#include "registers.h"

unsigned char __far *VGA = (unsigned char __far*)0xA0000000;
static uint32_t PLANE_SIZE = 0;

static uint16_t color_count = 0;

/* some video BIOS stuff */
#define _BIOS_SET_MODE			0x0000
#define _BIOS_SET_CURPOS		0x0200
#define _BIOS_GET_MODE			0x0F00
#define _BIOS_SET_PALETTE		0x1000

// Vsync
#define INPUT_STATUS      	0x03da
#define VRETRACE 			0x08

/* macro to write a word to a port */
#define word_out(port,register,value) \
  outpw(port,(((word)value<<8) + register))

/* this is what'll be used to call BIOS interrupt 10 to set the video mode */
extern short VideoInt(short, short, short, short);

#pragma aux     VideoInt = \
                "push   bp      ", \
                "int    10h     ", \
                "pop    bp      ", \
                parm caller [ax] [bx] [cx] [dx] value [ax];


/* These 3 funtions are for setting the VGA screen mode*/
static void readyVgaRegs(void)
{
	int v;
	outp(0x3d4,0x11);
    v = inp(0x3d5) & 0x7f;
	outp(0x3d4,0x11);
	outp(0x3d5,v);
}
static void outReg(const Register r)
{
	switch (r.port)
	{
		/* First handle special cases: */
		case ATTRCON_ADDR:
			inp(STATUS_ADDR);  		/* reset read/write flip-flop */
			outp(ATTRCON_ADDR, r.index | 0x20);
										/* ensure VGA output is enabled */
			outp(ATTRCON_ADDR, r.value);
		break;
		case MISC_ADDR:
		case VGAENABLE_ADDR:
			outp(r.port, r.value);	/*	directly to the port */
		break;
		case SEQ_ADDR:
		case GRACON_ADDR:
		case CRTC_ADDR:
		default:						/* This is the default method: */
			outp(r.port, r.index);	/*	index to port			   */
			outp(r.port+1, r.value);/*	value to port+1 		   */
		break;
	}
}
static void outRegArray(const Register *r, int n)
{
    readyVgaRegs();
	while (n--) outReg(*r++);
}

static void plot_pixel_256(uint16_t x,uint16_t y, uint8_t color)
{
	uint16_t offset;
	outp(SC_INDEX, MAP_MASK);
	outp(SC_DATA,  1 << (x&3) );
	offset=((uint16_t)y*screen_width+x) >> 2;
	VGA[(uint16_t)offset]=color;
}

void plot_pixel_16(uint16_t x, uint16_t y, uint8_t color) {
    uint32_t base_offset;
    int plane, i;
    unsigned char mask;
    unsigned char latch;

    // Calculate the base_offset
    base_offset = ((uint32_t)y * (screen_width >> 3)) + (x >> 3);

    // Iterate through bitplanes
    for (plane = 0; plane < 4; plane++) {
        // Select the plane
        outp(SC_INDEX, MAP_MASK);
        outp(SC_DATA, 1 << i);

        mask = 0x80 >> (x & 7);

        // Check if the corresponding bit in the color needs to be set
        if ((color >> plane) & 1) {
            VGA[base_offset] |= mask;
        } else {
            latch = VGA[base_offset];
            VGA[base_offset] = latch & ~mask;
        }
    }
}

static void (*plot_pixel)(uint16_t x,uint16_t y, uint8_t color);


static void drawChar(char ch, unsigned short x, unsigned char y, unsigned char color_index)
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
		}
	}
	x += 8;
}

static unsigned char Init_VGA(unsigned short width, unsigned short height, unsigned short flags, int argc, char** argv)
{
	unsigned short i;
	dword *ptr=(dword *)VGA;
	
	screen_width = width;
	screen_height = height;

	
	#ifdef DJGPP
	if (__djgpp_nearptr_enable() == 0)
	{
		puts("Could get access to first 640K of memory.\n");
		exit(-1);
	}

	VGA+=__djgpp_conventional_base;
	my_clock = (void *)my_clock + __djgpp_conventional_base;
	#endif
	
	
	if (width == 640 && height == 480)
	{
		set_mode(0x12);
		PLANE_SIZE = (width >> 2) * height;
		#ifdef DJGPP
		__djgpp_nearptr_disable();
		#endif
		return 0;
	}

	/* set mode 13 */
	set_mode(VGA_256_COLOR_MODE);
	
	/* turn off chain-4 mode */
	word_out(SC_INDEX, MEMORY_MODE,0x06);

	/* set map mask to all 4 planes for screen clearing */
	word_out(SC_INDEX, MAP_MASK, 0xff);

	/* clear all 256K of memory */
	for(i=0;i<0x4000;i++)
		*ptr++ = 0;

	/* turn off long mode */
	word_out(CRTC_INDEX, UNDERLINE_LOCATION, 0x00);

	/* turn on byte mode */
	word_out(CRTC_INDEX, MODE_CONTROL, 0xe3);
	
	// Calculate the size of each plane
	PLANE_SIZE = (width >> 2) * height;
	
	plot_pixel = plot_pixel_256;
	color_count = 256;
    
	switch(width)
	{
		case 800:
			switch(height)
			{
				case 600:
					// flags denotes and says that SVGA modes should be used
					if (flags == 1) outRegArray(vga_planar_800x600_Special_16, sizeof(vga_planar_800x600_Special_16)/sizeof(Register));
					// Otherwise Chunky
					else 
					{
						outRegArray(vga_planar_800x600_16, sizeof(vga_planar_800x600_16)/sizeof(Register));
					}
					outpw(0x3c4, 0x0f02);	/* Enable all 4 planes */
					
					plot_pixel = plot_pixel_16;
					color_count = 16;
					
					PLANE_SIZE = (width >> 3) * height;
				break;
			}
		break;
		case 640:
			switch(height)
			{
				case 400:
					outRegArray(vga_planar_640x400_special, sizeof(vga_planar_640x400_special)/sizeof(Register));
					
					outpw(0x3c4, 0x0f02);	/* Enable all 4 planes */
				break;
			}
		break;
		case 376:
			switch(height)
			{
				case 564:
					outRegArray(vga_planar_376x564, sizeof(vga_planar_376x564)/sizeof(Register));
					outpw(0x3c4, 0x0f02);	/* Enable all 4 planes */
				break;
			}
		break;
		case 360:
			switch(height)
			{
				case 480:
					outRegArray(vga_planar_360x480_special, sizeof(vga_planar_360x480_special)/sizeof(Register));
				break;
				case 400:
					outRegArray(vga_planar_360x400, sizeof(vga_planar_360x400)/sizeof(Register));
				break;
				case 270:
					outRegArray(vga_planar_360x270, sizeof(vga_planar_360x270)/sizeof(Register));
				break;
			}
			outpw(0x3c4, 0x0f02);	/* Enable all 4 planes */
		break;
		case 320:
			switch(height)
			{
				case 240:
					// Doing it this way just to make sure we don't break stuff
					outp(MISC_OUTPUT, 0xe3);
					word_out(CRTC_INDEX, V_RETRACE_END, 0x2c); // Turn off write protect

					word_out(CRTC_INDEX, V_TOTAL, 0x0d);
					word_out(CRTC_INDEX, OVERFLOW, 0x3e);
					word_out(CRTC_INDEX, V_RETRACE_START, 0xea);
					word_out(CRTC_INDEX, V_RETRACE_END, 0xac);
					word_out(CRTC_INDEX, V_DISPLAY_END, 0xdf);
					word_out(CRTC_INDEX, V_BLANK_START, 0xe7);
					word_out(CRTC_INDEX, V_BLANK_END, 0x06);
					//outRegArray(vga_planar_320x240, sizeof(vga_planar_320x240)/sizeof(Register));
				break;
			}
		break;
	}
	
	#ifdef DJGPP
	__djgpp_nearptr_disable();
	#endif
	
	return 0;
}


/*static void VGA_Draw_bitmap(BITMAP *bmp, short x, short y, unsigned long off)
{
  uint16_t j;
  unsigned char plane;
  uint32_t screen_offset;
  uint32_t bitmap_offset;

  for(plane=0; plane<4; plane++)
  {
    outp(SC_INDEX, MAP_MASK);
    outp(SC_DATA,  1 << ((plane+x)&3) );
    bitmap_offset = 0;
    screen_offset = ((uint32_t)y*screen_width+x+plane) >> 2;
    for(j=0; j<bmp->height; j++)
    {
      memcpy(&VGA[screen_offset], &bmp->data[plane][bitmap_offset], (bmp->width >> 2));

      bitmap_offset+=bmp->width>>2;
      screen_offset+=screen_width>>2;
    }
  }
}*/

static void VGA_Draw_bitmap(BITMAP *bmp, short x, short y, unsigned long off)
{
	unsigned char plane;


	for(plane=0; plane<4; plane++)
	{
		outp(SC_INDEX, MAP_MASK);
		outp(SC_DATA,  1 << ((plane+x)&3) );
		memcpy(&VGA[((y*screen_width+x+plane) >> 2)], &bmp->data[plane][off], bmp->plane_size[plane]);
	}
}

static void VGA_Draw_bitmap_compressed(BITMAP *bmp, short x, short y)
{
	unsigned char plane;

	for(plane=0; plane<4; plane++)
	{
		outp(SC_INDEX, MAP_MASK);
		outp(SC_DATA,  1 << ((plane+x)&3) );
		CAL_LZSAExpand(bmp->data[plane],&VGA[((y*screen_width+x+plane) >> 2)]);
	}
}

/*
void fskip(FILE *fp, int ts)
{
   int i;
   for (i=0; i<ts; i++)
      fgetc(fp);
}*/


static void VGA_Load_SIF(const char *file, BITMAP *b, unsigned short s_width, unsigned short s_height, unsigned char dontdecompress)
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
		if (b->plane_size[i] < 2)
		{
			if (color_count == 16)
			{
				b->plane_size[i] = (b->sprite_width >> 3) * b->sprite_height;
			}
			else
			{
				b->plane_size[i] = (b->sprite_width >> 2) * b->sprite_height;
			}
		}
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
		tmp = malloc(64000);
		for( i=0; i<4; i++ )
		{
			if (b->data[i])
			{
				free(b->data[i]);
				b->data[i] = NULL;
			}
			b->data[i] = malloc(b->plane_size[i]);
			_dos_read(fd, tmp, b->plane_size[i], &totalsize);
			CAL_LZSAExpand(tmp, b->data[i]);
		}
		
		for(i=0;i<4;i++)
		{
			if (color_count == 16)
			{
				b->plane_size[i] = (b->sprite_width >> 3) * b->sprite_height;
			}
			else
			{
				b->plane_size[i] = (b->sprite_width >> 2) * b->sprite_height;
			}
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


static void VGA_Load_palette(const char *file)
{
	int fd;
	unsigned totalsize;
	_dos_open(file, O_RDONLY, &fd);
	_dos_read(fd, VGA_8514_GAMEPAL, color_count*3, &totalsize);
	_dos_close(fd);
}

static void VGA_Apply_palette()
{
	unsigned short i;
	outp(PALETTE_INDEX,0);	/* tell the VGA that palette data is coming. */
	for(i=0;i<color_count*3;i++)
	{
		outp(PALETTE_DATA,VGA_8514_GAMEPAL[i]);    /* write the data */
	}
}

static void VGA_Wait_for_retrace(void) 
{
	/* wait until done with vertical retrace */
	while ((inp(INPUT_STATUS) & VRETRACE)) ;
	/* wait until done refreshing */
	while (!(inp(INPUT_STATUS) & VRETRACE)) ;
}

static void VGA_Free_bmp(BITMAP *b)
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

static void VGA_Draw_Text(const char* str, unsigned short x, unsigned short y, unsigned short color_index)
{
	unsigned char i;
	for(i = 0; str[i]; i++)
		drawChar(str[i], x + (i << 3), y, color_index);
}

static void VGA_Draw_Text_Center(const char* str, unsigned short y, unsigned short color_index)
{
	unsigned char i;
	uint16_t x;
	uint16_t text_width;
	text_width = strlen(str) << 3;
	x = (screen_width - text_width) >> 1;
	
	for(i = 0; str[i]; i++)
		drawChar(str[i], x + (i << 3), y, color_index);
}

static void Kill_VGA_()
{
	set_mode(TEXT_MODE);
}

static void VGA_Clear()
{
	short j;
	unsigned char plane;
	uint16_t screen_offset;
	uint16_t bitmap_offset;
	unsigned char* src, dest;

	for(plane=0; plane<4; plane++)
	{
		outp(SC_INDEX, MAP_MASK);
		outp(SC_DATA,  1 << ((plane+0)&3) );
		bitmap_offset=0;
		memset(&VGA[plane >> 2], 0x00, PLANE_SIZE);
	}
}

static void VGA_Vsync()
{
	// This is like VGA i believe
	while ((inp(INPUT_STATUS) & VRETRACE)) ;
	while (!(inp(INPUT_STATUS) & VRETRACE)) ;	
}

static void VGA_stub()
{
	
}


VideoDevice Planar_VGA = {
	"VGA Planar video",
	Init_VGA,
	VGA_Draw_bitmap,
	VGA_Draw_bitmap_compressed,
	VGA_stub, //Draw Sprite trans
	VGA_stub, //Draw sprite non trans
	VGA_Load_SIF,
	VGA_Load_palette,
	VGA_Apply_palette, // Set palette
	VGA_Free_bmp,
	VGA_Clear,
	VGA_stub, // Flip
	VGA_Vsync,
	Kill_VGA_,
	VGA_Draw_Text, // Draw Text
	VGA_Draw_Text_Center
};

