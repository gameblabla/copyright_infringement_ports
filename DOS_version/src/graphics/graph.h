
#ifndef _GEN_GRAPH_
#define _GEN_GRAPH_


#define VIDEO_INT           0x10      /* the BIOS video interrupt. */
#define SET_MODE            0x00      /* BIOS func to set the video mode. */
#define VGA_256_COLOR_MODE  0x13      /* use to set 256-color mode. */
#define TEXT_MODE           0x03      /* use to set 80x25 text mode. */


typedef struct tagBITMAP              /* the structure for a bitmap. */
{
	uint16_t width;
	uint16_t height;
	uint16_t sprite_width;
	uint16_t sprite_height;
	uint32_t filesize;
	//uint8_t filename[31]; // For 8514/XGA
	uint8_t encoding;
    uint32_t plane_size[4];
	uint8_t __far *data[4];
	uint8_t cleared;
} BITMAP;

#pragma pack(push, 1)
typedef struct {
	uint8_t version;
    uint16_t width;
    uint16_t height;
    uint16_t sprite_width;
    uint16_t sprite_height;
    uint8_t bytespp;
    uint8_t encoding;
    uint8_t endianess;
    uint32_t filesize;
    uint16_t plane_size[4];
} BMPHeader;
#pragma pack(pop)

#define LZSA2_COMPRESSION_ID 5
#define APLIB_COMPRESSION_ID 6

extern void set_mode(uint8_t mode);
extern uint8_t VGA_8514_GAMEPAL[768];

typedef struct VideoDevice {
	/* * * */
	/* The name of this video driver */
	const char *name;

	unsigned char (*SetVideoMode)(unsigned short width, unsigned short height, unsigned short flags, int argc, char** argv);

	void (*DrawBMP_static)(BITMAP *bmp, short x, short y, unsigned long offset);
	void (*DrawBMP_compressed)(BITMAP *bmp, short x, short y);
	
	
	void (*DrawBMP_sprite_trans)(BITMAP *bmp, short x, short, unsigned char frame);
	void (*DrawBMP_sprite_notrans)(BITMAP *bmp, short x, short, unsigned char frame);
	
	void (*LoadBMP)(const char *file, BITMAP *bmp, unsigned short s_width, unsigned short s_height, unsigned char dontdecompress);
	
	void (*LoadPalette)(const char *file);
	
	void (*SetPalette)();
	
	void (*FreeBMP)(BITMAP* bmp);
	
	void (*ClearVideo)();
	void (*FlipVideo)();
	
	void (*Vsyncwait)();
	void (*FreeVideo)();
	
	void (*Print_text)(const char* str, unsigned short x, unsigned short y, unsigned short color_index);
	
	void (*Print_text_Center)(const char* str, unsigned short y, unsigned short color_index);
	
} VideoDevice;

extern VideoDevice Planar_VGA;
extern VideoDevice Normal_CGA;
extern VideoDevice Normal_VESA;
extern VideoDevice Normal_EGA;
extern VideoDevice Normal_HGC;
extern VideoDevice Normal_HGCC;
extern VideoDevice Normal_8514;
extern VideoDevice Normal_Tandy;
extern VideoDevice Normal_MCGA;

#endif
