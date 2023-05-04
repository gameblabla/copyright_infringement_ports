#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <libgen.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>


typedef struct tagBITMAP              /* the structure for a bitmap. */
{
  uint16_t width;
  uint16_t height;
  uint8_t *data;
  uint8_t *pdata[4];
  uint16_t sprite_width;
  uint16_t sprite_height;
  uint8_t bytespp;
} BITMAP;


uint32_t key_pressed = 0;
uint8_t str[64];
uint8_t tmp_str[64];

int spr = 0;

uint8_t VGA_8158_GAMEPAL[768];

typedef enum {
    STR2INT_SUCCESS,
    STR2INT_OVERFLOW,
    STR2INT_UNDERFLOW,
    STR2INT_INCONVERTIBLE
} str2int_errno;

void fskip(FILE *fp, int num_bytes)
{
   int i;
   for (i=0; i<num_bytes; i++)
      fgetc(fp);
}


str2int_errno str2int(short *out, char *s, int base) {
    char *end;
    if (s[0] == '\0' || isspace(s[0]))
        return STR2INT_INCONVERTIBLE;
    errno = 0;
    long l = strtol(s, &end, base);
    /* Both checks are needed because INT_MAX == LONG_MAX is possible. */
    if (l > INT_MAX || (errno == ERANGE && l == LONG_MAX))
        return STR2INT_OVERFLOW;
    if (l < INT_MIN || (errno == ERANGE && l == LONG_MIN))
        return STR2INT_UNDERFLOW;
    if (*end != '\0')
        return STR2INT_INCONVERTIBLE;
    *out = l;
    return STR2INT_SUCCESS;
}

static char *removestr(char* myStr)
{
    char *retStr;
    char *lastExt;
    if (myStr == NULL) return NULL;
    if ((retStr = malloc (strlen (myStr) + 1)) == NULL) return NULL;
    strcpy (retStr, myStr);
    lastExt = strrchr (retStr, '.');
    if (lastExt != NULL)
        *lastExt = '\0';
    return retStr;
}

static void Load_bmp(const char *file, BITMAP *b, unsigned short s_width, unsigned short s_height)
{
	FILE *fp;
	long long index;
	uint16_t num_colors;
	long x;
  
	/* open the file */
	if ((fp = fopen(file,"rb")) == NULL)
	{
		printf("Error opening file %s.\n",file);
		exit(1);
	}
  
	/* check to see if it is a valid bitmap file */
	if (fgetc(fp)!='B' || fgetc(fp)!='M')
	{
		fclose(fp);
		printf("%s is not a bitmap file.\n",file);
		exit(1);
	}
	
	/* read in the width and height of the image, and the
	number of colors used; ignore the rest */
	fskip(fp,16);
	fread(&b->width, sizeof(uint16_t), 1, fp);
	fskip(fp,2);
	fread(&b->height,sizeof(uint16_t), 1, fp);
	fskip(fp,22);
	fread(&num_colors,sizeof(uint16_t), 1, fp);
	fskip(fp,6);

	/* assume we are working with an 8-bit file */
	if (num_colors==0) num_colors=256;

	/* try to allocate memory */
	if ((b->data = malloc((b->width*b->height))) == NULL)
	{
		fclose(fp);
		printf("Error allocating memory for file %s.\n",file);
		exit(1);
	}
  
	fskip(fp,num_colors*4);
	
	/* read the bitmap */
	for(index = (b->height-1)*b->width; index >= 0;index-=b->width)
	{
		for(x = 0; x < b->width; x++)
		{
			b->data[(int)((index+x))]=fgetc(fp);
		}
	}
	fclose(fp);
  
	b->sprite_width = s_width;
	b->sprite_height = s_height;
}

static void Load_palette(const char *file)
{
	FILE *fp;
	long index;
	uint16_t num_colors = 256;
  
	/* open the file */
	if ((fp = fopen(file,"rb")) == NULL)
	{
		printf("Error opening file %s.\n",file);
		exit(1);
	}
	/* check to see if it is a valid bitmap file */
	if (fgetc(fp)!='B' || fgetc(fp)!='M')
	{
		fclose(fp);
		printf("%s is not a bitmap file.\n",file);
		exit(1);
	}

	//fskip(fp,52);
	fseek(fp, 52 + 2, SEEK_SET);

	/* assume we are working with an 8-bit file */
	if (num_colors==0) num_colors=256;
  
  /* read the palette information */
	for(index=0;index<num_colors;index++)
	{
		VGA_8158_GAMEPAL[(int)(index*3+2)] = fgetc(fp);
		VGA_8158_GAMEPAL[(int)(index*3+1)] = fgetc(fp);
		VGA_8158_GAMEPAL[(int)(index*3+0)] = fgetc(fp);
		fgetc(fp);
	}
	
	fclose(fp);
}

