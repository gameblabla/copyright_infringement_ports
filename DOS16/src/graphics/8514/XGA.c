#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <malloc.h>
#include <conio.h>
#include <unistd.h>
#include <fcntl.h>
#include "generic.h"


#include "generic.h"
#include "generic_io.h"
#include "ibmafi.h"
#include "graph.h"

static char xga_path[ 63 ];
static char file_path[ 63 ];
static char far *state_buf;

HQDPS_DATA    hqdps_data     = { 14 };             	/* Query state        */
HSPAL_DATA   *save_pal_data;                      	/* save palette       */
HOPEN_DATA    hopen_data_640_480     = { 2, 0, 1, 0 };     	/* open adapter       */
HOPEN_DATA    hopen_data_1024_768     = { 2, 0, 0, 0 }; //for 1024x768

HCLOSE_DATA   hclose_data    = { 1, 0 };           	/* close adapter      */
HINIT_DATA    hinit_data     = { 2, 0 };           	/* initialise adapter */
HQMODE_DATA   hqmode_data    = { 18 };             	/* query mode         */
HEAR_DATA     hear_data      = { 1, 0 };           	/* end area           */
HSCOL_DATA    hscol_data     = { 4, 0 };           	/* set colour         */
HINT_DATA     hint_data      = { 4, 0x80000000 };  	/* wait for event     */
HSCS_DATA     hscs_data      = { 4, 0 };           	/* set character set  */
HCHST_DATA( 128 ) hchst_data;                      	/* write string       */
HSPAL_DATA  *real_pal;


unsigned char IBM_PAL[768 + 3 + 3];

typedef struct LINE_S
{
struct LINE_S * next_ptr;
char  line[ 1 ];
} LINE_T;
static int cell_height;

void _8514_Set_palette();

static unsigned char Change_Resolution_8514(unsigned short width, unsigned short height)
{
	switch(width)
	{
		case 640:
			HOPEN( &hopen_data_640_480 );
			if ( hopen_data_640_480.iflags )
			{
				return 1;
			}
		break;
		case 1024:
			HOPEN( &hopen_data_1024_768 );
			if ( hopen_data_1024_768.iflags )
			{
				return 1;
			}
		break;
	}
	_8514_Set_palette();
	return 0;
}

static struct CharSetDef *  load_font(char* font_file )
{
    int           font_fid;
    static char   font_path[ 120 ];
    word          font_file_len;
    struct FontFileDefn * ffd_ptr;
    struct CharSetDef   * csd_ptr = NULL;

    /*----------------------------------------------------------*/

    /* Open font file as binary, read only                      */

    font_fid = open( font_file,  O_RDONLY | O_BINARY );


    if ( font_fid == -1 )
    {
        /* Can't find file in current directory so try xga path. */

        strcpy( font_path, xga_path );
        strcat( font_path, font_file );

        font_fid = open( font_path, O_RDONLY | O_BINARY );

        if ( font_fid == -1 )
        {
            /* Can't find file in xga path, so try \XGAPCDOS.    */

            strcpy( font_path, "\\XGAPCDOS\\" );
            strcat( font_path, font_file );

            font_fid = open( font_path, O_RDONLY | O_BINARY );
        }
    }

    if ( font_fid != -1 )
    {
        /* Calculate length of font file                            */

        font_file_len = ( word ) lseek( font_fid, 0L, SEEK_END );

        ffd_ptr = ( struct FontFileDefn * ) calloc( font_file_len, 1 );


        /* read font file into memory                               */

        lseek( font_fid, 0L, SEEK_SET );

        read( font_fid, ( char * ) ffd_ptr, font_file_len );


        /* Set up pointer to character set definition.        */

        csd_ptr = ( struct CharSetDef * ) ( ( char * ) ffd_ptr  +
                            ffd_ptr->page_array[ ffd_ptr->def_page ].csd_offset );


        /* Set up internal csd pointers                       */

        csd_ptr->chardef1 = ( byte far * ) ffd_ptr +  ( long )csd_ptr->chardef1;
        csd_ptr->chardef2 = ( byte far * ) ffd_ptr +  ( long )csd_ptr->chardef2;
        csd_ptr->chardef3 = ( byte far * ) ffd_ptr +  ( long )csd_ptr->chardef3;

        csd_ptr->indextbl = ( word far * ) ( ( byte far * )ffd_ptr  +
                                                ( long )csd_ptr->indextbl );

        csd_ptr->enveltbl = ( ( byte far * )ffd_ptr  + ( long )csd_ptr->enveltbl );


        /* Finished with font file. */

        close( font_fid );

        cell_height = csd_ptr->cellheight;
    }

