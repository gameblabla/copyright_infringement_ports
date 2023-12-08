CC = i386-pc-msdosdjgpp-gcc
CFLAGS += -Oz -ffast-math -fomit-frame-pointer -fdata-sections -ffunction-sections -fipa-pta -fno-common -fstack-usage -Wstack-usage=160 -fno-short-enums -flto
CFLAGS += -fno-PIC  -fno-semantic-interposition
CFLAGS += -Wshadow -Wformat=2 -Wformat-truncation -Wundef -Wpadded -Wunused-parameter
CFLAGS += -march=i386 -mtune=i386 -Iinclude -Iinput/pc98
CFLAGS += -Wall -I. -Isound 
CFLAGS += -I./generic -I./graphics -I. -I./graphics/vga -I./input/dos -Isound/music -Ivideo/pc9821 -Ivideo -Isound -Isb -Isbdma -Isound/wave -Isound/music
LDFLAGS = -lc -Wl,--as-needed -Wl,--gc-sections -flto -s -no-pie

OUTPUT = GAME.exe

SOURCES =  ./main.c  video/pc9821/pc98.c sound/music/pmd/musicp.c sound/music/null/musnull.c sound/wave/pmd/pmdpcm.c sound/wave/null/wavenull.c input/pc98/input.c
OBJS = ${SOURCES:.c=.o}

all: asm ${OUTPUT}

${OUTPUT}:${OBJS}
	${CC} ${CFLAGS} -o TEMP.exe ${OBJS} aplib.o ${LDFLAGS} 
	i386-pc-msdosdjgpp-strip --strip-all TEMP.exe
	exe2coff TEMP.exe
	cat CWSDSTUB.EXE TEMP > ${OUTPUT}
	upx ${OUTPUT} --ultra-brute
	
asm:
	nasm -f coff decomp/aplib.asm -o aplib.o
	
clean:
	rm $(OBJS) aplib.o
	rm ${OUTPUT}
