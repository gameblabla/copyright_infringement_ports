/*****************************************************************************
*               VBE 2.0 detection and routines for DJGPP C.                  *
*****************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <dpmi.h>
#include <go32.h>
#include <conio.h>
#include <dos.h>
#include <sys/nearptr.h>
#include <sys/farptr.h>
#include <unistd.h>
#include <malloc.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include "types.h"
#include "vesa.h"

#ifdef DJGPP
// Doing so greatly reduces size
#include <crt0.h>
char **__crt0_glob_function (char *arg) { return 0; }
void   __crt0_load_environment_file (char *progname) { }
void   __crt0_setup_arguments (void) { }
#endif


#define PRINT_STUB(a)
#define PRINTF_STUB

VBESURFACE   _vbe_surface;
static VBEINFO      _vbe_info;
static MODEINFO     _vbe_mode_info;
static PMINTERFACE *_pmode_interface_ptr;
static ulong       *_ytable;

static void (*PM_VBEsetDisplayStart)(void);

static unsigned char Internal_SVGA_Palette[768];

void Load_palette(const char *file)
{
    int fd;
    int index;
    unsigned totalsize;
    _dos_open(file, O_RDONLY, &fd);
    _dos_read(fd, Internal_SVGA_Palette, sizeof(Internal_SVGA_Palette), &totalsize);
    _dos_close(fd);
    
	for(index = 0; index < 256; index += 1)
	{
        outportb(0x3C8, index);
        outportb(0x3C9, Internal_SVGA_Palette[(index * 3) + 0] >> 2);
        outportb(0x3C9, Internal_SVGA_Palette[(index * 3) + 1] >> 2);
        outportb(0x3C9, Internal_SVGA_Palette[(index * 3) + 2] >> 2);
	}
}

unsigned char* Load_RAW_Bitmap(const char *file, unsigned long size)
{
    int fd;
    unsigned totalsize;
    unsigned char *bitmap;
    
    bitmap = malloc(size);
    if (!bitmap) return NULL;
    
    _dos_open(file, O_RDONLY, &fd);
    _dos_read(fd, bitmap, size, &totalsize);
    _dos_close(fd);
    
	return bitmap;
}


/*****************************************************************************
*                                                                            *
*    VBEgetInfo returns a pointer to a VBEINFO structure or NULL on error    *
*                                                                            *
*****************************************************************************/
VBEINFO *VBEgetInfo(void)
{
    ushort counter = 0;
    //ushort offset  = 0;
    ushort vbe_mode = 0xFFFF;
    ushort *vbe_mode_list = 0;
    __dpmi_regs r;

    // we want VBE 2.0+ info
    strncpy(_vbe_info.vbe_signature, "VBE2", 4);

    // request info
    r.x.ax = 0x4F00;
    r.x.di = __tb & 0x0F;
    r.x.es = (__tb >> 4) & 0xFFFF;
    dosmemput(&_vbe_info, sizeof(VBEINFO), __tb);
    __dpmi_int(0x10, &r);
    if(r.x.ax != 0x004F) return(0);
    dosmemget(__tb, sizeof(VBEINFO), &_vbe_info);

    if(strncmp(_vbe_info.vbe_signature, "VESA", 4) != 0) return(0);  // VESA ?

    // get the videomode list
    do {
        dosmemget(((((long)_vbe_info.video_mode_ptr >> 16) & 0xFFFF) << 4) +
                  ((long)_vbe_info.video_mode_ptr & 0xFFFF) +
                  (counter * sizeof(short)),
                  sizeof(short), &vbe_mode);
        counter++;
    } while(vbe_mode != 0xFFFF);
    vbe_mode_list = malloc((counter + 1) * sizeof(short));
    counter = 0;

    do {
        dosmemget(((((long)_vbe_info.video_mode_ptr >> 16) & 0xFFFF) << 4) +
                  ((long)_vbe_info.video_mode_ptr & 0xFFFF) +
                  (counter * sizeof(short)),
                  sizeof(short), &vbe_mode_list[counter]);
        counter++;
    } while(vbe_mode_list[counter-1] != 0xFFFF);
    _vbe_info.video_mode_ptr = vbe_mode_list;
    counter = 0;

#if 0
    // get the OEM string
    do {
        dosmemget(((((long)_vbe_info.oem_string_ptr >> 16) & 0xFFFF) << 4) +
                  ((long)_vbe_info.oem_string_ptr & 0xFFFF) + counter,
                  sizeof(char), &_vbe_info.oem_data[counter]);
        counter++;
    } while(_vbe_info.oem_data[counter-1] != 0);
    _vbe_info.oem_string_ptr = &_vbe_info.oem_data[0];
    offset  = counter;
    counter = 0;

    if(_vbe_info.vbe_version >= 0x0200) {

        // get the vendor name
        do {
            dosmemget(((((long)_vbe_info.oem_vendor_name_ptr >> 16) & 0xFFFF) << 4) +
                      ((long)_vbe_info.oem_vendor_name_ptr & 0xFFFF) + counter,
                      sizeof(char), &_vbe_info.oem_data[counter+offset]);
            counter++;
        } while(_vbe_info.oem_data[counter+offset-1] != 0);
        _vbe_info.oem_vendor_name_ptr = &_vbe_info.oem_data[offset];
        offset  = offset + counter;
        counter = 0;

        // get the product name
        do {
            dosmemget(((((long)_vbe_info.oem_product_name_ptr >> 16) & 0xFFFF) << 4) +
                      ((long)_vbe_info.oem_product_name_ptr & 0xFFFF) + counter,
                      sizeof(char), &_vbe_info.oem_data[counter+offset]);
            counter++;
        } while(_vbe_info.oem_data[counter+offset-1] != 0);
        _vbe_info.oem_product_name_ptr = &_vbe_info.oem_data[offset];
        offset  = offset + counter;
        counter = 0;

        //get the product revision
        do {
            dosmemget(((((long)_vbe_info.oem_product_rev_ptr >> 16) & 0xFFFF) << 4) +
                      ((long)_vbe_info.oem_product_rev_ptr & 0xFFFF) + counter,
                      sizeof(char), &_vbe_info.oem_data[counter+offset]);
            counter++;
        } while(_vbe_info.oem_data[counter+offset-1] != 0);
        _vbe_info.oem_product_rev_ptr = &_vbe_info.oem_data[offset];
    }
#endif

    _vbe_surface.vbe_boolean = 1;
    return(&_vbe_info);
}

