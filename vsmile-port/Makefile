#####################################################################
#																	 
#	Created by u'nSP IDE V4.0.1		22:28:54	03/16/23
#
#####################################################################




PRJDIR	= C:\Program Files (x86)\Generalplus\vsmile-ctrldemo

APPDIR	= C:\PROG~5P2\GENE~V14\UNSP~E1R.1

OUTDIR	= .\Debug

MK	= $(APPDIR)\make

CC	= $(APPDIR)\toolchain\gcc

AS	= $(APPDIR)\toolchain\xasm16

LD	= $(APPDIR)\toolchain\xlink16

AR	= $(APPDIR)\toolchain\xlib16

RESC	= $(APPDIR)\toolchain\resc

BHPCK	= $(APPDIR)\toolchain\BootPacker

RM	= del	/F	1>NUL	2>NUL

STRIP	= $(APPDIR)\toolchain\stripper

S37STRIP	= $(APPDIR)\toolchain\BinaryFileStripper

RD	= rd /S /Q

BOOTPACKER	= $(APPDIR)\toolchain\BootPacker 

LIKMODIFIER	= $(APPDIR)\toolchain\unSP_LIKModifier 

INCLUDES	= -I"C:/Program Files (x86)/Generalplus/vsmile-ctrldemo" -I"C:/Program Files (x86)/Generalplus/unSPIDE 4.0.1/library/include" -I"C:/Program Files (x86)/Generalplus/unSPIDE 4.0.1/library/include/sys"

BODY	= -body SPG200CartG -nobdy -bfile "C:\Program Files (x86)\Generalplus\vsmile-ctrldemo\vsmile-ctrldemo.bdy"

BODYFILE	= "C:\Program Files (x86)\Generalplus\vsmile-ctrldemo\vsmile-ctrldemo.bdy" 

BINFILE	= "$(OUTDIR)\vsmile-ctrldemo.TSK"

BINFILENOEXT	= $(OUTDIR)\vsmile-ctrldemo

ARYFILE	= "$(OUTDIR)\vsmile-ctrldemo.ary"

SBMFILE	= "$(OUTDIR)\vsmile-ctrldemo.sbm"

OPT	= -S -Wall -mglobal-var-iram

ASFLAGS	= -t5 -d -sr

CASFLAGS	= -t5 -sr -wpop

CFLAGS	= $(OPT) -B$(APPDIR)\toolchain\ $(INCLUDES) 

BINTYPE	= -at

LDFLAGS	=  -blank 0x00

EXTRAFLAGS	= 


OBJFILES	= \
	"$(OUTDIR)\font_bin.res" \
	"$(OUTDIR)\frames_bin.res" \
	"$(OUTDIR)\frames_pal_bin.res" \
	"$(OUTDIR)\title_bin.res" \
	"$(OUTDIR)\title_pal_bin.res" \
	"$(OUTDIR)\yami_bin.res" \
	"$(OUTDIR)\yami_pal_bin.res" \
	"$(OUTDIR)\black_bin.res" \
	"$(OUTDIR)\main.obj" \
	"$(OUTDIR)\Resource.obj" 

"$(OUTDIR)\font_bin.res": "C:\Program Files (x86)\Generalplus\vsmile-ctrldemo\font.bin"
	$(RESC) "C:\Program Files (x86)\Generalplus\vsmile-ctrldemo\font.bin" "$(OUTDIR)\font_bin.res" RES_FONT_BIN 

"$(OUTDIR)\frames_bin.res": "C:\Program Files (x86)\Generalplus\vsmile-ctrldemo\frames.bin"
	$(RESC) "C:\Program Files (x86)\Generalplus\vsmile-ctrldemo\frames.bin" "$(OUTDIR)\frames_bin.res" RES_FRAMES_BIN 

"$(OUTDIR)\frames_pal_bin.res": "C:\Program Files (x86)\Generalplus\vsmile-ctrldemo\frames_pal.bin"
	$(RESC) "C:\Program Files (x86)\Generalplus\vsmile-ctrldemo\frames_pal.bin" "$(OUTDIR)\frames_pal_bin.res" RES_FRAMES_PAL_BIN 

"$(OUTDIR)\title_bin.res": "C:\Program Files (x86)\Generalplus\vsmile-ctrldemo\title.bin"
	$(RESC) "C:\Program Files (x86)\Generalplus\vsmile-ctrldemo\title.bin" "$(OUTDIR)\title_bin.res" RES_TITLE_BIN 

