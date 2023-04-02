/*	example code for llvm-mos, for NES
 *  writing a full screen from RLE compressed .h file
 *  -can only be done with rendering off
 *	using neslib
 *	Doug Fraker 2018
 */
#include <string.h>
#include <nesdoug.h>
#include <neslib.h>
#include <bank.h>
#include <famitone2.h>


#include "bakura.h"
#include "porn1_nametable.h"
#include "empty.h"

//#define MMC1 1

//asm (".globl __prg_ram_size\n__prg_ram_size=8\n");

const unsigned char bakura_palette[] = { 0x0f, 0x16, 0x24, 0x30, 0x0f, 0x16, 0x09, 0x30, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f };
const unsigned char porn_pal[16] = { 0x0f, 0x16, 0x09, 0x30, 0x0f, 0x30, 0x0f, 0x30, 0x0f, 0x30, 0x0f, 0x30, 0x0f, 0x30, 0x0f, 0x30 };
const unsigned char palette_sp[16] = {
	0x0f, 0x30, 0x0f, 0x2f // black, black, yellow
};


static unsigned char FRAME_CURRENT;
static unsigned char delayf;
static unsigned char game_mode;
static unsigned char text_progress;

static char pad1;
static char pad1_new;
static unsigned char status;
static unsigned short time_game;


void switch_gamemode(unsigned char mode);


static unsigned char bcd[3];
//static unsigned char hibcd[3];

#define add_score(add) \
	bcd[2] += add; \
	if (bcd[2] > 9) { \
		bcd[1]+= 1; \
		bcd[2] = 0; } \
	if (bcd[1] > 9) { \
		bcd[0]+= 1; \
		bcd[1] = 0; } \

#define decrease_score(dec) \
	bcd[2] -= dec; \
	if (bcd[2] > 250) \
	{ \
		bcd[1]-= 1; \
		bcd[2] = 9; \
	} \
	if (bcd[1] > 250) \
	{ \
		bcd[0]-= 1; \
		bcd[1] = 9; \
	} \



const unsigned char ingame_quote_offset[4] =
{
	0, 4, 8, 12
};

/*
	* \x24 : !
	* \x25 : '
	* \x26 : :
	* \x27 : ?
	* \x28 : .
*/
static unsigned char text_buffer[82];
void Print_text(char* text, unsigned char x, unsigned char y)
{
	unsigned char t, size_txt_buffer;
	//vram_adr(NTADR_A(x, y));
	size_txt_buffer = 0;
	for (unsigned char i = 0; text[i]; ++i)
	{
		// This is for escape codes
		t = text[i];
		
		// Space
		if (text[i] == ' ')
		{
			t = 254;
		}
		// To print text
		else if (text[i] >= 'A' && text[i] <= 'Z')
		{
			t -= 55;
		}
		// To print numbers
		else if (text[i] <= '9' && text[i] >= '0')
		{
			t -= 48;
		}
		 
		text_buffer[i] = t;
		size_txt_buffer++;
		//
		//vram_put(t); // this pushes 1 char to the screen	
	}
	
	multi_vram_buffer_horz(text_buffer, size_txt_buffer, NTADR_A(x,y));
	//vram_adr(NTADR_A(x, y));
	//one_vram_buffer(tex[0], get_ppu_addr(0,x,y));
}

void Clear_Text()
{
	memset(text_buffer, 254, sizeof(text_buffer));
	multi_vram_buffer_horz(text_buffer, sizeof(text_buffer), NTADR_A(2,24));	
}

/*
static inline void Play_Voice(unsigned char v)
{
	//set_prg_8000(14);
	sample_play(v);
	//set_prg_8000(0);
}*/




#define KEEPITUP_GAME_STATUS 0
#define STOP_GAME_STATUS 1
#define FUCK_GAME_STATUS 2
#define END_GAME_STATUS 3

unsigned char toggle_sound_status;

const unsigned char status_level1[12] =
{ 
	FUCK_GAME_STATUS, KEEPITUP_GAME_STATUS, 
	STOP_GAME_STATUS, FUCK_GAME_STATUS, KEEPITUP_GAME_STATUS, 
	STOP_GAME_STATUS, FUCK_GAME_STATUS, KEEPITUP_GAME_STATUS, 
	STOP_GAME_STATUS, FUCK_GAME_STATUS,
	END_GAME_STATUS, END_GAME_STATUS 
};


const unsigned char ingame_quote[5*4] =
{
	'Y'-55, 'E'-55, 'S'-55, 254,
	'S'-55, 'T'-55, 'O'-55, 'P'-55,
	'F'-55, 'U'-55, 'C'-55, 'K'-55,
	'E'-55, 'N'-55, 'D'-55, 254,
};

static void Sprite_Update_Game()
{
	oam_clear();
	oam_spr(104,184,bcd[0],0);
	oam_spr(112,184,bcd[1],0);
	oam_spr(120,184,bcd[2],0);
	oam_spr(112,56,ingame_quote[ingame_quote_offset[status_level1[status]]+0],0);
	oam_spr(112+8,56,ingame_quote[ingame_quote_offset[status_level1[status]]+1],0);
	oam_spr(112+8+8,56,ingame_quote[ingame_quote_offset[status_level1[status]]+2],0);
	oam_spr(112+8+8+8,56,ingame_quote[ingame_quote_offset[status_level1[status]]+3],0);
	
	// Debugging
	//oam_spr(0,0,status,0);
}

