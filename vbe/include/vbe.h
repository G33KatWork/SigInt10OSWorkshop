#ifndef _VBE_H_
#define _VBE_H_

#include <types.h>

//define some neat places in realmode memory for data that is returned by INT 10h BIOS calls
#define VBEDATA_SEG         0x2000
#define CONTROLLERINFO_OFF  0x0
#define MODEINFO_OFF        0x200

//flags for vbe controller capabilities
#define VBECTRL_CAP_DAC8BIT         (1 << 0)
#define VBECTRL_CAP_VGACOMPAT       (1 << 1)
#define VBECTRL_CAP_RAMDACBLANK     (1 << 2)
#define VBECTRL_CAP_STEREOSIGNAL    (1 << 3)
#define VBECTRL_CAP_STEREOEVCCON    (1 << 4)

//flags for vbe mode attributes
#define VBEMODE_ATTR_HWSUPPORT      (1 << 0)
#define VBEMODE_ATTR_BIOSTTY        (1 << 2)
#define VBEMODE_ATTR_COLOR          (1 << 3)
#define VBEMODE_ATTR_GRAPHICS       (1 << 4)
#define VBEMODE_ATTR_NOVGACOMPAT    (1 << 5)
#define VBEMODE_ATTR_NOVGACOMPATMEM (1 << 6)
#define VBEMODE_ATTR_LINEARFRAMEBUF (1 << 7)
#define VBEMODE_ATTR_DOUBLESCAN     (1 << 8)
#define VBEMODE_ATTR_INTERLACED     (1 << 9)
#define VBEMODE_ATTR_HWTRIPLEBUF    (1 << 10)
#define VBEMODE_ATTR_HWSTEREO       (1 << 11)
#define VBEMODE_ATTR_DUALDISPSTARTADDR  (1 << 12)

//types of vbe mode memory models
#define VBEMODE_MEMMODEL_TEXT       0x00
#define VBEMODE_MEMMODEL_CGA        0x01
#define VBEMODE_MEMMODEL_HERCULES   0x02
#define VBEMODE_MEMMODEL_PLANAR     0x03
#define VBEMODE_MEMMODEL_PACKED     0x04
#define VBEMODE_MEMMODEL_NONCHAIN4  0x05
#define VBEMODE_MEMMODEL_DIRECTCOL  0x06
#define VBEMODE_MEMMODEL_YUV        0x07

//vbe functions
#define VBEFUNC_GET_CONTROLLER_INFO     0x4F00
#define VBEFUNC_GET_MODE_INFO           0x4F01
#define VBEFUNC_SET_MODE                0x4F02
#define VBEFUNC_GET_MODE                0x4F03
#define VBEFUNC_SAVERESTORE_STATE       0x4F04
#define VBEFUNC_DISPLAY_WINDOW_CONTROL  0x4F05
#define VBEFUNC_GETSET_SCANLINE_LENGTH  0x4F06
#define VBEFUNC_SETGET_DISPLAY_START    0x4F07
#define VBEFUNC_SETGET_DAC_PALETTE_FORMAT   0x4F08
#define VBEFUNC_SETGET_PALETTE_DATA     0x4F09
#define VBEFUNC_RETURN_PMODE_INTERFACE  0x4F0A

struct vbeControllerInfo {
   char     signature[4];             // == "VESA"
   uint16_t version;                 // == 0x0300 for VBE 3.0
   uint16_t oemString[2];            // isa vbeFarPtr
   uint32_t capabilities;
   uint16_t videomodes[2];           // isa vbeFarPtr
   uint16_t totalMemory;             // as # of 64KB blocks
   uint16_t oemSoftwareRev;
   uint16_t oemVendorNamePtr[2];
   uint16_t oemProductNamePtr[2];
   uint16_t oemProductRevPtr[2];
   uint8_t  reserved[222];
   uint8_t  oemData[256];
} __attribute__ ((packed));

struct vbeModeInfo {
  uint16_t attributes;
  uint8_t  winA, winB;
  uint16_t granularity;
  uint16_t winsize;
  uint16_t segmentA, segmentB;
  uint16_t realFctPtr[2];
  uint16_t bytesPerScanline;
 
  uint16_t Xres, Yres;
  uint8_t  Wchar, Ychar, planes, bpp, banks;
  uint8_t  memoryModel, bankSize, imagePages;
  uint8_t  reserved0;
 
  uint8_t redMaskSize, redPosition;
  uint8_t greenMaskSize, greenPosition;
  uint8_t blueMaskSize, bluePosition;
  uint8_t rsvMaskSize, rsvPosition;
  uint8_t directcolorAttributes;
 
  uint32_t physbase;  //framebuffer address
  uint32_t offscreenMemOffset;
  uint16_t offscreenMemSize;
  
  uint8_t  reserved1[206];
} __attribute__ ((packed));

struct vbePmodeInterface {
    uint16_t setWindowCallPtr[2];
    uint16_t setDisplayStartPtr[2];
    uint16_t setSetPrimaryPalettePtr[2];
    uint16_t portAndMemAccessTablePtr[2];
    uint8_t  remainder[1];
} __attribute__ ((packed));

//as soon as vbe_setMode is called, this will be filled with the current mode
extern struct vbeModeInfo vbe_currentVbeMode;

struct vbeControllerInfo* vbe_getControllerInfo(void);

//CAREFUL! The returned pointer is only valid until this function is called again!
struct vbeModeInfo* vbe_getModeInfo(uint16_t index);
bool vbe_setMode(uint16_t mode);

uint16_t vbe_findClosestMode(uint16_t width, uint16_t height, uint8_t depth);

#endif
