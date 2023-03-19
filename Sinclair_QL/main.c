#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <qdos.h>
#include <dirent.h>
#include <stdio.h>

#define uint32_t unsigned short
#define int32_t short
#define uint8_t unsigned char
#define uint16_t unsigned short

#ifndef DEVICE_LOADFROM
#error "Define DEVICE_LOADFROM to something like flp1 or mdv1"
#endif

char main_device_name[6] = DEVICE_LOADFROM"_";

unsigned char wait = 0;
unsigned char game_mode = 0;
unsigned char text_progress = 0;
unsigned char status = 0;
unsigned char time_game = 0;
unsigned char i = 0;
unsigned short global_score = 0;

static unsigned short keys;
static const long IPCcmd [2] = { 0x09010000L, 0x00000102L };

const char bad_end1[] = "Wow, you actually suck!\nGuess you'll stay there for a \nwhile.\nHaha!";
const char bad_end2[] = "Not bad but you can do better!\nOtherwise you can't leave this place!";
const char good_end[] = "Haha, congrats! I'll leave you\nalone...For now, anyway.\nI'll see you later, bye for now.";

unsigned char FRAME_CURRENT = 0;

// The "1" in the second long is the keyrow #

#define SCREEN_MODE_ADDR ((volatile unsigned char*)0x18063)

#define SET_HIGH_RESOLUTION() *SCREEN_MODE_ADDR = (0 << 3);
#define SET_LOW_RESOLUTION() *SCREEN_MODE_ADDR = (1 << 3);

#define SCR8_START_ADDRESS ((volatile void*)0x20000)


#define TEMP_RAM_AREA_ALT_HIGH_MEMORY ((volatile void*)0x40000) 
// Assume default 128KB configuration
// If we attempt to go further, we overwrite our own memory
#define TEMP_RAM_AREA_ALT_128K_CONFIG ((volatile void*)0x35000) 

//0x3A000 used to work but not on Q-emulator...
//Oh well, the fun of manually managing memory !

// Screen 2 area
// Stock ROM can go further at 0x2A000
// but Minerva, this is as far as we can go...
#define TEMP_RAM_AREA ((volatile void*)0x2C000)


WINDOWDEF_t my_windef = { 
		WHITE_M4, 0, 
		BLACK_M4, 
		RED_M4, 
		512, 40, 
		0, 256-40
};

WINDOWDEF_t small_windef = { 
		WHITE_M4, 0, 
		BLACK_M4, 
		RED_M4, 
		256, 16, 
		48, 176
};

WINDOWDEF_t score_windef = { 
		WHITE_M4, 0, 
		BLACK_M4, 
		RED_M4, 
		256, 16, 
		48, 212
};

WINDOWDEF_t full_windef = { 
		WHITE_M4, 0, 
		BLACK_M4, 
		RED_M4, 
		512, 256, 
		0, 0
};

WINDOWDEF_t empty = { 
		WHITE_M4, 0, 
		BLACK_M4, 
		RED_M4, 
		0, 0, 
		0, 0
};

chanid_t in, in2;

#define Init_text() in =  ut_con ( &my_windef );  sd_setsz( in, 1, 1, 0 ); sd_setin( in, 1, WHITE_M4 ); sd_clear(in, 1 );
#define Initfull_text() in =  ut_con ( &full_windef ); sd_setsz( in, 1, 1, 0 ); sd_setin( in, 1, WHITE_M4 ); sd_clear(in, 1 );

#define Init_Game_text() in =  ut_con ( &small_windef ); sd_setsz( in, 1, 1, 0 ); sd_setin( in, 1, WHITE_M4 ); sd_clear(in, 1 );
#define Init_Game_text_Score() in2 =  ut_con ( &score_windef ); sd_setsz( in2, 1, 1, 0 ); sd_setin( in2, 1, WHITE_M4 ); sd_clear(in2, 1 );

#define Disable_text() in =  ut_con ( &empty ); in2 =  ut_con ( &empty ); sd_clear(in, 1 ); sd_clear(in2, 1 );

const unsigned char text_minigame_1[] = 
{
	"Your goal is to FUCK Zhongli!\nYou must follow the instructionsto get more points.\nPenetrate him by pressing a key then pull out by releasing it. \nGood luck !\n\nPress any key to start"
};