/*****************************************************************************
*                                                                            *
* VBEgetModeInfo takes a ModeNr on entry and returns a pointer to a          *
* ModeInfoBlock Structure or NULL on failure.                                *
*                                                                            *
*****************************************************************************/
MODEINFO *VBEgetModeInfo(unsigned short vbe_mode_nr)
{
    __dpmi_regs r;

    if(_vbe_surface.vbe_boolean == 0) return(0);

    r.x.ax = 0x4F01;
    r.x.cx = vbe_mode_nr;
    r.x.di = __tb & 0x0F;
    r.x.es = (__tb >> 4) & 0xFFFF;
    __dpmi_int(0x10, &r);

    if(r.x.ax != 0x004F) return(0);

    dosmemget(__tb, sizeof(MODEINFO), &_vbe_mode_info);

    if(_vbe_mode_info.bits_per_pixel == 16) // for buggy VBE implementations
        _vbe_mode_info.bits_per_pixel = _vbe_mode_info.red_mask_size +
                                        _vbe_mode_info.green_mask_size +
                                        _vbe_mode_info.blue_mask_size;
    return(&_vbe_mode_info);
}

/*****************************************************************************
*                                                                            *
*                      Get protected mode interface                          *
*                                                                            *
*****************************************************************************/
void *VBEgetPmodeInterface(void)
{
    __dpmi_regs r;
    __dpmi_meminfo m;
    ushort *ptr;

    // only available in VBE 2.0+
    if(_vbe_info.vbe_version < 0x200)  return(0);

    r.x.ax = 0x4F0A;
    r.x.bx = 0;
    __dpmi_int(0x10, &r);
    if(r.x.ax != 0x004F) return(0);

    // copy interface
    _pmode_interface_ptr = malloc(r.x.cx);
    dosmemget((r.x.es * 16) + r.x.di, r.x.cx, _pmode_interface_ptr);
    _go32_dpmi_lock_data(_pmode_interface_ptr, r.x.cx);

    // need memory-mapped IO?
    if(_pmode_interface_ptr->io_info) {
        ptr = (ushort *)((char *)_pmode_interface_ptr + _pmode_interface_ptr->io_info);
        while(*ptr != 0xFFFF) ptr++;
        ptr++;                                       // skip the port table and
        if(*ptr != 0xFFFF) {                         // get descriptor
            m.address = *((ulong *)ptr);
            m.size = *(ptr + 2);
            if(__dpmi_physical_address_mapping(&m) != 0) return(0);

            _vbe_surface.io_linear = m.address;
            __dpmi_lock_linear_region(&m);

            _vbe_surface.io_segment = __dpmi_allocate_ldt_descriptors(1);
            if(_vbe_surface.io_segment < 0) {
                __dpmi_unlock_linear_region(&m);
                __dpmi_free_physical_address_mapping(&m);
                return(0);
            }

            __dpmi_set_segment_base_address(_vbe_surface.io_segment,
                                            _vbe_surface.io_linear);
            __dpmi_set_segment_limit(_vbe_surface.io_segment, m.size - 1);

        }
    }

    PM_VBEsetDisplayStart = (void *)((char *)_pmode_interface_ptr +
                                     _pmode_interface_ptr->set_display_start);
    return(PM_VBEsetDisplayStart);
}

