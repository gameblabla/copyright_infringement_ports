#include <stdio.h>
#include <stdlib.h>
#include "gfx.h"
void print_menu()
{
	CLEAR_TEXT_VRAM();
	Cursor_SetXY(0, 0);
    puts("================================================================================");
    puts("                             SELECT A SOUNDBOARD                                ");
    puts("================================================================================");
    puts("");
}
int main()
{
    int choice = -1;
    
	CLEAR_TEXT_VRAM();
	
	// Hides text cursor
	__asm
	{
		mov	DX,'5h'
		
		mov	AL,27	; ESC
		int	29h
		mov	AL,'['	; '['
		int	29h
		mov	AL,'>'	; '>'
		int	29h
		mov	AL,DH
		int	29h
		mov	AL,DL
		
		int	29h
	}
	// Hide system line
	__asm
	{
		mov	DX,'1l'
		
		mov	AL,27	; ESC
		int	29h
		mov	AL,'['	; '['
		int	29h
		mov	AL,'>'	; '>'
		int	29h
		mov	AL,DH
		int	29h
		mov	AL,DL
		int	29h
	}

    while (choice < 1 || choice > 4)
    {
        print_menu();
        puts("1: PC-9801-86 without ADPCM memory");
        puts("2: Mate-X/Chibi-Oto/SpeakerBoard");
        puts("3: No sound");
        puts("4: Exit");
        puts("Enter your choice (0, 1, 2, 3): ");
        scanf("%d", &choice);

        if (choice < 1 || choice > 4)
        {
            puts("Invalid choice. Please select a valid option.");
        }
    }
    
	
	// Show cursor
	__asm
	{
		mov	DX,'5l'
		
		mov	AL,27	; ESC
		int	29h
		mov	AL,'['	; '['
		int	29h
		mov	AL,'>'	; '>'
		int	29h
		mov	AL,DH
		int	29h
		mov	AL,DL
		int	29h
	}
	
	// Show system line
	__asm
	{
		mov	DX,'1h'
		
		mov	AL,27	; ESC
		int	29h
		mov	AL,'['	; '['
		int	29h
		mov	AL,'>'	; '>'
		int	29h
		mov	AL,DH
		int	29h
		mov	AL,DL
		int	29h
	}
	
	// KEY BEEP
	__asm
	{
		xor	AX,AX
		mov	ES,AX
		and	byte ptr ES:[500H],NOT 20h	
	}
    
    return choice;
}
