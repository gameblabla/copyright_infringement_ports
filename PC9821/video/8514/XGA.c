
    /********************************************************/
    /*                                                      */
    /*                                                      */
    /*            XGA Adapter Demonstration Program         */
    /*                                                      */
    /*     Compile this module, then link to CALLAFI.OBJ    */
    /*                                                      */
    /*               Version 1.01  7/30/91                  */
    /*                                                      */
    /********************************************************/



/**************************************************/
/*                                                */
/*          Include files                         */
/*                                                */
/**************************************************/
#include <stdlib.h>
#include <string.h>
#include <dos.h>
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

char * demo_files[] = { "xgademo1.msg"};

unsigned char IBM_PAL[768 + 3 + 3];

    typedef struct LINE_S
                   {
                       struct LINE_S * next_ptr;
                       char  line[ 1 ];
                   } LINE_T;
static int cell_height;

void _8514_Load_Font(unsigned char index);

struct CharSetDef *  load_font( font_file )

    char  * font_file;
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



void _8514_Load_BMP(BITMAP *bmp, const char *file, unsigned long ww, unsigned long hh, unsigned char bitplanes)
{
	int fd;
	uint8_t i;
	unsigned long temp;
	unsigned totalsize;
	uint16_t y, increment;
	
	bmp->width = ww;
	bmp->height = hh;
	bmp->num_planes = bitplanes;
	bmp->num_planes++;
	
	for(i=0;i<bmp->num_planes;i++)
	{
		bmp->pdata[i] = malloc(64000);
	}
	
	_dos_open(file, O_RDONLY, &fd);
	for(i=0;i<bmp->num_planes;i++)
	{
		_dos_read(fd, bmp->pdata[i], 64000, &totalsize);
	}
	_dos_close(fd);
}

void _8514_Draw_bitmap(BITMAP *bmp, short x, short y)
{
	unsigned short i;
	unsigned short STEPS_PICT = bmp->height;
	unsigned char a;
	//HRECT_DATA quad;
	
	HBBW_DATA img;
	HBBCHN_DATA img2;
	HBBC_DATA img3;
	
	for(i=0;i<bmp->num_planes;i++)
	{
		HBAR();
		img.length = 10;
		img.format = 8;
		img.width = bmp->width;
		img.height = bmp->height;
		img.coord.x_coord = x;
		img.coord.y_coord = y + (i * 100);
		
		img2.length = 6;
		img2.address = bmp->pdata[i];
		img2.len = 64000;
		
		img3.length = 16;
		img3.format = 8;
		img3.width = bmp->width;
		img3.height = 96;
		img3.plane = 0;
		img3.source.x_coord = 0;
		img3.source.y_coord = 0;
		img3.dest.x_coord = 0;
		img3.dest.y_coord = 0;
		
		HBBW(&img);
		HBBCHN(&img2);
		HBBC(&img3);
		
		HEAR(&hear_data);
	}
	

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

void Free_8158_AI()
{
    if (save_pal_data != NULL) free(save_pal_data);
    if (real_pal != NULL) free(real_pal);
    if (state_buf != NULL) free(state_buf);
}

void Free_bitmap(unsigned char* data)
{
    if (data != NULL) free(data);
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


static int Init_AI_8514(unsigned short width, unsigned short height, unsigned short flags, int argc, char** argv)
{
	real_pal = malloc (768);
	real_pal->length = 768;

	memcpy(real_pal , IBM_PAL, 768);

    get_xga_path( argc, argv );
	
    if ( getafi() == NULL )
    {
		printf("No AFI Adapter detected !\n");
		return 1;
	}
	else
	{
		printf("An AFI Adapter was detected !\n");
	}

    /* Allocate Adapter interface task dependent buffer         */
    HQDPS( &hqdps_data );
    state_buf =  malloc( hqdps_data.size + 15 );

    /* Allocate palette save restore buffer                     */
    save_pal_data =  malloc( hqdps_data.palbufsize + 2 );
    save_pal_data->length = hqdps_data.palbufsize;
    
    /* save palette                                             */
    HSPAL( save_pal_data );

    /* Attempt to open XGA adapter in mode 0 ( 1024 * 768 ). If the attached */
    /* monitor does not support this mode, or there is insufficient vram,    */
    /* then the XGA adapter may open in another mode.                        */
    HOPEN( &hopen_data_640_480 );

    if ( hopen_data_640_480.iflags )
    {
		printf("Can't open AI\n");
		//error_exit( OPEN_FAILED_MSG, ( byte )hopen_data.iflags );
	}
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

void Apply_Palette()
{
	memcpy(real_pal , IBM_PAL, 768);
	HRPAL( real_pal );
}

void Change_Resolution_8514(unsigned short width, unsigned short height)
{
	switch(width)
	{
		case 640:
			HOPEN( &hopen_data_640_480 );
		break;
		case 1024:
			HOPEN( &hopen_data_1024_768 );
		break;
	}
	Apply_Palette();
}

void _8514_Load_Font(unsigned char index)
{
	char font_file[13];
	/* Select a font depending on character size for mode       */
	if (index == 0)
	{
		switch ( hqmode_data.ac_h )
		{
			case 14:
				sprintf(font_file, "stan0814.fnt");
			break;
			case 15:
				sprintf(font_file, "stan0715.fnt");
			break;
			default:
				sprintf(font_file, "stan1220.fnt");
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
				sprintf(font_file, "stan0814.fnt");
			break;
			case 2:
				sprintf(font_file, "stan0715.fnt");
			break;
			default:
				sprintf(font_file, "stan1220.fnt");
			break;
		}
	}
		
     /* read font definition from file, then load adapter character set */
	if ( hscs_data.address = load_font( font_file ) )
	{
		HSCS( &hscs_data );
	}
}

void error_exit( int msg_id, int error_num )
{
    HCLOSE( &hclose_data );
    HRPAL( save_pal_data );
    exit(1);
}

VideoDevice Normal_PC9821_PGC = {
	"PC9821 PGC video",
	PC9821_SetVideo,
	PC9821_Draw_static_bitmap_normal,
	PC9821_Draw_sprite_normal_trans,
	PC9821_Draw_sprite_normal_notrans,
	PC9821_Load_SIF,
	PC9821_Load_palette,
	PC9821_Set_palette,
	PC9821_Free_bmp,
	PC9821_Clear_Screen,
	PC9821_Flip,
	PC9821_Wait_for_retrace,
	PC9821_Kill,
	PC9821_Print_Text
};