/*****************************************************************************
*                                                                            *
* VBEselectModeNr searches the vbe_mode_list for a Mode that matches the       *
* requirements given on entry (xres, yres, bpp).                             *
*                                                                            *
*****************************************************************************/
ushort VBEselectModeNr(ushort xres, ushort yres, uchar bpp)
{
    ushort counter;

    if(_vbe_surface.vbe_boolean == 0) return(0);

    for(counter = 0; ; counter++) {
        if(_vbe_info.video_mode_ptr[counter] == 0xFFFF) return(0);

        VBEgetModeInfo(_vbe_info.video_mode_ptr[counter]);

        if(_vbe_mode_info.x_resolution  == xres &&
                _vbe_mode_info.y_resolution  == yres &&
                _vbe_mode_info.bits_per_pixel == bpp) break;
    }

    return(_vbe_info.video_mode_ptr[counter]);
}


/*****************************************************************************
*                                                                            *
*       VBEsetScanlineLength returns the MaxNum of logical scanlines         *
*                                                                            *
*****************************************************************************/
static inline ushort VBEsetScanlineLength(ushort pixel_length)
{
    __dpmi_regs r;

    if(_vbe_surface.vbe_boolean == 0) return(0);

    r.x.ax = 0x4F06;
    r.x.bx = 0x0000;
    r.x.cx = pixel_length;
    __dpmi_int(0x10, &r);
    if(r.h.ah != 0) return(0);
    if(r.x.cx != pixel_length) return(0);

    _vbe_surface.virtual_x_resolution = pixel_length;

    return(r.x.dx);
}