static void PNG_Load_palette(const char *file)
{
	FILE *fp;
	long index;
	uint16_t num_colors = 256;
	int done = 1;
	int yes = 1;
	/* open the file */
	if ((fp = fopen(file,"rb")) == NULL)
	{
		printf("Error opening file %s.\n",file);
		exit(1);
	}

	//fskip(fp,52);
	while(done)
	{
		/* Find PLTE identifier, which is then followed by the palette in RGB24 format */
		if (fgetc(fp) == 'P')
		{
			if (fgetc(fp) == 'L')
			{
				fgetc(fp);
				fgetc(fp);
				fgetc(fp);
				fgetc(fp);
				done = 0;
			}
		}
	}

	/* assume we are working with an 8-bit file */
	if (num_colors==0) num_colors=256;
  
  /* read the palette information */
	for(index=0;index<num_colors;index++)
	{
		/*fread(&VGA_8158_GAMEPAL[(int)(index*3)], 1, sizeof(uint16_t), fp);
		fread(&VGA_8158_GAMEPAL[(int)(index*3+1)], 1, sizeof(uint16_t), fp);
		fread(&VGA_8158_GAMEPAL[(int)(index*3+2)], 1, sizeof(uint16_t), fp);*/
		VGA_8158_GAMEPAL[(int)(index*3)] = fgetc(fp);
		VGA_8158_GAMEPAL[(int)(index*3+1)] = fgetc(fp);
		VGA_8158_GAMEPAL[(int)(index*3+2)] = fgetc(fp);
		fgetc(fp);
	}
	
	fclose(fp);
}

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

BMPHeader header;
BITMAP bmp;
	