void Print_text(const char* str, unsigned char text_size)
{
	char buffer[189];
	QLSTR_t* p_qlstr;

	sd_clear(in, 1 );
	
	p_qlstr = (QLSTR_t*)buffer;
	sprintf( p_qlstr->qs_str, str);
	p_qlstr->qs_strlen = text_size;
	ut_mtext( in, p_qlstr );
}

void Print_score()
{
	sd_clear(in2, 1 );
	ut_mint(in2, global_score);
}

unsigned char open_image_to_buffer(const char* fname, void* buf, unsigned short fsize)
{
    chanid_t fil;
    char temp[16];
    sprintf(temp, "%s%s", main_device_name, fname);
    
    fil = io_open(temp, 0x01);
    if (fil < 0) return 1;
	fs_load(fil, buf, fsize);
	io_close(fil);

	return 0;
}

extern void zx0_decompress(void *src, const void *dst);
extern unsigned int lz4w_unpack(void * source, void *destination);

void switch_gamemode(unsigned char mode);

#define Q_KEY_UP 4
#define Q_KEY_DOWN 128
#define Q_KEY_LEFT 2
#define Q_KEY_RIGHT 16
#define Q_KEY_SPACE 64

unsigned long frames_offset[]=
{
	TEMP_RAM_AREA + 0, TEMP_RAM_AREA + 5610, TEMP_RAM_AREA + 5610+5854, 
	TEMP_RAM_AREA_ALT_128K_CONFIG + 0, TEMP_RAM_AREA_ALT_128K_CONFIG + 5876,
};

#define KEEPITUP_GAME_STATUS 0
#define STOP_GAME_STATUS 1
#define FUCK_GAME_STATUS 2
#define END_GAME_STATUS 3

const char ingame_quote[5][12] =
{
	"KEEP IT UP!",
	"STOP       ",
	"FUCK HIM ! ",
	"FINISH !   ",
};

const unsigned char status_level1[] =
{ 
	FUCK_GAME_STATUS, KEEPITUP_GAME_STATUS, 
	STOP_GAME_STATUS, FUCK_GAME_STATUS, KEEPITUP_GAME_STATUS, 
	STOP_GAME_STATUS, FUCK_GAME_STATUS, KEEPITUP_GAME_STATUS, 
	STOP_GAME_STATUS, FUCK_GAME_STATUS,
	END_GAME_STATUS, END_GAME_STATUS 
};

extern void WAITVBLANK();