/*****************************************************************************
*                                                                            *
* Set inquired Mode and return a VbeSurface Structure or Null on failure     *
*                                                                            *
*****************************************************************************/
VBESURFACE *VBEsetMode(ushort vbe_mode_nr)
{
    __dpmi_regs r;
    __dpmi_meminfo m;
    ulong counter;

    if(_vbe_surface.vbe_boolean == 0) return(0);
    if(_vbe_surface.vbe_init_boolean == 1) return(0);
    if(VBEgetModeInfo(vbe_mode_nr) == 0) return(0);

    // create lookup table for putPixel routines
    if(_ytable) free(_ytable);
    if((_ytable = malloc(4 * (_vbe_mode_info.y_resolution + 1))) == 0) return(0);
    for(counter = 0; counter <= _vbe_mode_info.y_resolution; counter++)
        _ytable[counter] = _vbe_mode_info.x_resolution * counter *
                           ((_vbe_mode_info.bits_per_pixel + 1) / 8);

    // request frame buffer
    r.x.ax = 0x4F02;
    r.x.bx = (vbe_mode_nr | 0x4000);
    __dpmi_int(0x10, &r);
    if(r.x.ax != 0x004F) return(0);

    m.size = (_vbe_info.total_memory * 64 * 1024);
    m.address = _vbe_mode_info.phys_base_ptr;
    __dpmi_physical_address_mapping(&m);
    __dpmi_lock_linear_region(&m);

    _vbe_surface.lfb_linear_address = m.address;
    _vbe_surface.lfb_selector = __dpmi_allocate_ldt_descriptors(1);

    __dpmi_set_segment_base_address(_vbe_surface.lfb_selector,
                                    _vbe_surface.lfb_linear_address);
    __dpmi_set_segment_limit(_vbe_surface.lfb_selector,
                             (_vbe_info.total_memory * 64 * 1024) - 1);

    _vbe_surface.lfb_near_ptr = (void*)(_vbe_surface.lfb_linear_address
                                        + __djgpp_conventional_base);
    _vbe_surface.x_resolution         = _vbe_mode_info.x_resolution;
    _vbe_surface.y_resolution         = _vbe_mode_info.y_resolution;
    _vbe_surface.bits_per_pixel       = _vbe_mode_info.bits_per_pixel;
    _vbe_surface.virtual_x_resolution = _vbe_mode_info.x_resolution;
    _vbe_surface.bytes_per_pixel      = (_vbe_mode_info.bits_per_pixel + 1) / 8;
    _vbe_surface.center_x             = _vbe_surface.virtual_x_resolution / 2;
    _vbe_surface.center_y             = _vbe_surface.y_resolution / 2;
    _vbe_surface.number_of_offscreens =
        VBEsetScanlineLength(_vbe_mode_info.x_resolution) / _vbe_mode_info.y_resolution;
    _vbe_surface.vbe_init_boolean     = 1;
    _vbe_surface.screen_bytes         = (_vbe_surface.x_resolution *
                                         _vbe_surface.y_resolution *
                                         _vbe_surface.bytes_per_pixel);
    _vbe_surface.screen_dwords        = _vbe_surface.screen_bytes / 4;
    _vbe_surface.lfb_mem_size         = (_vbe_surface.x_resolution *
                                         _vbe_surface.y_resolution *
                                         _vbe_surface.bytes_per_pixel) / 1024 +
                                        _vbe_mode_info.off_screen_mem_size;

    for(counter = 0; counter < (_vbe_surface.lfb_mem_size * 1024); counter++)
        _farpokeb(_vbe_surface.lfb_selector, counter, 0x0); // clear Lfb

    if(_vbe_surface.offscreen_ptr) free(_vbe_surface.offscreen_ptr);
    if((_vbe_surface.offscreen_ptr = malloc(_vbe_surface.screen_bytes)) == 0)
        return(0);
    else
        memset(_vbe_surface.offscreen_ptr, 0, _vbe_surface.screen_bytes);

    /*if(_vbe_surface.bits_per_pixel == 8) {
        putPixel = putPixel8;
        VGAcreatePalette();
    }
    if(_vbe_surface.bits_per_pixel == 16) putPixel = putPixel16;
    if(_vbe_surface.bits_per_pixel == 15) putPixel = putPixel16;
    if(_vbe_surface.bits_per_pixel == 32) putPixel = putPixel32;*/

    return(&_vbe_surface);
}

/*****************************************************************************
*                                                                            *
*           VBEmodeInit switches to the desired resolution                   *
*                                                                            *
*****************************************************************************/
VBESURFACE *VBEmodeInit(ushort xres, ushort yres, uchar bpp)
{
    ushort vbe_mode_nr;
    __dpmi_meminfo m;

    if(_vbe_surface.vbe_boolean == 0) return(0);
    if(_vbe_surface.vbe_init_boolean == 0) return(0);

    m.size = (_vbe_info.total_memory * 64 * 1024);
    m.address = _vbe_surface.lfb_linear_address;
    __dpmi_unlock_linear_region(&m);
    __dpmi_free_physical_address_mapping(&m);
    __dpmi_free_ldt_descriptor(_vbe_surface.lfb_selector);

    _vbe_surface.vbe_init_boolean = 0;

    if((vbe_mode_nr = VBEselectModeNr(xres, yres, bpp)) == NULL) return(0);

    vbe_mode_nr |= 0x8000;
    if(VBEsetMode(vbe_mode_nr) == NULL) return(0);

    return(&_vbe_surface);
}


/*****************************************************************************
*                                                                            *
*                       VBEsetDisplayStart: let's scroll!                    *
*                                                                            *
*****************************************************************************/
#if 1
void VBEsetDisplayStart(ulong pixel, ulong scanline)
{
    __dpmi_regs r;
    long address;
    long selector;

    if(PM_VBEsetDisplayStart) {
        address = (_ytable[scanline] + (pixel * _vbe_surface.bytes_per_pixel)) >> 2;

        selector = (_vbe_surface.io_segment) ? _vbe_surface.io_segment : _my_ds();

        asm(
            "  pushw %%es ; "
            "  movw %w1, %%es ; "          // set the IO segment
            "  call *%0 ; "                // call the VESA function
            "  popw %%es "
            :                              // no outPRINT_STUB

            : "S"(PM_VBEsetDisplayStart),  // function pointer in esi
            "a"(selector),                 // IO segment in eax
            "b"(0x0080),                   // mode in ebx
            "c"(address & 0xFFFF),         // low word of address in ecx
            "d"((address >> 16))           // high word of address in edx

            : "memory", "%edi", "%cc"      // touches edi and flags
        );

        return;
    }

    r.x.ax = 0x4F07;
    r.x.bx = 0x0080;
    r.x.cx = pixel;
    r.x.dx = scanline;
    __dpmi_int(0x10, &r);

}
#endif