int main (int argc, char *argv[]) 
{
	FILE *fp;

	int noheader = 0;
	int compressed = 0;
	uint8_t stub = 0;
	uint16_t in, i;
	
	header.version = 2;
	header.encoding = 0;
	// 8 for Little endian, 0 for Big endian
	header.endianess = 8;
	
	for(i=0;i<4;i++)
	{
		header.plane_size[i] = 0;
	}
	
	char str[128];
	
	if (argc < 7)
	{
		printf("BMP/PNG/JPEG to 8bpp raw or SIF\nThis is useful for converting images or FC8/LZ4W/NRV2 to\na custom RAW 8bpp image with a custom header for MSDOS/PC-98.\n");
		printf("imgtosif in.bmp/in.fc8 out.raw bitmap_width bitmap_height sprite_width sprite_height noheader\n");
		
		printf("\nFor background images, just enter the same value for sprite_width and bitmap_width and so on.\n");
		
		return 0;
	}
	

	str2int(&header.sprite_width, argv[5], 10);
	str2int(&header.sprite_height, argv[6], 10);
	printf("Sprite width : %d pixels\n", header.sprite_width);
	printf("Sprite height : %d pixels\n", header.sprite_height);
	
	snprintf(str, sizeof(str), "%s.PAL", removestr(basename(argv[1])));
	
	if (argc == 8)
	{
		if (strstr (argv[7],"noheader"))
		{
			printf("No header mode, output as a RAW Big-Endian GRB555 format.");
			noheader = 1;
		}
	}

	if (strstr (argv[1],".bmp") != NULL || strstr (argv[1],".BMP") != NULL)
	{
		printf("This is a BMP format, use built-in converter\n");
		
		Load_bmp(argv[1], &bmp, header.sprite_width, header.sprite_height);
		header.bytespp = 1;
		
		Load_palette(argv[1]);
		
		fp = fopen(str, "wb");
		fwrite(&VGA_8158_GAMEPAL, 1, 256 * 3, fp);
		fclose(fp);
	}
	else if (strstr (argv[1],".raw") || strstr (argv[1],".RAW"))
	{
		str2int(&header.width, argv[3], 10);
		str2int(&header.height, argv[4], 10);
		compressed = 0;
		header.bytespp = 1;
		
		fp = fopen(argv[1], "rb");
		fseek (fp , 0 , SEEK_END);
		header.filesize = ftell (fp);
		fseek (fp , 0 , SEEK_SET);

		// allocate memory to contain the whole file:
		bmp.data = (char*) malloc (sizeof(char)*header.filesize);
		
		// Assume that bitdepth is 8bpp
		header.bytespp = 1;
		
		// copy the file into the buffer:
		fread (bmp.data,1,header.filesize,fp);
		
		fclose(fp);
	}
	else
	{
		str2int(&header.width, argv[3], 10);
		str2int(&header.height, argv[4], 10);
		compressed = 1;
		if (strstr (argv[1],".fc8"))
		{
			printf("Custom binary format is FC8\n");
			header.encoding = 1;
		}
		else if (strstr (argv[1],".lzw"))
		{
			printf("Custom binary format is LZ4W (by Stephane)\n");
			header.encoding = 2;
		}
		else if (strstr (argv[1],".nv"))
		{
			printf("Custom binary format is nrv2s\n");
			header.encoding = 3;
		}
		else if (strstr (argv[1],".lz4"))
		{
			printf("Custom binary format is LZ4\n");
			header.encoding = 4;
		}
		else if (strstr (argv[1],".lz1"))
		{
			printf("Custom binary format is LZSA1\n");
			header.encoding = 5;
		}
		else if (strstr (argv[1],".ap"))
		{
			printf("Custom binary format is APLIB\n");
			header.encoding = 6;
		}
		else if (strstr (argv[1],".pcf"))
		{
			printf("Custom binary format is PackFire\n");
			header.encoding = 7;
		}
		else if (strstr (argv[1],".zx0"))
		{
			printf("Custom binary format is ZX0\n");
			header.encoding = 8;
		}
		else
		{
			printf("Unknown format. Make sure that the file extension is set appropriately.\n");
			return -1;
		}
		
		printf("We will just append a header to it.\n(using the info provided through the command line)\n");
		
		fp = fopen(argv[1], "rb");
		fseek (fp , 0 , SEEK_END);
		header.filesize = ftell (fp);
		fseek (fp , 0 , SEEK_SET);
		
		
		// Assume that bitdepth is 8bpp
		header.bytespp = 1;
		
		if (header.encoding > 0)
		{
			if (argc == 11)
			{
				printf("Setting offsets (Planar based) within the (compressed) file\n");
				// Number of planes used
				str2int((short*)&header.plane_size[0], argv[7], 10);
				str2int((short*)&header.plane_size[1], argv[8], 10);
				str2int((short*)&header.plane_size[2], argv[9], 10);
				str2int((short*)&header.plane_size[3], argv[10], 10);
				
				for(i=0;i<4;i++)
				{
					printf("Plane %d size %d\n", i, header.plane_size[i]);
				}
			}
		}

		// allocate memory to contain the whole file:
		bmp.data = (char*) malloc (sizeof(char)*header.filesize);
		
		// copy the file into the buffer:
		fread (bmp.data,1,header.filesize,fp);
		
		fclose(fp);
	}
	
	fp = fopen(argv[2], "wb");
	if (!fp) 
	{
		printf("Cannot write to output file (read-only filesystem or lack of free space perhaps ?)\n");
		return 1;
	}
	
	if (noheader == 0)
	{
		printf("Writing file of Filesize is %d, size of header %d\n", header.filesize, sizeof(header));
		fwrite (&header, sizeof(char), sizeof(header), fp);
		//fwrite (&stub, sizeof(char), sizeof(stub), fp);
		//fwrite (&stub, sizeof(char), sizeof(stub), fp);
		//fwrite (&stub, sizeof(char), sizeof(stub), fp);
		//fwrite (&stub, sizeof(char), sizeof(stub), fp);
	}
	
	if (compressed == 0)
	{
		fwrite (bmp.data , sizeof(char), header.filesize, fp);
	}
	else
	{
		fwrite (bmp.data , sizeof(char), header.filesize, fp);
	}
	
	fclose(fp);
	free(bmp.data);
	
	return 0;
}
