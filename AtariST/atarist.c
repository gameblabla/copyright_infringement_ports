#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <osbind.h>
#include <mint/cookie.h>
#include "ikbd.h"
#include "stsprite.h"
#include "wizzcat.h"
#include "sound.h"
#include "atarist.h"

uint32_t isSoundDMA = 0;

uint16_t system_pal[16];
uint16_t savepal[16];

extern SoundDevice mainsound;

#define PRINT(x) (void)Cconws(x)


#define COLOR_REG_BASE 0xff8240
static void Save_HWPalette()
{
    volatile uint16_t* colorReg = (uint16_t*)COLOR_REG_BASE;
    for (uint8_t i = 0; i < 16; i++) { savepal[i] = colorReg[i]; }
}


typedef struct
{
  uint32_t magic ;
  uint32_t data ;
} COOKIE;

static COOKIE *cookie_jar(void)
{
	COOKIE *cookie ;
	cookie = *(COOKIE**) 0x5A0 ;
	return(cookie) ;
}

static COOKIE *cookie_find(char ident[4])
{
	COOKIE *cookie ;
	uint32_t   magic ;

	((char*)&magic)[0] = ident[0];
	((char*)&magic)[1] = ident[1];
	((char*)&magic)[2] = ident[2];
	((char*)&magic)[3] = ident[3];

	cookie = cookie_jar() ;
	if (cookie == NULL) return(cookie) ;
	while (cookie->magic != 0)
	{
		if (cookie->magic == magic) break;
		cookie++ ;
	}

	if (cookie->magic == 0) cookie = NULL ;
	return(cookie) ;
}


void Init_Machine()
{
	COOKIE* is_cookie_mch = cookie_find("_MCH");
	isSoundDMA = 0;
	mainsound = PSGPCM_device;
	if (is_cookie_mch->data != 0)
	{
		isSoundDMA = 1;
		mainsound = DMA_device;
	}
	
	Save_HWPalette();
	
	IKBD_Install();
	IKBD_MouseOn();
}

void Close_Machine()
{
	//Cconin();
	
	// Restore palette
	Setpalette(savepal);
	
	mainsound.Stop_Sound();
	mainsound.Uninit_Sound();
	
	IKBD_Flush();
	IKBD_Uninstall();	
}

unsigned char Load_Degas_PAL(char* path)
{
	short fp;

	// Open the file
	fp = Fopen(path, 0);
	if (fp < 0) {
		return 1;
	}

	// Skip the first 2 bytes
	Fseek(2, fp, 0);

	// Read palette
	Fread(fp, 32, system_pal);
	Setpalette(system_pal);

	// Close the file
	Fclose(fp);
	
	return 0;
}


unsigned char* Load_Degas(char* path, uint8_t loadpal)
{
	short fp;
	unsigned char* data;

	// Open the file
	fp = Fopen(path, 0);
	if (fp < 0) {
		return NULL;
	}

	Fseek(2, fp, 0);
	
	Fread(fp, 32, system_pal);
	if (loadpal) Setpalette(system_pal);

	// Allocate memory for data
	data = (unsigned char *)Malloc(32000);
	if (data == NULL) {
		Fclose(fp);
		return NULL;
	}
	
	Fseek(34, fp, 0);

	// Read the image data
	Fread(fp, 16000, data);
	Fread(fp, 16000, data + 16000);

	// Close the file
	Fclose(fp);
	
	return data;
}

unsigned char Load_Pal(char* path)
{
	short fp;

	// Open the file
	fp = Fopen(path, 0);
	if (fp < 0) {
		return 1;
	}

	// Read palette
	Fread(fp, 32, system_pal);
	Setpalette(system_pal);

	// Close the file
	Fclose(fp);
	
	return 0;
}

#define CHUNK_SIZE 16000

unsigned char* Load_Data(const char* path, int32_t size)
{
	unsigned short	handle;
	unsigned char* data;
	int32_t read_size = 0;
	
	handle = Fopen(path, 0);
	data = (unsigned char *)Malloc(size);
	
	// Read the file in chunks
	while(read_size < size) {
		int32_t current_chunk_size = CHUNK_SIZE;
		
		// If the remaining size is less than the chunk size, adjust the current chunk size
		if(size - read_size < CHUNK_SIZE) {
			current_chunk_size = size - read_size;
		}
		
		Fread(handle, current_chunk_size, data + read_size);
		//gemdos(63, handle, current_chunk_size, data + read_size);
		read_size += current_chunk_size;
	}
	
	Fclose(handle);
	
	return data;
}

void Free_Data(unsigned char* data)
{
	if (data)
	{
		Mfree(data);
		data = NULL;
	}
}

void* memset __P ((void* __s, int __c, size_t __n))
{
	uint32_t *dest = (uint32_t *)(__s);
	__n = __n >> 2;
	
	while (__n--) {                       
		*dest++ = __c;         
	}
	return dest;                              
}


// Generic memcpy

void* memcpy __P ((void*  __dest, const void*  __src, size_t __n))
{
	uint32_t *src = (uint32_t *)(__src);
	uint32_t *dest = (uint32_t *)(__dest);
	__n = __n >> 2;
	
	while (__n--) {                       
		*dest++ = *src++;         
	}
	return dest;                              
}