/*****************************************************************************
*                                                                            *
*                         VBEgetVbeSurfacePtr(void)                          *
*                                                                            *
*****************************************************************************/
VBESURFACE *VBEgetVbeSurfacePtr(void)
{
    return(&_vbe_surface);
}

/*****************************************************************************
*                                                                            *
*                         VBEgetModeInfoPtr(void)                            *
*                                                                            *
*****************************************************************************/
MODEINFO *VBEgetModeInfoPtr(void)
{
    return(&_vbe_mode_info);
}

/*****************************************************************************
*                                                                            *
*   VBEinit silently enters the desired mode and returns VbeSurface          *
*                                                                            *
*****************************************************************************/
VBESURFACE *VBEinit(ushort xres, ushort yres, uchar bpp)
{
    ushort vbe_mode_nr;

    if(VBEgetInfo() == NULL) {
        PRINT_STUB("\nNo Vesa Bios Extension found...");
        return(0);
    }

    if(_vbe_info.vbe_version < 0x0200) {
        PRINT_STUB("\nVBE Version 2.0 or better required...");
        return(0);
    }

    VBEgetPmodeInterface();

    if((vbe_mode_nr = VBEselectModeNr(xres, yres, bpp)) == NULL) {
        PRINT_STUB("\nNo appropriate Videomode available...");
        return(0);
    }

    if(VBEsetMode(vbe_mode_nr) == NULL)
        return(0);
    else
        return(&_vbe_surface);
}

/*****************************************************************************
*                                                                            *
*   VBEinit shows an info screen then enters the mode, returns VbeSurface    *
*                                                                            *
*****************************************************************************/
VBESURFACE *VBEinfoInit(ushort xres, ushort yres, uchar bpp, ushort delay_ms)
{
    ushort vbe_mode_nr;
    char  *LFBmsg[] = { "NOLFB present", "LFB present"};
    signed char counter;

    PRINT_STUB("\nVBE2.0 driver v1.0 (c) 1999, Tobias Koch <tobias.koch@gmail.com>\n");

    if(VBEgetInfo() == NULL) {
        PRINT_STUB("\n\nNo VESA Bios Extension found...\n\n");
        return(0);
    }
    PRINTF_STUB("Video Memory      - %d KB           \n"
           "VBE version       - %d.%d  detected \n"
           "OEM spec.         - %s              \n",
           _vbe_info.total_memory * 64,
           _vbe_info.vbe_version >> 8, _vbe_info.vbe_version & 0x00FF,
           _vbe_info.oem_string_ptr);

    if(_vbe_info.vbe_version >= 0x0200) {
        PRINTF_STUB("OEM software rev. - %d.%d\n"
               "OEM vendor name   - %s   \n"
               "OEM product name  - %s   \n"
               "OEM product rev.  - %s   \n",
               _vbe_info.oem_software_rev >> 8, _vbe_info.oem_software_rev & 0x00FF,
               _vbe_info.oem_vendor_name_ptr,
               _vbe_info.oem_product_name_ptr,
               _vbe_info.oem_product_rev_ptr);
    } else PRINT_STUB("\nVESA Bios Extension 2.0 or better required! "
                    "\nAttempting to set the mode anyway...");

    if(VBEgetPmodeInterface())
        PRINT_STUB("\nProtected Mode Interface detected ... ");

    if((vbe_mode_nr = VBEselectModeNr(xres, yres, bpp)) == NULL) {
        PRINT_STUB("\nNo appropriate Videomode Available...");
        return(0);
    } else if(VBEgetModeInfo(vbe_mode_nr) != NULL) {
        PRINTF_STUB("\nMode %Xh - %dx%dx%dbpp - RGB %d:%d:%d - %s\n\n",
               vbe_mode_nr,
               _vbe_mode_info.x_resolution,
               _vbe_mode_info.y_resolution,
               _vbe_mode_info.bits_per_pixel,
               _vbe_mode_info.red_mask_size,
               _vbe_mode_info.green_mask_size,
               _vbe_mode_info.blue_mask_size,
               LFBmsg[((_vbe_mode_info.mode_attributes) & (1<<7)) >> 7]);
    }

    delay_ms /= 10; // 10...9...8...7...6...5...4...3...2...1...GO!
    if(delay_ms > 0) {
        for(counter = 10; counter > 0; counter--) {
            delay(delay_ms);
            PRINTF_STUB("%d..", counter);
        }
        delay(delay_ms);
        PRINTF_STUB("Launch");
        delay(delay_ms);
    } else getch();

    if(VBEsetMode(vbe_mode_nr) == NULL) {
        VBEshutdown();
        PRINT_STUB("\n\nUnable to access linear frame buffered mode!"
             "\nTry loading UNIVBE by Scitech Software (tm).  ");
        return(0);
    }

    return(&_vbe_surface);
}

