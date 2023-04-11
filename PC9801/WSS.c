#include <stdio.h>
#include <dos.h>
#include <stdio.h>
#include <stdint.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>
#include <i86.h>
#include "PCM.H"

#define WSS_BASE_PORT 0xF44
#define WSS_DATA_PORT 0xF45
#define WSS_INT_PORT 0xF40

#define WSS_DMA_PORT 0xF4


#define WSS_REG_MODE 0xCC
#define WSS_REG_DAC 0x10
#define WSS_REG_VOLUME_LEFT 0x06
#define WSS_REG_VOLUME_RIGHT 0x07
#define WSS_REG_FORMAT_RATE 0x48

const unsigned short rate_table[] = {64000, 54840, 48000, 44100, 37800, 33075, 32000, 27420, 22050, 18900, 16000, 11025, 9600, 8000, 6620, 0};
//const unsigned char rate_table2[] = {0b1010, 0b1000, 0b1100, 0b1011, 0b1001, 0b1101, 0b0110, 0b0100, 0b0111, 0b0101, 0b0010, 0b0011, 0b1110, 0b0000, 0b1111, 0b0001};
const unsigned char rate_table2[] = {0x0A, 0x08, 0x0C, 0x0B, 0x09, 0x0D, 0x06, 0x04, 0x07, 0x05, 0x02, 0x03, 0x0E, 0x00, 0x0F, 0x01};


const char wav_ext[] = "WAV";
const unsigned short def_rate = 44100;
const unsigned char def_ch = 1;


// DMA and Interrupt tables
unsigned short dma_table[] = {
    0x00, 0x27, 0x01, 0x03, 0x01, 0x21, 0x05, 0x07, 0x03, 0x25, 0x0D, 0x0F
};

unsigned char int_table[] = {
    0x0B, 0x02, 0x08, 0x00, 0x0D, 0x02, 0x20, 0x00, 0x12, 0x0A, 0x04, 0x00, 0x14, 0x0A, 0x10, 0x00
};


void wss_set(unsigned short reg_value) {
    outp(WSS_BASE_PORT, reg_value >> 8);
    outp(WSS_DATA_PORT, reg_value & 0xFF);
}

void wss_wait() {
    unsigned int cx = 10000;
    while (cx--) {
        if ((inp(WSS_BASE_PORT) & 0x80) == 0)
            break;
    }
}

void wss_init() {
    wss_set(0xCC00 | 0x40); // Set mode to CS4231
    wss_set(0x1080);        // DAC on, Timer off, 0dB mode
    wss_set(0x1100);        // HPF off
}

void play_pcm() {
    unsigned int i;
    unsigned long sample;

    wss_init();

    // Configure WSS for 44100 Hz, 16-bit mono
    wss_set(0x08 << 8 | 0x2A); // 44100 Hz
    wss_set(0x28 << 8 | 0x81); // 16-bit mono
    wss_set(0x0A << 8 | 0x02); // Enable output

    // Play PCM data
    for (i = 0; i < pcm_data_length; i += 2) {
        // Read 16-bit sample from pcm_data
        sample = pcm_data[i] | (pcm_data[i + 1] << 8);

        // Wait for WSS to be ready
        wss_wait();

        // Write sample to WSS
        outpw(WSS_DATA_PORT, sample);
    }

    // Disable output and reset WSS
    wss_set(0x0A << 8 | 0x00);
    wss_set(0x28 << 8 | 0x00);
}

/*
int check_wss() {
    unsigned short sound_id = 0xA460;
    unsigned char al, ah;

    outp(WSS_BASE_PORT, 0xE1);
    ah = inp(WSS_BASE_PORT + 7);
    al = inp(WSS_BASE_PORT + 9);
    sound_id = (ah << 8) | al;

    al = inp(sound_id);
    ah = al & 0xF0;

    if (ah == 0x60 || ah == 0x70 || ah == 0x80) {
        return 1;
    } else {
        return 0;
    }
}*/

#define WSS_IRQ 7


