#include <vbe.h>
#include <v86.h>
#include <print.h>
#include <lib.h>

struct vbeModeInfo vbe_currentVbeMode;

struct vbeControllerInfo* vbe_getControllerInfo()
{
    struct vbeControllerInfo* info = (struct vbeControllerInfo*)REALMODE_PTR(VBEDATA_SEG, CONTROLLERINFO_OFF);
    strcpy(info->signature, "VBE2");   //Tell the VGABIOS we understand VBE2
    
    struct v86_biosArguments results = {.eax = VBEFUNC_GET_CONTROLLER_INFO, .es = VBEDATA_SEG, .edi = CONTROLLERINFO_OFF};
    v86_bioscall(0x10, results, &results);
    
    if(results.eax == 0x4f)
    {
        /*print_string_static("Query for VBE controller info was successful:\n");
        
        print_string_static("VBE Signature: ");
        print_string(info->signature);
        print_string_static("\nVBE Version: ");
        print_integer(info->version, 16);
        print_string_static("\nVBE OEM String: ");
        print_string((char*)REALMODE_PTR(info->oemString[1], info->oemString[0]));
        print_string_static("\nVideo Memory (K): ");
        print_integer(info->totalMemory*64, 10);
        print_string_static("\n");*/
        
        return info;
    }
    else
    {
        print_string_static("Query for VBE controller info was not successful - wrong return value in AX :'(\n");
        return NULL;
    }
}

struct vbeModeInfo* vbe_getModeInfo(uint16_t index)
{
    struct v86_biosArguments results = {.eax = VBEFUNC_GET_MODE_INFO, .ecx = index, .es = VBEDATA_SEG, .edi = MODEINFO_OFF};
    v86_bioscall(0x10, results, &results);
    
    if(results.eax == 0x4f)
        return (struct vbeModeInfo*)REALMODE_PTR(VBEDATA_SEG, MODEINFO_OFF);
    else
        return NULL;
}

bool vbe_setMode(uint16_t mode)
{
    struct vbeModeInfo* modeInfo = vbe_getModeInfo(mode);
    if(modeInfo)
    {
        //copy current mode into variable
        memcpy(&vbe_currentVbeMode, modeInfo, sizeof(struct vbeModeInfo));
    
        struct v86_biosArguments results = {.eax = VBEFUNC_SET_MODE, .ebx = mode};
        v86_bioscall(0x10, results, &results);
        
        if(results.eax != 0x4f)
        {
            print_string_static("Setting VBE mode was not successful - wrong return value in AX :'(\n");
            return FALSE;
        }
        
        return TRUE;
    }
    
    return FALSE;
}

uint16_t vbe_findClosestMode(uint16_t width, uint16_t height, uint8_t depth)
{
    struct vbeControllerInfo* controllerInfo = vbe_getControllerInfo();
    uint16_t* modeArray = (uint16_t*)REALMODE_PTR(controllerInfo->videomodes[1], controllerInfo->videomodes[0]);
    
    uint16_t bestmode = 0;
    
    uint16_t curDepthError = 0xFFFF;
    uint32_t curResError = 0xFFFFFFFF;
    
    while(*modeArray != 0xFFFF)
    {
        uint16_t curMode = *modeArray;
        
        struct vbeModeInfo* modeInfo = vbe_getModeInfo(*modeArray);
        
        modeArray++;
        
        if(!modeInfo)
            continue;
        
        //check if mode is available
        if(!(modeInfo->attributes & VBEMODE_ATTR_HWSUPPORT))
            continue;
        
        //check if mode is a graphical color mode
        if(!(modeInfo->attributes & VBEMODE_ATTR_GRAPHICS) || !(modeInfo->attributes & VBEMODE_ATTR_COLOR))
            continue;
        
        //check for linear framebuffer
        if(!(modeInfo->attributes & VBEMODE_ATTR_LINEARFRAMEBUF))
            continue;
        
        //check for packed pixel or direct color memory model
        if(modeInfo->memoryModel != VBEMODE_MEMMODEL_PACKED && modeInfo->memoryModel != VBEMODE_MEMMODEL_DIRECTCOL)
            continue;
        
        //exact match?
        if(width == modeInfo->Xres && height == modeInfo->Yres && modeInfo->bpp == depth)
            return curMode | (1 << 14);
        
        //if the width or height is higher than desired, don't use this mode
        if(modeInfo->Xres > width || modeInfo->Yres > height)
            continue;
        
        uint32_t deltaX = modeInfo->Xres - width;
        uint32_t deltaY = modeInfo->Yres - height;
        uint32_t resError = deltaX*deltaX + deltaY*deltaY;
        
        uint16_t depthError = (depth - modeInfo->bpp);
        
        if(resError < curResError || (resError == curResError && depthError < curDepthError))
        {
            curResError = resError;
            curDepthError = depthError;
            bestmode = curMode;
        }
    }
    
    if(!bestmode)
        return 0;
    
    //Set bit 14 for linear framebuffer
    return bestmode | (1 << 14);
}