/*****************************************************************************
*                                                                            *
*                VBEshutdown cleans up descriptors, memory, etc.             *
*                                                                            *
*****************************************************************************/
int VBEshutdown(void)
{
    __dpmi_regs r;
    __dpmi_meminfo m;

    if(_vbe_surface.vbe_init_boolean == 0) {
        r.x.ax = 0x03;               // make sure we're in 3h
        __dpmi_int(0x10, &r);        // for buggy vesa implementations
        PRINT_STUB("\nuninitialized...");
        return(1);
    }

    // free mapping etc.
    m.size = (_vbe_info.total_memory * 64 * 1024);
    m.address = _vbe_surface.lfb_linear_address;
    __dpmi_unlock_linear_region(&m);
    __dpmi_free_physical_address_mapping(&m);
    __dpmi_free_ldt_descriptor(_vbe_surface.lfb_selector);

    // get rid of PMI interface
    if(_pmode_interface_ptr) free(_pmode_interface_ptr);
    if(_vbe_surface.io_segment) {
        m.address = _vbe_surface.io_linear;
        __dpmi_free_physical_address_mapping(&m);
        __dpmi_free_ldt_descriptor(_vbe_surface.io_segment);
    }

    if(_vbe_surface.offscreen_ptr) free(_vbe_surface.offscreen_ptr);

    // return do DOS
    r.x.ax = 0x03;
    __dpmi_int(0x10, &r);

    // deinit VBE surface
    _vbe_surface.vbe_init_boolean = 0;
    _vbe_surface.vbe_boolean = 0;

    PRINT_STUB("Elvis has left the building...");

    // dealloc mode list
    free(_vbe_info.video_mode_ptr);
    return(0);
}

/*****************************************************************************
*                                                                            *
*                         Wait for Vertical Retrace                          *
*                                                                            *
*****************************************************************************/
/*void VGAwaitVrt(void)
{
    while(inportb(0x3DA) & 8);
    while(!(inportb(0x3DA) & 8));
}*/

/*****************************************************************************
*                                                                            *
*                  putPixel for 8Bit Screen modes                            *
*                                                                            *
*****************************************************************************/
/*
void putPixel8(long x, long y, ulong color)
{
    x = +x + _vbe_surface.center_x;
    y = -y + _vbe_surface.center_y;

    _farpokeb(_vbe_surface.lfb_selector, _ytable[y] + x , color);
}*/

/*****************************************************************************
*                                                                            *
*                  putPixel for 16Bit Screen modes                           *
*                                                                            *
*****************************************************************************/
/*void putPixel16(long x, long y, ulong color)
{
    x = +x + _vbe_surface.center_x;
    y = -y + _vbe_surface.center_y;

    _farpokew(_vbe_surface.lfb_selector, _ytable[y] + (x << 1), color);
}*/

/*****************************************************************************
*                                                                            *
*                  putPixel for 32Bit Screen modes                           *
*                                                                            *
*****************************************************************************/
/*void putPixel32(long x, long y, ulong color)
{
    x = +x + _vbe_surface.center_x;
    y = -y + _vbe_surface.center_y;

    _farpokel(_vbe_surface.lfb_selector, _ytable[y] + (x << 2), color);
}*/

/*****************************************************************************
*                                                                            *
*                      Flip Offsreen to active Screen                        *
*                                                                            *
*****************************************************************************/

/*
inline void flipScreen(void)
{
    _movedatal(_my_ds(), (long)_vbe_surface.offscreen_ptr, _vbe_surface.lfb_selector, 0x0, _vbe_surface.screen_dwords);
}*/

