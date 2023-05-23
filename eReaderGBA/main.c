#include <stdint.h>
#include <stddef.h>

#include "gbalib.h"
#include "frames16.h"

#ifdef EREADER
#include "def.h"
#include "erapi.h"
#endif

uint8_t quit;
uint32_t key;


#ifdef EREADER
#define SRAM_OFFSET 0x10000
#else
#define SRAM_OFFSET 0
#endif

unsigned char k, game_mode, text_progress;
#define FRAME_CURRENT text_progress
signed char diff;

void switch_gamemode(unsigned char mode) ;

#define KEEPITUP_GAME_STATUS 0
#define STOP_GAME_STATUS 1
#define FUCK_GAME_STATUS 2
#define END_GAME_STATUS 3

const unsigned char status_level1[] =
{ 
	FUCK_GAME_STATUS, KEEPITUP_GAME_STATUS, 
	STOP_GAME_STATUS, FUCK_GAME_STATUS, KEEPITUP_GAME_STATUS, 
	STOP_GAME_STATUS, FUCK_GAME_STATUS, KEEPITUP_GAME_STATUS, 
	STOP_GAME_STATUS, FUCK_GAME_STATUS,
	END_GAME_STATUS, END_GAME_STATUS 
};

unsigned char status;
unsigned char i;
unsigned short time_game;

const char ingame_quote[5][5] =
{
	"GO!!",
	"STOP",
	"FUCK",
	"END!",
};

const unsigned char jump[5] = 
{ 3, 0xFF, 0, 0xFF, 1 };

#define add_score(add) \
	bcd[2] += add; \
	if (bcd[2] > 57) { \
		bcd[1]+= 1; \
		bcd[2] = 48; } \
	if (bcd[1] > 57) { \
		bcd[0]+= 1; \
		bcd[1] = 48; } \

#define decrease_score(dec) \
	bcd[2] -= dec; \
	if (bcd[2] < 48) \
	{ \
		bcd[1]-= 1; \
		bcd[2] = 57; \
	} \
	if (bcd[1] < 48) \
	{ \
		bcd[0]-= 1; \
		bcd[1] = 57; \
	} \

static unsigned char bcd[4];
static unsigned char hibcd[4];

static const unsigned char* const frame_porn_pattern[]=
{
	f1Bitmap, f2Bitmap, f3Bitmap, f1Bitmap,
};

void Draw_T()
{
	Draw_Text_Center("SCORE", 114, 15, 0);
	Draw_Text_Center(bcd, 128, 15, 0);	
}

void switch_gamemode(unsigned char mode);

int main(void) {

	Init_GBA(DCNT_MODE4);
	quit = 0;
	
	hibcd[0] = SRAM_LOC[0  + SRAM_OFFSET];
	hibcd[1] = SRAM_LOC[1  + SRAM_OFFSET];
	hibcd[2] = SRAM_LOC[2  + SRAM_OFFSET];
	
	if (hibcd[0] < 48 || hibcd[0] > 57)
	{
		hibcd[0] = 48;
		hibcd[1] = 48;
		hibcd[2] = 48;
		hibcd[3] = '\0';
	}
	
	switch_gamemode(0);

#ifdef EREADER
	while (quit == 0)
#else
	for(;;)
#endif
	{
		updateButtons();
		switch(game_mode)
		{
			default:
				if ( wasButtonAPressed())
				{
					switch_gamemode(jump[game_mode]);
				}
#ifdef EREADER
			key = ERAPI_GetKeyStateRaw();
			if (key & ERAPI_KEY_B) quit = 1;
#endif
			break;
			case 3:
				if (wasButtonAPressed())
				{
					#ifdef EREADER
					ERAPI_PlaySoundSystem(5);
					#endif
					text_progress++;
					if (text_progress > 4)
					{
						switch_gamemode(4);
					}
					else
					{
						DMAFastFill(0x00000000, fb, (240*160)/4, DMA_32NOW);
						switch(text_progress)
						{
							case 1:
								Draw_Text("YOU'RE GOING TO BE THE", 0, 144, 15, 0);
								Draw_Text("PERFECT VICTIM FOR MY GAMES", 0, 152, 15, 0);
							break;
							case 2:
								Draw_Text("I'M RIKUTO,I CHALLENGE", 0, 144, 15, 0);
								Draw_Text("YOU TO A TRIAL", 0, 152, 15, 0);
							break;
							case 3:
								Draw_Text("DONT BE SCARED,IT WILL BE FUN", 0, 144, 15, 0);
							break;
							case 4:
								Draw_Text("FUCK HIM REAL HARD", 0, 144, 15, 0);
							break;
						}
					}	
				}
			break;
			case 1:
				time_game+=1;

				if (FRAME_CURRENT > 0 && status_level1[status] != END_GAME_STATUS)
				{
					if (FRAME_CURRENT > 3)
					{
						FRAME_CURRENT = 0;
						DMAFastCopy((void*)frame_porn_pattern[FRAME_CURRENT], (void*)fb, sizeof(f1Bitmap)/4, DMA_32NOW);
					}
					else
					{
					
						if (status_level1[status] == STOP_GAME_STATUS)
						{
							if (!(bcd[2] == 48 && bcd[0] == 48 && bcd[1] == 48)) 
							{
								decrease_score(1);
							}
						}
						else
						{
							add_score(1);
						}
						
						DMAFastCopy((void*)frame_porn_pattern[FRAME_CURRENT], (void*)fb, sizeof(f1Bitmap)/4, DMA_32NOW);
						
						FRAME_CURRENT++;

					}
					Draw_T();
					
					vid_vsync();
					vid_vsync();
					vid_vsync();
					vid_vsync();
					vid_vsync();
				}
				
				if (FRAME_CURRENT == 0)
				{
					if (wasButtonAPressed())
					{
						#ifdef EREADER
						ERAPI_PlaySoundSystem(1);
						#endif
						FRAME_CURRENT = 1;
					}
				}
				
				
				if (time_game > 70)
				{
					time_game = 0;
					if (status_level1[status] == END_GAME_STATUS)
					{
						switch_gamemode(2);
					}
					else
					{
						#ifdef EREADER
						ERAPI_PlaySoundSystem(0);
						#endif
						status++;
						Draw_Text_Center(ingame_quote[status_level1[status]], 144, 15, 0);
					}
				}
				

			break;
		}

		
		
		vid_vsync();

	}
	
	return 2; // Exit to menu
}

