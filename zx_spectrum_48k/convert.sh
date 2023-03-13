dd if=frame1.scr bs=1 skip=$(($(stat -c %s frame1.scr)-768)) count=768 of=frame1clr.scr
dd if=frame2.scr bs=1 skip=$(($(stat -c %s frame1.scr)-768)) count=768 of=frame2clr.scr
dd if=frame3.scr bs=1 skip=$(($(stat -c %s frame1.scr)-768)) count=768 of=frame3clr.scr
dd if=frame4.scr bs=1 skip=$(($(stat -c %s frame1.scr)-768)) count=768 of=frame4clr.scr
dd if=frame5.scr bs=1 skip=$(($(stat -c %s frame1.scr)-768)) count=768 of=frame5clr.scr
dd if=frame1.scr bs=1 count=$(echo "scale=0; (6114*0.6875+0.5)/1" | bc) of=frame1c.scr
dd if=frame2.scr bs=1 count=$(echo "scale=0; (6114*0.6875+0.5)/1" | bc) of=frame2c.scr
dd if=frame3.scr bs=1 count=$(echo "scale=0; (6114*0.6875+0.5)/1" | bc) of=frame3c.scr
dd if=frame4.scr bs=1 count=$(echo "scale=0; (6114*0.6875+0.5)/1" | bc) of=frame4c.scr
dd if=frame5.scr bs=1 count=$(echo "scale=0; (6114*0.6875+0.5)/1" | bc) of=frame5c.scr
dd if=frame6.scr bs=1 count=$(echo "scale=0; (6114*0.6875+0.5)/1" | bc) of=frame6c.scr
zx1 -f frame1clr.scr frame1clr.scr.zx1
zx1 -f frame2clr.scr frame2clr.scr.zx1
zx1 -f frame3clr.scr frame3clr.scr.zx1
zx1 -f frame4clr.scr frame4clr.scr.zx1
zx1 -f frame5clr.scr frame5clr.scr.zx1
zx1 -f frame1c.scr frame1.scr.zx1
zx1 -f frame2c.scr frame2.scr.zx1
zx1 -f frame3c.scr frame3.scr.zx1
zx1 -f frame4c.scr frame4.scr.zx1
zx1 -f frame5c.scr frame5.scr.zx1
#rcs -f frame1.scr
#rcs -f frame2.scr
#rcs -f frame3.scr
#rcs -f frame4.scr
#rcs -f frame5.scr
#zx1 -f frame1.scr.rcs
#zx1 -f frame2.scr.rcs
#zx1 -f frame3.scr.rcs
#zx1 -f frame4.scr.rcs
#zx1 -f frame5.scr.rcs