    return csd_ptr;
}


static void _8514_Load_Font(unsigned char index)
{
	char font_file[13];
	/* Select a font depending on character size for mode       */
	if (index == 0)
	{
		switch ( hqmode_data.ac_h )
		{
			case 14:
				strcpy(font_file, "stan0814.fnt");
			break;
			case 15:
				strcpy(font_file, "stan0715.fnt");
			break;
			default:
				strcpy(font_file, "stan1220.fnt");
			break;
			/*case 23:
				sprintf(font_file, "stan1223.fnt");
			break;*/
		}
	}
	else
	{
		switch ( index )
		{
			case 1:
				strcpy(font_file, "stan0814.fnt");
			break;
			case 2:
				strcpy(font_file, "stan0715.fnt");
			break;
			default:
				strcpy(font_file, "stan1220.fnt");
			break;
		}
	}
		
     /* read font definition from file, then load adapter character set */
	if ( hscs_data.address = load_font( font_file ) )
	{
		HSCS( &hscs_data );
	}
}



#define CHUNK_SIZE 64000  // 64KB

void _8514_Load_RAW(const char *file, BITMAP *bmp, unsigned short s_width, unsigned short s_height, unsigned char load_pal)
{
    int fd;
    uint8_t i;
    
    bmp->width = s_width;
    bmp->height = s_height;
    strcpy(bmp->filename, file);
    
    // Allocate a huge far buffer
    bmp->data[0] = malloc(CHUNK_SIZE);
}


void _8514_Draw_bitmap(BITMAP *bmp, short x, short y, unsigned long offset)
{
	int fd;
	unsigned char i = 0;
	unsigned long totalBytesRead = 0;
    unsigned bytesRead;
	HBBW_DATA img;
	HBBCHN_DATA img2;
	HBBC_DATA img3;

    img.length = 18;
    img.format = 8;
    img.width = bmp->width;
    img.height = 96; // 640x480
    img.coord.x_coord = x;
    img.w = bmp->width;
    img.h = 96; // 640x480
    img.l = 0;
    img.t = 0;

    img3.length = 16;
    img3.format = 8;
    img3.width = bmp->width;
    img3.height = 64;
    img3.plane = 0;
    img3.source.x_coord = 0;
    img3.source.y_coord = 0;
    img3.dest.x_coord = 0;
    img3.dest.y_coord = 0;

    // 1024x768
    if (bmp->width == 1024)
    {
        img.h = 64;
        img.height = 64;
        img3.height = 64;
    }
	
    _dos_open(bmp->filename, O_RDONLY, &fd);
    
    if (offset > 0) lseek(fd, offset, SEEK_SET);
    
    // Read the file in 64KB chunks
    do {
        _dos_read(fd, bmp->data[0], CHUNK_SIZE, &bytesRead);
        
        HBAR();

        img.coord.y_coord = y + (i * img.h);
        
        img2.length = 6;
        img2.address = (uint8_t*)bmp->data[0];
        img2.len = bytesRead;
        
        HBBW(&img);
        HBBCHN(&img2);
        HBBC(&img3);
        
		i++;
        totalBytesRead += bytesRead;
    } while (bytesRead == CHUNK_SIZE);
    _dos_close(fd);
}

void _8514_Draw_Text(const char* str, unsigned short x, unsigned short y, unsigned short color_index)
{
	hchst_data.coord.x_coord = x;
	hchst_data.coord.y_coord = cell_height + y;
	hscol_data.index = color_index;
	
	HSCOL( &hscol_data );
	hchst_data.length = sizeof(coord_pr) + strlen( str );
	memcpy( hchst_data.string, str, strlen(str));
	HCHST( &hchst_data );
}


void _8514_Draw_Text_Center(const char* str, unsigned short y, unsigned short color_index)
{
	uint16_t x;
	x = (screen_width - (strlen(str) << 3)) >> 1;
	
	hchst_data.coord.x_coord = x;
	hchst_data.coord.y_coord = cell_height + y;
	hscol_data.index = color_index;
	
	HSCOL( &hscol_data );
	hchst_data.length = sizeof(coord_pr) + strlen( str );
	memcpy( hchst_data.string, str, strlen(str));
	HCHST( &hchst_data );
}