void switch_gamemode(unsigned char mode)
{
	game_mode = mode;
	text_progress = 0;
	status = 0;
	time_game = 0;
	i = 0;
	
	DMAFastCopy((void*)f1Pal, (void*)MEM_BG_PALETTE, sizeof(f1Pal)/4, DMA_32NOW);
	DMAFastFill(0x00000000, fb, (240*160)/4, DMA_32NOW);

	switch(mode)
	{
		case 0:
			Draw_Text_Center("RIK'S REVENGE", 16, 15, 0);
			Draw_Text(hibcd, 0, 152, 15, 0);
			#ifdef EREADER
			ERAPI_PlaySoundSystem(2);
			#endif
		break;
		case 1:
			bcd[0] = 48;
			bcd[1] = 48;
			bcd[2] = 48;
			bcd[3] = '\0';
			DMAFastCopy((void*)frame_porn_pattern[FRAME_CURRENT], (void*)fb, sizeof(f1Bitmap)/4, DMA_32NOW);
			Draw_T();
			Draw_Text_Center(ingame_quote[status_level1[status]], 144, 15, 0);
		break;
		case 2:

			//Canonical approach, thanks calcmaniac
			diff = bcd[0] - hibcd[0];
			if (diff == 0)
				diff = bcd[1] - hibcd[1];
			if (diff == 0)
				diff = bcd[2] - hibcd[2];
				
			if (diff > 0)
			{
				hibcd[0] = bcd[0];
				hibcd[1] = bcd[1];
				hibcd[2] = bcd[2];
				
				SRAM_LOC[0 + SRAM_OFFSET] = hibcd[0];
				SRAM_LOC[1 + SRAM_OFFSET] = hibcd[1];
				SRAM_LOC[2 + SRAM_OFFSET] = hibcd[2];
			}
			
			hibcd[3] = '\0';

			if (bcd[0] < 48+1)
			{
				#ifdef EREADER
				ERAPI_PlaySoundSystem(4); // Error Message
				#endif
				Draw_Text("BETTER LUCK NEXT TIME", 0, 144, 15, 0);
			}
			else
			{
				#ifdef EREADER
				ERAPI_PlaySoundSystem(3); // Confirm message
				#endif
				Draw_Text("CONGRATS!", 0, 144, 15, 0);
			}
		break;
		case 3:
			Draw_Text("WHAT DO WE HAVE HERE?", 0, 144, 15, 0);
			Draw_Text("YOU LOOK SO INNOCENT", 0, 152, 15, 0);
		break;
		case 4:
			Draw_Text("FUCK HIM WITH A", 0, 8, 15, 0);
		break;
	}
	
}