"$(OUTDIR)\title_pal_bin.res": "C:\Program Files (x86)\Generalplus\vsmile-ctrldemo\title_pal.bin"
	$(RESC) "C:\Program Files (x86)\Generalplus\vsmile-ctrldemo\title_pal.bin" "$(OUTDIR)\title_pal_bin.res" RES_TITLE_PAL_BIN 

"$(OUTDIR)\yami_bin.res": "C:\Program Files (x86)\Generalplus\vsmile-ctrldemo\yami.bin"
	$(RESC) "C:\Program Files (x86)\Generalplus\vsmile-ctrldemo\yami.bin" "$(OUTDIR)\yami_bin.res" RES_YAMI_BIN 

"$(OUTDIR)\yami_pal_bin.res": "C:\Program Files (x86)\Generalplus\vsmile-ctrldemo\yami_pal.bin"
	$(RESC) "C:\Program Files (x86)\Generalplus\vsmile-ctrldemo\yami_pal.bin" "$(OUTDIR)\yami_pal_bin.res" RES_YAMI_PAL_BIN 

"$(OUTDIR)\black_bin.res": "C:\Program Files (x86)\Generalplus\vsmile-ctrldemo\black.bin"
	$(RESC) "C:\Program Files (x86)\Generalplus\vsmile-ctrldemo\black.bin" "$(OUTDIR)\black_bin.res" RES_BLACK_BIN 

"$(OUTDIR)\main.asm": "C:\Program Files (x86)\Generalplus\vsmile-ctrldemo\main.c" ".\Resource.h" 
	set PATH="$(APPDIR)\toolchain\";%PATH% & \
	$(CC) $(CFLAGS) -o "$(OUTDIR)\main.asm" "C:/Program Files (x86)/Generalplus/vsmile-ctrldemo/main.c" 

"$(OUTDIR)\main.obj": "$(OUTDIR)\main.asm"
	$(AS) $(CASFLAGS) $(INCLUDES) -o "$(OUTDIR)\main.obj" "$(OUTDIR)\main.asm" 

"$(OUTDIR)\Resource.obj": "C:\Program Files (x86)\Generalplus\vsmile-ctrldemo\Resource.asm" 
	$(AS) $(ASFLAGS) $(INCLUDES) -o "$(OUTDIR)\Resource.obj" "C:\Program Files (x86)\Generalplus\vsmile-ctrldemo\Resource.asm" 


.SUFFIXES : .c .asm .obj .s37 .tsk .res

all :	 BEFOREBUILD "$(OUTDIR)" $(BINFILE)

BEFOREBUILD :

"$(OUTDIR)" :
	if not exist "$(OUTDIR)\$(NULL)" mkdir "$(OUTDIR)"

$(BINFILE) : $(OBJFILES) 
	$(LD) $(BINTYPE) $(ARYFILE) $(BINFILE) $(LDFLAGS) $(BODY) $(EXTRAFLAGS) -undefined-opt __TgP190708CM -undefined-opt __TgP190708CL -undefined-opt __TgP190708M

compile :	 $(OBJFILES)

CLEANFILES = \
	"$(OUTDIR)\font_bin.res" \
	"$(OUTDIR)\frames_bin.res" \
	"$(OUTDIR)\frames_pal_bin.res" \
	"$(OUTDIR)\title_bin.res" \
	"$(OUTDIR)\title_pal_bin.res" \
	"$(OUTDIR)\yami_bin.res" \
	"$(OUTDIR)\yami_pal_bin.res" \
	"$(OUTDIR)\black_bin.res" \
	"$(OUTDIR)\main.obj" \
	"$(OUTDIR)\main.lst" \
	"$(OUTDIR)\main.asm" \
	"$(OUTDIR)\Resource.obj" \
	"$(OUTDIR)\Resource.lst" \
	"$(BINFILENOEXT).s37" "$(BINFILENOEXT).tsk" "$(BINFILENOEXT)_SPI.bin*" "$(BINFILENOEXT).hdb" "$(BINFILENOEXT).lod" "$(BINFILENOEXT).map" "$(BINFILENOEXT).sbm" "$(BINFILENOEXT).sym" "$(BINFILENOEXT).smy" \
	"$(SBMFILE)

clean :
	$(RM) $(wordlist 1,65536,$(CLEANFILES))

.c.asm:
	$(CC) $(CFLAGS) $(INCLUDES) -o "$(OUTDIR)\$@" $<

.asm.obj:
	$(AS) $(ASFLAGS) $(INCLUDES) -o "$(OUTDIR)\$@" $<

