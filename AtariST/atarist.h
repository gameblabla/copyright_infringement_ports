#ifndef ATARIST_H
#define ATARIST_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <osbind.h>
#include <mint/cookie.h>
#include "ikbd.h"
#include "stsprite.h"
#include "wizzcat.h"
#include "sound.h"

extern uint32_t init_mode;
extern uint32_t isSoundDMA;

#define NR_SPRITES 16
extern st_sprite_t spr[NR_SPRITES];

extern unsigned int lz4w_unpack(void * source, void *destination);

extern unsigned char* Load_Data(const char* path, int32_t size);
extern unsigned char Load_Degas_PAL(char* path);

#define COPY_DATA(src, dest, nr_longs)        \
    do {                                      \
        uint32_t *src_ptr = (uint32_t *)(src);  \
        uint32_t *dest_ptr = (uint32_t *)(dest); \
        uint16_t count = (nr_longs);            \
        while (count--) {                       \
            *dest_ptr++ = *src_ptr++;         \
        }                                     \
    } while(0)

extern void Init_Machine();

extern void Close_Machine();

extern unsigned char* Load_Degas(char* path, uint8_t loadpal);
extern unsigned char Load_Pal(char* path);

extern void drawChar(char ch, unsigned short x, unsigned short y, unsigned char color_index);
extern void Draw_Text_Center(const char* str, unsigned short y, unsigned short color_index);
extern void Draw_Text(const char* str, unsigned short x, unsigned short y, unsigned short color_index);
extern void Free_Data(unsigned char* data);

#endif