static unsigned short music_tick = 0;
static unsigned short note_index = 0;
static const unsigned short note_delay = 7;
unsigned char music_toplay = 99;
static const unsigned short MUSIC_NOTES[][5] =
{
	// Music 1
	{285, 0x50, 0x10, 0xAA, 0x99},
	{1675, 0xA3, 0x5F, 0x12, 0x38},
	{1090, 0x7C, 0x22, 0x78, 0xCD},
	{2200, 0x11, 0x01, 0xEF, 0x4D},
	{1200, 0xD2, 0x6B, 0x33, 0x8F},
	{1620, 0x99, 0x2C, 0x5A, 0x77},
	{1425, 0xB8, 0x40, 0xC3, 0x2E},
	{2800, 0x3F, 0x15, 0x91, 0x72},
	{2355, 0x22, 0x0A, 0x66, 0xB5},
	{2050, 0x88, 0x4B, 0x1A, 0xFF},
	// Music 2
	{350, 0x60, 0x20, 0xAA, 0x99},
	{1875, 0xB4, 0x4F, 0x22, 0x43},
	{940, 0x8C, 0x15, 0x68, 0xDE},
	{2350, 0x1E, 0x0D, 0xD7, 0x6C},
	{1300, 0xE3, 0x5E, 0x3F, 0x9A},
	{1920, 0xA2, 0x2F, 0x61, 0x44},
	{1600, 0xC1, 0x42, 0x88, 0xC8},
	{2900, 0x47, 0x1A, 0x72, 0x91},
	{2485, 0x2A, 0x0F, 0x55, 0x36},
	{2150, 0x90, 0x5A, 0x1B, 0xEE},
	// Music 3
	{1500, 0x60, 0x20, 0xAA, 0x99},
	{1200, 0x8C, 0x1E, 0x68, 0xDE},
	{1600, 0x90, 0x1C, 0x61, 0x44},
	{1500, 0x5E, 0x00, 0x6A, 0x38},
	{1200, 0x7C, 0x2F, 0x78, 0xCD},
	{1500, 0x5B, 0x00, 0x6A, 0x38},
	{1200, 0x7C, 0x2F, 0x78, 0xCD},
	{1500, 0x5E, 0x00, 0x6A, 0x38},
	{1200, 0x8C, 0x1E, 0x68, 0xDE},
	{1500, 0x60, 0x20, 0xAA, 0x99},
	{1200, 0x8C, 0x1E, 0x68, 0xDE},
	{1600, 0x90, 0x1C, 0x61, 0x44},
	{1500, 0x5E, 0x00, 0x6A, 0x38},
	{1200, 0x7C, 0x2F, 0x78, 0xCD},
	{1500, 0x5B, 0x00, 0x6A, 0x38},
	{1200, 0x7C, 0x2F, 0x78, 0xCD},
	{1500, 0x5E, 0x00, 0x6A, 0x38},
	{1200, 0x8C, 0x1E, 0x68, 0xDE},
	{2000, 0x88, 0x34, 0x1A, 0xFF},
	{1500, 0x60, 0x20, 0xAA, 0x99},
	{1200, 0x8C, 0x1E, 0x68, 0xDE},
	{1600, 0x90, 0x1C, 0x61, 0x44},
	{1500, 0x5E, 0x00, 0x6A, 0x38},
	{1200, 0x7C, 0x2F, 0x78, 0xCD},
	{1500, 0x5B, 0x00, 0x6A, 0x38},
	{1200, 0x7C, 0x2F, 0x78, 0xCD},
	{1500, 0x5E, 0x00, 0x6A, 0x38},
	{1200, 0x8C, 0x1E, 0x68, 0x38},
	{1200, 0x7C, 0x2F, 0x78, 0xCD},
	{1500, 0x5B, 0x00, 0x6A, 0x38},
	// Music 4
	{2500, 0x60, 0x20, 0xAA, 0x99},
	{2200, 0x8C, 0x1E, 0x68, 0xDE},
	{2500, 0x5E, 0x00, 0x6A, 0x38},
	{2200, 0x7C, 0x2F, 0x78, 0xCD},
	{2500, 0x5B, 0x00, 0x6A, 0x38},
	{2200, 0x7C, 0x2F, 0x78, 0xCD},
	{2500, 0x5E, 0x00, 0x6A, 0x38},
	{2200, 0x8C, 0x1E, 0x68, 0xDE},
	{2500, 0x60, 0x20, 0xAA, 0x99},
	{2200, 0x8C, 0x1E, 0x68, 0xDE},
	{2500, 0x5E, 0x00, 0x6A, 0x38},
	{2200, 0x7C, 0x2F, 0x78, 0xCD},
	{2500, 0x5B, 0x00, 0x6A, 0x38},
	{2200, 0x7C, 0x2F, 0x78, 0xCD},
	{2500, 0x5E, 0x00, 0x6A, 0x38},
	{2200, 0x8C, 0x1E, 0x68, 0xDE},
	{2800, 0x88, 0x34, 0x1A, 0xFF},
	{2500, 0x60, 0x20, 0xAA, 0x99},
	{2200, 0x8C, 0x1E, 0x68, 0xDE},
	{2500, 0x5E, 0x00, 0x6A, 0x38},
	{2200, 0x7C, 0x2F, 0x78, 0xCD},
	{2500, 0x5B, 0x00, 0x6A, 0x38},
	{2200, 0x7C, 0x2F, 0x78, 0xCD},
	{2500, 0x5E, 0x00, 0x6A, 0x38},
	{2200, 0x8C, 0x1E, 0x68, 0xDE},
	{2800, 0x88, 0x34, 0x1A, 0xFF},
};

const unsigned short music_limit_notes[]
=
{ 10, 20, 40, 65 };

const unsigned short music_start_notes[]
=
{ 0, 10, 20, 40 };


