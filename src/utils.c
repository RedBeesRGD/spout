#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <string.h>
#include <ogc/machine/processor.h>
#include <wiiuse/wpad.h>
#include "utils.h"


u32 get_mem1_size(int dolphin) {
    
    u32 mem_cfg = ((vu16 *)0xCC004000)[20] & 7;
    switch (mem_cfg) { //  0xCC004028
        case 0:
	    if(dolphin) return *(u32*)(0x80000028);
            return 0x1000000;
            break;
        case 1: case 4:
            return 0x2000000;
            break;
        case 2: case 6:
	    if(dolphin) return *(u32*)(0x80000028);
            return 0x1800000;
            break;
        case 3: case 7:
            return 0x3000000;
            break;
        case 5:
            return 0x4000000;
            break;
	default:
	    return *(u32*)(0x80000028);
	    break;
    }
    return 0;
}

u32 get_mem2_size(int dolphin) {
    #ifdef GAMECUBE_BUILD
    return 0;
    #else
    u32 DdrSize = 0;

    DdrSize = *(u32*)(0x80003120) - 0x90000000;
    u32 RealDdrSize = DdrSize;
    if ((read32(0xCD800064) == 0xFFFFFFFF) ? 1 : 0) {
        // We have full hardware access.
        // Disable DDR memory protection.
        write16(0xCD8B420A, 0);
        // We now can check various registers.
        // Devkits have two ranks of DDR.
        if (read16(0xCD8B4216) == 1) {
            // This is really 128MB, if this thing is running 64MB IOS then override the size
            if (RealDdrSize <= 0x4000000) RealDdrSize = 0x8000000;
        }
        // On vWii we can modify a single register and get 256MB of DDR
        if ((read32(0xCD8005A0) >> 16) == 0xCAFE) {
            write16(0xCD8B421A, 0x3FFF);
            RealDdrSize = 0x10000000;
        }
    }
 //   if(dolphin && RealDdrSize < 0x4000000) {
//	    return 0x4000000;
//	}

    return RealDdrSize;
    #endif
}

void ram_to_units(uint32_t bytes, char *buf, size_t buf_size) {
    if (bytes >= (1u << 30)) {
        snprintf(buf, buf_size, "%u GiB", (unsigned)((bytes + (1u << 30) - 1) >> 30));
    } else if (bytes >= (1u << 20)) {
        snprintf(buf, buf_size, "%u MiB", (unsigned)((bytes + (1u << 20) - 1) >> 20));
    } else if (bytes >= (1u << 10)) {
        snprintf(buf, buf_size, "%u KiB", (unsigned)((bytes + (1u << 10) - 1) >> 10));
    } else {
        snprintf(buf, buf_size, "%u B", bytes);
    }
}
