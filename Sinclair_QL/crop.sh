#!/bin/sh
dd if=FRAME1.scr8 of=FRAME1C.SC8 bs=1 count=20480
#dd if=FRAME2.scr8 of=FRAME2C.SC8 bs=1 count=20480
dd if=FRAME3.scr8 of=FRAME2C.SC8 bs=1 count=20480
dd if=FRAME4.scr8 of=FRAME3C.SC8 bs=1 count=20480
dd if=FRAME5.scr8 of=FRAME4C.SC8 bs=1 count=20480
dd if=FRAME6.scr8 of=FRAME5C.SC8 bs=1 count=20480

java -jar lz4w.jar p FRAME1C.SC8 frame1
#java -jar lz4w.jar p FRAME2C.SC8 FRAME2
java -jar lz4w.jar p FRAME2C.SC8 frame2
java -jar lz4w.jar p FRAME3C.SC8 frame3
java -jar lz4w.jar p FRAME4C.SC8 frame4
java -jar lz4w.jar p FRAME5C.SC8 frame5

rm FRAME1C.SC8 FRAME2C.SC8 FRAME3C.SC8 FRAME4C.SC8 FRAME5C.SC8 FRAME6C.SC8
