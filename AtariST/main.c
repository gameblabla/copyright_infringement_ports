#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <osbind.h>
#include <mint/cookie.h>
#include "ikbd.h"
#include "sound.h"
#include "atarist.h"

unsigned char* bmp[2];
SoundDevice mainsound;

#define DATA_FOLDER "DATA\\"

void * physbase;
void *logbase;

const char soundfiles[2][7][18] = 
{
	{
		DATA_FOLDER"SP1.TDY",
		DATA_FOLDER"SP2.TDY",
		DATA_FOLDER"SP3.TDY",
		DATA_FOLDER"SP4.TDY",
		DATA_FOLDER"SP5.TDY",
		DATA_FOLDER"FUCK.TDY",
		DATA_FOLDER"FUCK2.TDY",
	},
	{
		DATA_FOLDER"SP1.RAW",
		DATA_FOLDER"SP2.RAW",
		DATA_FOLDER"SP3.RAW",
		DATA_FOLDER"SP4.RAW",
		DATA_FOLDER"SP5.RAW",
		DATA_FOLDER"FUCK.RAW",
		DATA_FOLDER"FUCK2.RAW",
	},
};

unsigned char k, game_mode, text_progress;
#define FRAME_CURRENT text_progress
signed char diff;
unsigned char status;
unsigned char i;
unsigned short time_game;


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

const char ingame_quote[5][5] =
{
	"GO!!\0",
	"STOP\0",
	"FUCK\0",
	"END \0",
};
const unsigned char jump[5] = 
{ 3, 0xFF, 0, 0xFF, 1 };

static char bcd[4];
static char hibcd[4];

void switch_gamemode(unsigned char mode) ;


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


uint8_t special_sound;


void Clear_Text()
{
	memset(physbase + 28000, 0, 32000-28000); 
}

void DrawHUD()
{
	Draw_Text("SCORE", 0, 180, 15);
	Draw_Text(bcd, 0, 188, 15);
	Draw_Text_Center(ingame_quote[status_level1[status]], 180, 15);	
}

/*
int main ( void )
{
	uint8_t exitflag = 0;
	logbase = Logbase();
	physbase = Physbase();

	bmp[0] = Load_Data("DATA\\FRAME.RAW", 28160);
	Load_Pal("DATA\\FRAME1.PAL");
	memcpy(logbase, bmp[0], 28160);
	//lz4w_unpack(bmp[0], physbase);
	
	while(1){}
	
	return 0;
}*/


int main ( void )
{
	uint8_t exitflag = 0;
	
	logbase = Logbase();
	physbase = Physbase();

	Setscreen(logbase, physbase, ST_LOW);
	
	Init_Machine();
	
	mainsound.Init_Sound();
	
	hibcd[0] = 48;
	hibcd[1] = 48;
	hibcd[2] = 48;
	hibcd[3] = '\0';
	
	switch_gamemode(0);

	while(exitflag == 0)
	{
		if(IKBD_IsKeyPressed(IKBD_KEY_ESC))
		{
			exitflag = 1;
		}
		
		switch(game_mode)
		{
			default:
				if (IKBD_IsKeyPressed(IKBD_KEY_SPACE))
				{
					switch_gamemode(jump[game_mode]);
				}
			break;
			case 1:
				time_game+=1;

				if (FRAME_CURRENT > 0 && status_level1[status] != END_GAME_STATUS)
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
					
					FRAME_CURRENT++;
					if (FRAME_CURRENT > 8) FRAME_CURRENT = 0;
					
					//lz4w_unpack(bmp[FRAME_CURRENT], physbase);
					memcpy(physbase, bmp[0] + (FRAME_CURRENT * 28160), 28160);
					Draw_Text(bcd, 0, 188, 15);
				}
				
				if (FRAME_CURRENT == 0)
				{
					if (IKBD_IsKeyPressed(IKBD_KEY_SPACE))
					{
						mainsound.Play_Sound(0, special_sound);
						if (special_sound == 1) 
						{
							special_sound = 0;
						}
						FRAME_CURRENT = 1;
					}
				}
				
				
				if (time_game > 128)
				{
					if (status_level1[status] == END_GAME_STATUS)
					{
						switch_gamemode(2);
					}
					else
					{
						time_game = 0;
						status++;
						if (status_level1[status] == KEEPITUP_GAME_STATUS)
						{
							special_sound = 1;
						}
						Draw_Text_Center(ingame_quote[status_level1[status]], 180, 15);	
					}
				}
			break;
			case 3:
				if (IKBD_IsKeyPressed(IKBD_KEY_SPACE))
				{
					text_progress++;

					if (text_progress > 4)
					{
						switch_gamemode(4);
					}
					else
					{
						Clear_Text();
						switch(text_progress)
						{
							case 1:
								lz4w_unpack(bmp[1], physbase);
								Draw_Text("You're going to be the perfect victim", 0, 180, 15);
								Draw_Text("for my games!", 0, 188, 15);
								mainsound.Load_Sound(soundfiles[isSoundDMA][1], 0);
							break;
							case 2:
								lz4w_unpack(bmp[0], physbase);
								Draw_Text("I'm Rikuto, i offer you the chance to", 0, 180, 15);
								Draw_Text("prove your worth in a trial", 0, 188, 15);
								mainsound.Load_Sound(soundfiles[isSoundDMA][2], 0);
							break;
							case 3:
								lz4w_unpack(bmp[1], physbase);
								Draw_Text("Don't be scared, it'll be fun!", 0, 180, 15);
								mainsound.Load_Sound(soundfiles[isSoundDMA][3], 0);
							break;
							case 4:
								lz4w_unpack(bmp[0], physbase);
								Draw_Text("Your challenge will be this: you'll be", 0, 180, 15);
								Draw_Text("on the other end and punish them", 0, 188, 15);
								mainsound.Load_Sound(soundfiles[isSoundDMA][4], 0);
							break;
						}
						mainsound.Play_Sound(0, 0);
					}
				}
			break;
		}
	
		Vsync();
	}
	
	for(i=0;i<12;i++)
	{
		Free_Data(bmp[i]);
	}
	Close_Machine();
}