int main()
{
    short mode = 8;
    short s_mode = -1;

	// Turn on Supervisor mode
	// This is much faster than the default
	// Thanks SMFX for the advice
	asm ("trap    #0");
	asm ("or.w    #$700,sr");

    mt_dmode(&mode, &s_mode);
    SET_LOW_RESOLUTION();
	
	switch_gamemode(0);
	
	// This will be expanded later for systems with more memory
	// For now assume 128K system
	//frames_offset[3] = TEMP_RAM_AREA_ALT_128K_CONFIG + 0;
	//frames_offset[4] = TEMP_RAM_AREA_ALT_128K_CONFIG + 5876;

	while(1)
	{
		keys = mt_ipcom ((char *)IPCcmd);  // returns the same as S*BASIC KEYROW, keyboard reading
		
		if (music_toplay != 99)
		{
			music_tick++;
			if (note_index > music_limit_notes[music_toplay])
			{
				music_tick = 0;
				note_index = music_start_notes[music_toplay];
			}
			if (music_tick > note_delay)
			{
				music_tick = 0;
				note_index++;
				do_sound(MUSIC_NOTES[note_index][0], MUSIC_NOTES[note_index][1], MUSIC_NOTES[note_index][2], 0, 0, 0, 
				MUSIC_NOTES[note_index][3], MUSIC_NOTES[note_index][4]);
			}
		}
		
		switch(game_mode)
		{
			case 0:
				wait++;
				if (keys == Q_KEY_SPACE && wait > 10)
				{
					do_sound(1280, 020, 0x10, 0, 0, 0, 0x90, 0xCC);
					do_sound(3200, 0x80, 0x99, 0, 0, 0, 0x90, 0xCC);
					switch_gamemode(3);
				}
			break;
			case 1:
				wait++;
				time_game+=1;

				if (FRAME_CURRENT > 0 && status_level1[status] != END_GAME_STATUS)
				{
					if (status_level1[status] == STOP_GAME_STATUS)
					{
						if (global_score > 1)
						{
							global_score--;
							Print_score();
						}
						
					}
					else
					{
						global_score++;
						Print_score();
					}
					
					FRAME_CURRENT++;
					if (FRAME_CURRENT > 4) FRAME_CURRENT = 0;
					lz4w_unpack(( void*)frames_offset[FRAME_CURRENT], (void*)SCR8_START_ADDRESS);
				}
				
				if (FRAME_CURRENT == 0)
				{
					if (keys == Q_KEY_SPACE && wait > 10)
					{
						wait = 0;
						FRAME_CURRENT = 1;
						
						do_sound(320*6, 0x00, 0xCD, 0, 0, 0, 0xAA, 0x99);
						do_sound(160*6, 0xA0, 0x10, 0, 0, 0, 0x70, 0xCC);
						do_sound(190*6, 0xA5, 0xAA, 0, 0, 0, 0xDD, 0xFF);
						
						lz4w_unpack(( void*)frames_offset[FRAME_CURRENT], (void*)SCR8_START_ADDRESS);
					}
				}
				

				if (time_game > 123)
				{
					if (status_level1[status] == END_GAME_STATUS)
					{
						switch_gamemode(2);
					}
					else
					{
						time_game = 0;
						status++;
						Print_text(ingame_quote[status_level1[status]], 11);
					}
				}
			
			break;
			case 2:
				wait++;
				if (keys == Q_KEY_SPACE && wait > 10)
				{
					do_sound(1280, 020, 0x10, 0, 0, 0, 0x90, 0xCC);
					do_sound(3200, 0x80, 0x99, 0, 0, 0, 0x90, 0xCC);
					switch_gamemode(0);
				}
			break;
			case 3:
			wait++;
			if (keys == Q_KEY_SPACE)
			{
				if (wait > 10)
				{
					text_progress++;
					do_sound(1280, 020, 0x10, 0, 0, 0, 0x90, 0xCC);
					do_sound(3200, 0x80, 0x99, 0, 0, 0, 0x90, 0xCC);
					if (text_progress > 4)
					{
						switch_gamemode(4);
					}
					else
					{
						switch(text_progress)
						{
							case 1:
								Print_text("\n\n\n\n\nYour willingness to play along will make you the perfect partnerin my devilishly delightful games.", 103);
							break;
							case 2:
								Print_text("\n\n\n\n\nI am Rikuto, and I challenge youto prove yourself and earn your place among the cunning and powerful.", 106);
							break;
							case 3:
								Print_text("\n\n\n\n\nGive in to your mischievous sideand enjoy the thrill of the unknown!", 73);
							break;
							case 4:
								Print_text("\n\n\n\n\nYour task is to mete out justiceand teach them a valuable lessonthey won't soon forget.", 92);
							break;
						}
					}	
					wait = 0;
				}
			}
			break;
			case 4:
				wait++;
				if (keys == Q_KEY_SPACE && wait > 10)
				{
					do_sound(1280, 020, 0x10, 0, 0, 0, 0x90, 0xCC);
					do_sound(3200, 0x80, 0x99, 0, 0, 0, 0x90, 0xCC);
					switch_gamemode(1);
				}
			break;
		}

		WAITVBLANK();

	}
	
	return 0;
}