const unsigned char jump[5] = 
{ 3, 0xFF, 0, 0xFF, 1 };



extern const char dialogue1_data[];
extern const char dialogue2_data[];
extern const char dialogue3_data[];
extern const char dialogue4_data[];
extern const char dialogue5_data[];
extern const char dialogue6_data[];
extern const char dialogue7_data[];
extern const char ingame_data[];
extern const char results_data[];

void Play_Voice_banked(unsigned char voice_number, unsigned char bank, unsigned char duration)
{
	set_prg_8000(bank);

	switch(bank)
	{
		case 14:
			music_init(dialogue1_data);
		break;
		case 16:
			music_init(dialogue2_data);
		break;
		case 18:
			music_init(dialogue3_data);
		break;
		case 20:
			music_init(dialogue4_data);
		break;
		case 22:
			music_init(dialogue5_data);
		break;
		case 24:
			music_init(dialogue6_data);
		break;
		case 26:
			music_init(dialogue7_data);
		break;
		case 30:
			music_init(results_data);
		break;
	}
	
	sample_play(voice_number);
	
	while(duration--)
	{
		ppu_wait_nmi();
	}
}

unsigned char delay_voice_sys[5];

int main(void) {
	// NTSC is default
	delay_voice_sys[0] = 60;
	delay_voice_sys[1] = 85;
	delay_voice_sys[2] = 80;
	delay_voice_sys[3] = 70;
	delay_voice_sys[4] = 100;

	// if PAL adjust it again
	if (ppu_system() == 0)
	{
		delay_voice_sys[0] = 50;
		delay_voice_sys[1] = 70;
		delay_voice_sys[2] = 66;
		delay_voice_sys[3] = 58;
		delay_voice_sys[4] = 83;
	}

	oam_size(0);
	bank_spr(1);
	
	oam_clear();
	switch_gamemode(0);

	while (1) 
	{
		ppu_wait_nmi();
		pad1 = pad_poll(0); // read the first controller
		pad1_new = get_pad_new(0);
		
		if (delayf < 200)
		{
			delayf++;
		}
		
		switch(game_mode)
		{
			default:
			if (pad1_new & PAD_A)
			{
				if (delayf > 20)
				{
					switch_gamemode(jump[game_mode]);
				}
			}
			break;
			case 1:
				time_game+=1;

				if (FRAME_CURRENT > 0 && status_level1[status] != END_GAME_STATUS)
				{
					if (delayf > 5)
					{
						if (status_level1[status] == STOP_GAME_STATUS)
						{
							if (!(bcd[2] == 0 && bcd[0] == 0 && bcd[1] == 0)) 
							{
								decrease_score(1);
							}
						}
						else
						{
							add_score(1);
						}
						
						set_chr_bank(0,4+(FRAME_CURRENT<<2));
						set_chr_bank(1,6+(FRAME_CURRENT<<2));

						FRAME_CURRENT++;
						if (FRAME_CURRENT > 4) FRAME_CURRENT = 0;
						delayf = 0;
						
						Sprite_Update_Game();
					}
				}
				
				if (FRAME_CURRENT == 0)
				{
					if (pad1_new & PAD_A)
					{
						sample_play(1);
						if (toggle_sound_status == 1)
						{
							if (status_level1[status] == KEEPITUP_GAME_STATUS)
							{
								sample_play(3);
								toggle_sound_status = 0;
							}
							if (status_level1[status] == FUCK_GAME_STATUS)
							{
								sample_play(2);
								toggle_sound_status = 0;
							}
						}

						FRAME_CURRENT = 1;
					}
				}
				
				
				if (time_game > delay_voice_sys[4]<<2)
				{
					if (status_level1[status] == END_GAME_STATUS)
					{
						switch_gamemode(2);
					}
					else
					{
						time_game = 0;
						status++;
						toggle_sound_status = 1;
						Sprite_Update_Game();
					}
				}

			break;
			case 3:
				if (delayf > 20)
				{
					if (pad1_new & PAD_START)
					{
						switch_gamemode(4);
					}
					
					if (pad1_new & PAD_A)
					{
						text_progress++;
						delayf = 0;

						if (text_progress > 5)
						{
							switch_gamemode(4);
						}
						else
						{
							set_vram_buffer();
							Clear_Text();
							ppu_wait_nmi();
							
							set_vram_buffer();
							switch(text_progress)
							{
								case 1:
									Print_text("YOU\x25RE GOING TO BE THE PERFECT", 1, 24);
									Print_text("VICTIM FOR MY GAMES\x24", 1, 25);
									ppu_wait_nmi();
									Play_Voice_banked(1, 18, delay_voice_sys[2]);
									Play_Voice_banked(2, 18, delay_voice_sys[2]);
								break;
								case 2:
									Print_text("I\x25M RIKUTO\x28 I OFFER YOU A", 1, 24);
									Print_text("CHANCE TO PROVE YOUR WORTH", 1, 25);
									Print_text("IN A TRIAL\x28", 1, 26);
									ppu_wait_nmi();
									Play_Voice_banked(2, 20, delay_voice_sys[2]);
									Play_Voice_banked(1, 20, delay_voice_sys[3]);
									Play_Voice_banked(2, 22, delay_voice_sys[2]);
								break;
								case 3:
									Print_text("DON\x25T BE SCARED", 1, 24);
									Print_text("IT WILL BE FUN \x24", 1, 25);
									ppu_wait_nmi();
									Play_Voice_banked(1, 22, delay_voice_sys[2]);
								break;
								case 4:
									Print_text("YOUR CHALLENGE IS GOING BE", 1, 24);
									Print_text("THIS\x26YOU\x25LL ON THE OTHER END", 1, 25);
									Print_text("AND PUNISH THEM\x24", 1, 26);
									ppu_wait_nmi();
									Play_Voice_banked(1, 24, delay_voice_sys[4]);
									Play_Voice_banked(2, 24, delay_voice_sys[4]);
								break;
								case 5:
									Print_text("DON\x25T LIKE IT\x27 YOUR LOSS\x24", 1, 24);
									ppu_wait_nmi();
									Play_Voice_banked(1, 26, delay_voice_sys[4]);
								break;
							}
							
						}
					}
				}
			break;

		}

		

	}
}

