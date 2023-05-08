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
#ifdef DJGPP
byte *VGA = (byte *)0xA0000;          /* this points to video memory. */
#else
byte *VGA=(byte *)0xA0000000L;        /* this points to video memory. */
#endif

uint8_t VGA_8514_GAMEPAL[769];

static uint32_t total_plane_size = 0;



void _VGA_Set_Unchained_Mode(unsigned short width, unsigned short height)
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
	total_plane_size = (width >> 2) * height;
    
	switch(width)
	{
		case 640:
			switch(height)
			{
				case 400:
					outRegArray(vga_planar_640x400_special, sizeof(vga_planar_640x400_special)/sizeof(Register));
				break;
			}
		break;
		case 376:
			switch(height)
			{
				case 564:
					outRegArray(vga_planar_376x564, sizeof(vga_planar_376x564)/sizeof(Register));
				break;
			}
		break;
		case 360:
			switch(height)
			{
				case 400:
					outRegArray(vga_planar_360x400, sizeof(vga_planar_360x400)/sizeof(Register));
				break;
				case 270:
					outRegArray(vga_planar_360x270, sizeof(vga_planar_360x270)/sizeof(Register));
				break;
			}
		break;
		case 320:
			switch(height)
			{
				case 240:
					outRegArray(vga_planar_320x240, sizeof(vga_planar_320x240)/sizeof(Register));
				break;
			}
		break;
	}
	
	outpw(0x3c4, 0x0f02);	/* Enable all 4 planes */
	
	#ifdef DJGPP
	__djgpp_nearptr_disable();
	#endif
}

void _VGA_Draw_bitmap(PLANAR_BITMAP *bmp, short x, short y)
{
  short j;
  unsigned char plane;
  uint16_t screen_offset;
  uint16_t bitmap_offset;

  for(plane=0; plane<4; plane++)
  {
    outp(SC_INDEX, MAP_MASK);          /* select plane */
    outp(SC_DATA,  1 << ((plane+x)&3) );
    bitmap_offset=0;
    screen_offset = ((dword)y*screen_width+x+plane) >> 2;
    for(j=0; j<bmp->height; j++)
    {
      memcpy(&VGA[screen_offset], &bmp->data[plane][bitmap_offset], (bmp->width >> 2));

      bitmap_offset+=bmp->width>>2;
      screen_offset+=screen_width>>2;
    }
  }
}

void Load_RAW(const char* filename, PLANAR_BITMAP *b)
{
	int fd;
	int i;
	char* mem = NULL;
	uint32_t fsz;
	size_t totalsize;

	_dos_open (filename, O_RDONLY, &fd);
	if (!fd)
	{
		//printf("Can't load\n");
		exit(1);
	}
	
	for(i=0;i<4;i++)
	{
		b->data[i] = malloc(64000);
		_dos_read (fd, (char*)b->data[i], 64000, &totalsize);
		b->width = 640;
		b->height = 400;
	}

	_dos_close (fd);
}


void _VGA_Draw_Bitmap_Static(PLANAR_BITMAP *bmp)
{
	short j;
	unsigned char plane;
	uint16_t screen_offset;
	uint16_t bitmap_offset;
	unsigned char* src, dest;


	for(plane=0; plane<4; plane++)
	{
		outp(SC_INDEX, MAP_MASK);          /* select plane */
		outp(SC_DATA,  1 << ((plane+0)&3) );
		bitmap_offset=0;
		memcpy(&VGA[plane >> 2], bmp->data[plane], total_plane_size);
	}
}


void _VGA_Load_bmp(const char *file, PLANAR_BITMAP *b)
{
  FILE *fp;
  long index, size, part;
  word num_colors;
  int x;
  unsigned char plane;

  /* open the file */
  if ((fp = fopen(file,"rb")) == NULL)
  {
    //printf("Error opening file %s.\n", file);
    exit(1);
  }

  /* check to see if it is a valid bitmap file */
  if (fgetc(fp)!='B' || fgetc(fp)!='M')
  {
    fclose(fp);
    //printf("%s is not a bitmap file.\n",file);
    exit(1);
  }

  /* read in the width and height of the image, and the
     number of colors used; ignore the rest */
  fskip(fp,16);
  fread(&b->width, sizeof(word), 1, fp);
  fskip(fp,2);
  fread(&b->height,sizeof(word), 1, fp);
  fskip(fp,22);
  fread(&num_colors,sizeof(word), 1, fp);
  fskip(fp,6);

  /* assume we are working with an 8-bit file */
  if (num_colors==0) num_colors=256;

  size=b->width*b->height;

  /* try to allocate memory */
  for(plane=0;plane<4;plane++)
  {
    if ((b->data[plane] = (byte *) malloc((word)(size>>2))) == NULL)
    {
      fclose(fp);
      //printf("Error allocating memory for file %s.\n", file);
      exit(1);
    }
  }
  
  fskip(fp,1024);
  
  /* read the bitmap */
  for(index = (b->height-1)*b->width; index >= 0;index-=b->width)
  {
    for(x = 0; x < b->width; x++)
    {
      b->data[x&3][(int)((index+x)>>2)]=(byte)fgetc(fp);
	}
  }

  fclose(fp);
}

void _VGA_Load_palette(const char *file)
{
	int fd;
	unsigned totalsize;
	_dos_open(file, O_RDONLY, &fd);
	_dos_read(fd, VGA_8514_GAMEPAL, sizeof(VGA_8514_GAMEPAL), &totalsize);
	_dos_close(fd);
}

void _VGA_Set_palette()
{
	unsigned short i;
	outp(PALETTE_INDEX,0);	/* tell the VGA that palette data is coming. */
	for(i=0;i<256*3;i++)
	{
		outp(PALETTE_DATA,VGA_8514_GAMEPAL[i]);    /* write the data */
	}
}

void _VGA_Wait_for_retrace(void) 
{
	/* wait until done with vertical retrace */
	while ((inp(INPUT_STATUS) & VRETRACE)) ;
	/* wait until done refreshing */
	while (!(inp(INPUT_STATUS) & VRETRACE)) ;
}

void _VGA_Free_bmp(PLANAR_BITMAP bmp)
{
	unsigned char plane;
	for(plane=0;plane<4;plane++)
		free(bmp.data[plane]);
}

void Kill_VESA()
{
	set_mode(TEXT_MODE);
}