int wss_check() {
    unsigned char al, ah;
    unsigned short sound_id;

    // Read the sound card ID
    outp(0xC24, 0xE1);
    ah = inp(0xC2B);
    al = inp(0xC2D);

    // Combine ah and al to get the sound_id
    sound_id = (ah << 8) | al;

    // Check the ID
    al = sound_id & 0xF0;
    if (al == 0x60) {
        // PC-9821Np case
        al = inp(0x881E) & 0x18;
        al += 0x10;
        ah = inp(WSS_INT_PORT) & 0xC7;
        al |= ah;
        outp(WSS_INT_PORT, al);
        printf("PC-9821Np edgecase\n");
        return 1; // WSS found
    } else if (al > 0x60 && al < 0xF0) {
        return 1; // WSS found
    } else if (al == 0xF0) {
		
        return 0; // No WSS
    }

    return 0; // No WSS
}


/*
void dma_init(int def_dma, int def_int) {
    unsigned short wss_int_port = WSS_INT_PORT;
    unsigned char al, ah;

    al = inp(wss_int_port);

    if (def_dma != -1) {
        ah = def_dma + 1;
        al = (al & 0xF8) | ah;
    }

    if (def_int != -1) {
        ah = def_int + 1;
        ah = ah << 3;
        al = (al & 0xC7) | ah;
    }

    outp(wss_int_port, al);

    // Initialize DMA and interrupt ports based on the provided assembly code.
    // ...
}*/

char def_dma = -1;
char def_int = -1;
unsigned char int_vec, int_port, int_mask;
unsigned char dmanum, dma_bank;
unsigned short dma_adr, dma_count;
unsigned char filenext_flag= 0;

void dma_int_init() {
    unsigned char temp;
	unsigned char dma_int;
    // Read the current value from port WSS_INT_PORT
    temp = inp(WSS_INT_PORT);

    if (def_dma != -1) {
        // Set DMA channel
        temp &= 0xF8;
        temp |= (def_dma + 1) & 0x07;
        printf("Temp dma 0x%x\n", temp);
    }

    if (def_int != -1) {
        // Set interrupt number
        temp &= 0x87;
        temp |= ((def_int - 1) << 3) & 0x78;
        printf("Temp int 0x%x\n", temp);
    }

    // Write the new value to port (0xF40) WSS_INT_PORT
    outp(WSS_INT_PORT, temp);

    if (def_dma != -1 && def_int != -1) {

        // Initialize DMA and interrupt
		dma_int = ((def_dma + 1) << 3) | (def_int - 1);
        
        printf("dma_int 0x%x\n", dma_int);

        // Set interrupt vector
        int_vec = int_table[dma_int * 4];
        int_port = int_table[dma_int * 4 + 1];
        int_mask = int_table[dma_int * 4 + 2];

        // Set DMA channel
        dmanum = dma_table[def_dma * 4];
        dma_bank = dma_table[def_dma * 4 + 1];

        // Set DMA address
        dma_adr = dma_table[def_dma * 4 + 2];
        dma_adr |= (dma_table[def_dma * 4 + 3] << 8);

        // Set DMA count
        dma_count = 0;
    }
}

void interrupt dmaint_handler() {

    // DMA check
    unsigned char dma_check;
	
    _asm {
        push ax
        push dx
        push ds
    }

    _asm {
        cld
        mov ax, cs
        mov ds, ax
    }

    _asm {
        mov dx, 0f46h
        in al, dx
        mov dma_check, al
    }

    if (dma_check & 0x01) {
        dma_program();
        filenext_flag = 1;
    }

    _asm {
        mov dx, 0f46h
        out dx, al
    }

    _asm {
        pop ds
        pop dx
        pop ax
        iret
    }
}

void init_dmaint() {
    setvect(int_vec, dmaint_handler);
}

void int_vector_init(unsigned char int_vec, void interrupt (*dmaint)()) {
    _disable(); // cli
    _dos_getvect(int_vec); // get vector
    _dos_setvect(int_vec, dmaint); // set vector
    _enable(); // sti
}


int main() {
    if (wss_check()) {
        printf("WSS found\n");
        dma_int_init() ;
        
        int_vector_init(0x25, dmaint);

        
		// Play the PCM data
		play_pcm();
    } else {
        printf("WSS not found\n");
    }
	

    return 0;
}