void Display_Bakura(unsigned char e)
{
	ppu_off(); // screen off
	pal_bg(bakura_palette); //	load the palette
	pal_spr(palette_sp);

	// Bakura
	set_chr_bank(0,28);
	set_chr_bank(1,30);
			
	vram_adr(NAMETABLE_A);
	if (e == 0)
	{
		vram_unrle(empty);
	}
	if (e == 1)
	{
		vram_unrle(bakura);
	}
	
	ppu_on_all();	
}

void switch_gamemode(unsigned char mode)
{
	oam_clear();
	game_mode = mode;
	delayf = 0;
	time_game = 0;
	status = 0;
	text_progress = 0;
	switch(mode)
	{
		case 0:
			Display_Bakura(0);
			
			set_vram_buffer();
			Print_text("COPYRIGHT INFRINGEMENT", 6, 2);
			Print_text("THE REGRESSIVE RIGHT", 7, 3);
			Print_text("BY GAMEBLABLA", 10, 5);
			Print_text("APRIL 2023  BUILD", 8, 26);
			
			Print_text("PUSH A TO START", 9, 23);
			ppu_wait_nmi();
		break;
		case 1:
			bcd[0] = 0;
			bcd[1] = 0;
			bcd[2] = 0;
		
			ppu_off(); // screen off
			pal_bg(porn_pal); //	load the palette
			pal_spr(palette_sp);

			// First frame
			set_chr_bank(0,4);
			set_chr_bank(1,6);

			// Font
			set_chr_bank(2,0);
			set_chr_bank(3,2);

			bank_spr(1);
			vram_adr(NAMETABLE_A);
			vram_unrle(porn1_nametable);
			
			Sprite_Update_Game();
			
			ppu_on_all();
			
			// Switch to bank for ingame voices
			set_prg_8000(28);
			music_init(ingame_data);
			toggle_sound_status = 0;
		break;
		case 2:
			Display_Bakura(1);
			
			set_vram_buffer();
			if (bcd[0] < 2)
			{
				Print_text("GUESS YOU WILL STAY HERE\x24", 1, 24);
				Play_Voice_banked(1, 30, delay_voice_sys[4]);
			}
			else if (bcd[0] < 3)
			{
				Print_text("NOT BAD BUT YOU CAN DO BETTER\x24", 1, 24);
				Play_Voice_banked(1, 30, delay_voice_sys[4]);
			}
			else
			{
				Print_text("CONGRATS\x24 YOU WIN THIS TIME\x24", 1, 24);
				Print_text("I WILL SEE YOU LATER\x28\x28\x28", 1, 25);
				Play_Voice_banked(2, 30, delay_voice_sys[4]);
			}
			
			ppu_wait_nmi();
		break;
		case 3:
			Display_Bakura(1);
			
			set_vram_buffer();
			Print_text("OH MY\x24 WHAT DO WE HAVE HERE\x27", 1, 24);
			Print_text("YOU LOOK SO INNOCENT", 1, 25);
			ppu_wait_nmi();

			Play_Voice_banked(1, 14, delay_voice_sys[0]); //Oh my !
			Play_Voice_banked(2, 16, delay_voice_sys[1]); //What do we have here
			Play_Voice_banked(1, 16, delay_voice_sys[2]); //You look so innocent
		break;
		case 4:
			Display_Bakura(0);
			
			set_vram_buffer();
			Print_text("YOU MUST FUCK ZHONGLI\x24", 1, 1);
			Print_text("PRESS A TO FUCK", 1, 3);
			Print_text("FOLLOW ONSCREEN INSTRUCTIONS", 1, 4);
			Print_text("GOOD LUCK", 1, 7);
			ppu_wait_nmi();
		break;
	}
}
