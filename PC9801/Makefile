CC = wcl
MODE = c

DEFINES = -0 -ox -bcl=dos -m$(MODE) -s -I. -DTARGET_MSDOS=16

CFLAGS += $(DEFINES)

OUTPUT = GAME.EXE

SOURCES =  ./main.c gfx.c input.c

all: asm ${OUTPUT}

asm:
	nasm -f obj decomp/zx0.asm -o zx0.o
	nasm -f obj decomp/lzsa1.asm -o lzsa1.o
	
${OUTPUT}: ${SOURCES}
	${CC} ${CFLAGS} -fe=${OUTPUT} ${SOURCES} zx0.o lzsa1.o
	rm *.o *.err
	
clean:
	rm *.o ${OUTPUT} *.err