// Hardcoded MESS !

static void PLAY_JINGLE_FAILURE()
{
	do_sound(
	2500, 0xFF, 0xC0, 
	0, 0, 0, 
	0xCC, 0xCC);	
	for(i=0;i<60;i++) { WAITVBLANK(); };
}

static inline void PLAY_JINGLE_GOOD()
{
	// SUCESS
	do_sound(
	2500, 0xA0, 0xA0, 
	0, 0, 0, 
	0xCC, 0xCC);	
	for(i=0;i<60;i++) { WAITVBLANK(); };
	do_sound(
	2500, 0x80, 0xA0, 
	0, 0, 0, 
	0xCC, 0xCC);	
	for(i=0;i<60;i++) { WAITVBLANK(); };
	do_sound(
	4000, 0x50, 0xA0, 
	0, 0, 0, 
	0xCC, 0xCC);	
	for(i=0;i<60;i++) { WAITVBLANK(); };
}


void switch_gamemode(unsigned char mode) 
{
	char txt[32];
	unsigned char first_byte;
	
	game_mode = mode;
	text_progress = 0;
	status = 0;
	time_game = 0;
	i = 0;
	wait = 0;
	FRAME_CURRENT = 0;
	music_tick = 0;
	note_index = 0;
	music_toplay = 99;
	
	Disable_text();
	
	switch(mode)
	{
		case 0:
			memset( (void*)SCR8_START_ADDRESS, 0, 32768);
			open_image_to_buffer("titlelz4w", (void*)TEMP_RAM_AREA, 6774);
			lz4w_unpack((void*)TEMP_RAM_AREA, (void*)SCR8_START_ADDRESS);
			music_toplay = 0;
		break;
		case 1:
			music_toplay = 3;
			memset( (void*)SCR8_START_ADDRESS, 0, 32768);
			global_score = 0;
			
			open_image_to_buffer("frame1", ( void*)frames_offset[0], 5610);
			open_image_to_buffer("frame2", ( void*)frames_offset[1], 5854);
			open_image_to_buffer("frame3", ( void*)frames_offset[2], 5980);
			open_image_to_buffer("frame4", ( void*)frames_offset[3], 5854);
			open_image_to_buffer("frame5", ( void*)frames_offset[4], 5980);
			
			lz4w_unpack(( void*)frames_offset[0], (void*)SCR8_START_ADDRESS);
			
			Init_Game_text();
			Init_Game_text_Score();
			
			Print_text(ingame_quote[status_level1[status]], 11);
			Print_score();
		break;
		case 2:
			open_image_to_buffer("baklz4w", (void*)TEMP_RAM_AREA, 15000);
			lz4w_unpack((void*)TEMP_RAM_AREA, (void*)SCR8_START_ADDRESS);
			
			Init_text();

			if (global_score < 100)
			{
				PLAY_JINGLE_FAILURE();
				Print_text(bad_end1, sizeof(bad_end1)-1);
			}
			else if (global_score < 200)
			{
				PLAY_JINGLE_FAILURE();
				Print_text(bad_end2, sizeof(bad_end2)-1);
			}
			else
			{
				PLAY_JINGLE_GOOD();
				Print_text(good_end, sizeof(good_end)-1);
			}
			music_toplay = 1;
		break;
		case 3:
			music_toplay = 1;
			open_image_to_buffer("baklz4w", (void*)TEMP_RAM_AREA, 15000);
			lz4w_unpack((void*)TEMP_RAM_AREA, (void*)SCR8_START_ADDRESS);
			Init_text();
			Print_text("Oh my, i see a pure and untainted soul. Allow me to show you thepleasures of the darker side.", 93);
		break;
		case 4:
			music_toplay = 2;
			memset( (void*)SCR8_START_ADDRESS, 0, 32768);
			Initfull_text();
			Print_text(text_minigame_1, sizeof(text_minigame_1)-1);
		break;
	}
	
	// So we only have to do it once
	note_index = music_start_notes[music_toplay];
}