void _8514_Clear_Screen()
{
	HRECT_DATA quad;
	HSCOL_DATA    hscol_data     = { 4, 0 };
	HEAR_DATA     hear_data      = { 1, 0 };

	HBAR();

	hscol_data.index = 0,
	quad.coord.x_coord = 0;
	quad.coord.y_coord = 0;
	quad.width = 640;
	quad.height = 480;
	hscol_data.index = 0;
	
	HSCOL(&hscol_data);
	HRECT(&quad);
	HEAR(&hear_data);
}

void _8514_stub()
{
	// Stub
}

void _8514_Flip()
{
	// Stub
}

void _8514_Wait_for_retrace()
{
	HINT_DATA hint_data = { 4, 0x80000000 };
	HINT(&hint_data);
}

void Close_8158_AI()
{
	HRPAL( save_pal_data );
    HCLOSE( &hclose_data );
    
    if (save_pal_data != NULL) free(save_pal_data);
    if (real_pal != NULL) free(real_pal);
    if (state_buf != NULL) free(state_buf);
}


void _8514_Free_BMP(BITMAP *bmp)
{
    if (bmp->data[0])
    {
		free(bmp->data[0]);
		bmp->data[0] = NULL;
	}
}

void get_xga_path( int argc, char ** argv )
{
    /* If a parameter is passed to this program, it should      */
    /* represent the directory where the XGA Adapter software   */
    /* was installed.                                           */
    if ( argc >= 2 )
    {
        strcpy( xga_path, argv[ 1 ] );
        if ( xga_path[ strlen( xga_path ) - 1 ] != '\\' )
            strcat( xga_path, "\\" );
    }
    else
    {
        strcpy( xga_path, argv[ 0 ] );
        *( strrchr( xga_path, '\\' ) + 1 ) = '\0';
    }
}


unsigned char Init_AI_8514(unsigned short width, unsigned short height, unsigned short flags, int argc, char** argv)
{
	unsigned char res;
	real_pal = malloc (768);
	real_pal->length = 768;

	memcpy(real_pal , IBM_PAL, 768);

    get_xga_path( argc, argv );
	
    if ( getafi() == NULL )
    {
		puts("No AFI Adapter detected !");
		return 1;
	}
	else
	{
		puts("An AFI Adapter was detected !");
	}

    /* Allocate Adapter interface task dependent buffer         */
    HQDPS( &hqdps_data );
    state_buf =  malloc( hqdps_data.size + 15 );

    /* Allocate palette save restore buffer                     */
    save_pal_data =  malloc( hqdps_data.palbufsize + 2 );
    save_pal_data->length = hqdps_data.palbufsize;
    
    /* save palette                                             */
    HSPAL( save_pal_data );

	res = Change_Resolution_8514(width, height);
	if (res) return 1;
	
    /* Make sure that task dependent buffer is situated on a    */
    /*                                       16 byte boundary.  */

    hinit_data.segment = FP_SEG( state_buf ) + ( ( FP_OFF( state_buf ) + 15) >> 4 );

    /* initialise XGA adapter                                   */
    HINIT( &hinit_data );
    /* load palette for demo                                    */
	HRPAL( real_pal );
    /* Find out mode and associated data                        */
    HQMODE( &hqmode_data );
    
	_8514_Load_Font(3);
	
	screen_width = width;
	screen_height = height;
	
	//Change_Resolution_8514(1024, 768);
    
    return 0;
}

void _8514_Load_palette(const char *file)
{
	int fd;
	unsigned totalsize;
	_dos_open(file, O_RDONLY, &fd);
	_dos_read(fd, IBM_PAL+3, 768, &totalsize);
	_dos_close(fd);
}

void _8514_Set_palette()
{
	memcpy(real_pal , IBM_PAL, 768);
	HRPAL( real_pal );
}


VideoDevice Normal_8514 = {
	"XGA/8514 AI video",
	Init_AI_8514,
	_8514_Draw_bitmap,
	_8514_stub, // Draw decompress
	_8514_stub,
	_8514_stub,
	_8514_Load_RAW,
	_8514_Load_palette,
	_8514_Set_palette,
	_8514_Free_BMP,
	_8514_Clear_Screen,
	_8514_Flip,
	_8514_Wait_for_retrace,
	Close_8158_AI,
	_8514_Draw_Text,
	_8514_Draw_Text_Center
};