void switch_gamemode(unsigned char mode)
{
	game_mode = mode;
	text_progress = 0;
	status = 0;
	time_game = 0;
	special_sound = 0;
	i = 0;
	
	for(i=0;i<2;i++)
	{
		Free_Data(bmp[i]);
	}
	
	memset(physbase, 0, 32000);
	
	switch(mode)
	{
		case 0:
			bmp[0] = Load_Data("DATA\\TITLE.LZ", 13262);
			Load_Pal("DATA\\TITLE.PAL");
			lz4w_unpack(bmp[0], physbase);
			//memcpy(physbase, bmp[0], 1);
		
			Draw_Text_Center("COPYRIGHT INFRINGEMENT", 16, 15);
			Draw_Text_Center("THE REGRESSIVE RIGHT", 16+8, 15);
			Draw_Text_Center("BY GAMEBLABLA 2023", 16+16, 15);
			
			Draw_Text_Center("PRESS SPACE TO PLAY", 160, 15);
			Draw_Text("HISCORE", 0, 175, 15);
			Draw_Text(hibcd, 0, 188, 15);
		break;
		case 1:
			bcd[0] = 48;
			bcd[1] = 48;
			bcd[2] = 48;
			bcd[3] = '\0';
			
			//bmp[0] = Load_Data("DATA\\FRAME1.LZ");
			//bmp[1] = Load_Data("DATA\\FRAME2.LZ");
			//bmp[2] = Load_Data("DATA\\FRAME3.LZ");
			//bmp[3] = Load_Data("DATA\\FRAME4.LZ");
			//bmp[4] = Load_Data("DATA\\FRAME5.LZ");
			//bmp[5] = Load_Data("DATA\\FRAME6.LZ");
			//bmp[6] = Load_Data("DATA\\FRAME7.LZ");
			//bmp[7] = Load_Data("DATA\\FRAME8.LZ");
			//bmp[8] = Load_Data("DATA\\FRAME9.LZ");
			
			bmp[0] = Load_Data("DATA\\FRAME.RAW", 253440);
			Load_Pal("DATA\\FRAME1.PAL");
			memcpy(physbase, bmp[0], 28160);
			
			mainsound.Load_Sound(soundfiles[isSoundDMA][5], 0);
			mainsound.Load_Sound(soundfiles[isSoundDMA][6], 1);
			
			DrawHUD();
		break;
		case 2:
			bmp[0] = Load_Data("DATA\\RIK.LZ", 18082);
			Load_Pal("DATA\\RIK.PAL");
			lz4w_unpack(bmp[0], physbase);
		
			Draw_Text("Your SCORE was ", 0, 0, 15);
			Draw_Text(bcd, 0, 16, 15);
			
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
			}
			
			Clear_Text();
			
			if (bcd[0] < 48+3)
			{
				Draw_Text("Guess you'll stay there for a while", 0, 180, 15);
			}
			else if (bcd[0] < 48+5)
			{
				Draw_Text("Not bad but you can do better!", 0, 180, 15);
				Draw_Text("Otherwise you can't leave this place!", 0, 188, 15);
			}
			else
			{
				Draw_Text("Congrats! I'll leave you alone.", 0, 180, 15);
				Draw_Text("This won't be the last of me!", 0, 188, 15);
			}
		break;
		case 3:
			bmp[0] = Load_Data("DATA\\RIK.LZ", 18082);
			bmp[1] = Load_Data("DATA\\RIKL.LZ", 18148 );
			Load_Pal("DATA\\RIK.PAL");
			lz4w_unpack(bmp[0], physbase);
		
			Draw_Text("Oh my, what do we have here?", 0, 180, 15);
			Draw_Text("You look so innocent", 0, 188, 15);
			
			mainsound.Load_Sound(soundfiles[isSoundDMA][0], 0);
			mainsound.Play_Sound(0, 0);
		break;
		case 4:
			Draw_Text("Your goal is to FUCK Zhongli!", 0, 8, 15);
			Draw_Text("Follow the instructions to get points", 0, 16, 15);
			Draw_Text("Penetrate him by pressing a key", 0, 24, 15);
			Draw_Text("Good luck !", 0, 64, 15);
		break;
	}
	
}
