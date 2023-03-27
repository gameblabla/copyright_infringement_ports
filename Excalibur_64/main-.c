#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <compress/zx0.h>
#include <compress/zx1.h>
#include <graphics.h>
#include <conio.h>
#include <sys/ioctl.h>

// zcc +cpm -subtype=excali64 main.c
// 0x2000, 0x2fff


int file_handle;
unsigned short i;

extern unsigned char buffer_tocopy[];
extern void Copy_Buffer();
unsigned char tmp[10001];
unsigned char FRAME_CURRENT;
const unsigned char* address_frame[8] =
{
	tmp,
	tmp + 2000,
	tmp + 2000*2,
	tmp + 2000*3,
	tmp + 2000*4,
	tmp + 2000*3,
	tmp + 2000*2,
	tmp + 2000,
};
unsigned short delay;
        
int main() {
	FILE* fp;
	unsigned char i, a, c;
	unsigned short size;
	unsigned char mode;
	/*textbackground(RED);
	textcolor(RED);
	printk("TEST\n");
	*/
	/*
	#asm
	set	0,a
	out	($70),a
	#endasm
	bzero_();
	*/
        /*#if defined(SCREEN_MODE)
                int mode;
                mode = SCREEN_MODE;
                console_ioctl(IOCTL_GENCON_SET_MODE, &mode);        
        #endif*/

        // Clear the screen
        //printf("%c",12);
       // printf("\x0c");  // the "\x0c" (0x0c character) resets the cursor position to the top left corner

	
	/*textbackground(BLUE);
	textcolor(WHITE);*/
	
	bdos(0x0E, 1);
	
	clg();
	
	/*file_handle = open("BAKURA.BIN", O_RDONLY, _IOREAD | _IOUSE);
	size = read(file_handle, tmp, 1442);
	close(file_handle);
	putsprite(0, 0, 0, tmp);*/
	
	file_handle = open("FRAMES.BIN", O_RDONLY, _IOREAD | _IOUSE);
	size = read(file_handle, tmp, 10000);
	close(file_handle);

	gotoxy(0, 23);
	puts("SCORE");
	
	/*textbackground(0);
	clg();
	textbackground(2);
	
	putsprite(0, 0, 0, temp_space);
	gotoxy(0, 20);
	puts("You will be the perfect victim for my games !");
	fgetc_cons();
	
	textbackground(0);
	clg();
	
	gotoxy(0, 0);
	puts("LOADING");
	
	file_handle = open("F1.BIN", O_RDONLY, _IOREAD | _IOUSE);
	size = read(file_handle, temp_space, 2000);
	close(file_handle);
	
	clg();
	gotoxy(0, 0);
	for(i=0;i<80*5;i++)
	{
		textbackground(temp_space[i]);
		textcolor(temp_space[i]);
		fputc_cons(' ');
	}*/
	delay = 0;
	while(1)
	{
		delay++;
		if (delay > 5000)
		{
			FRAME_CURRENT++;
			if (FRAME_CURRENT > 7) FRAME_CURRENT = 0;
			
			memcpy(buffer_tocopy, address_frame[FRAME_CURRENT], 1920);
			Copy_Buffer();
			delay = 0;
		}
	}



}
