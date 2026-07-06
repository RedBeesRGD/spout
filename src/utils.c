#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <string.h>
#include <ogc/machine/processor.h>
#include <wiiuse/wpad.h>

#include "utils.h"
#include "console_info.h"

u8 get_dolphin_type( void ) {
	#ifndef GAMECUBE_BUILD
	int fd = IOS_Open("/dev/dolphin", 1);
	if(fd >= 0) {
		IOS_Close(fd);
		return ConsoleType_DolphinWii;
	}
	IOS_Close(fd);
	#else
	// thanks Raiiri
	// Checks reload stub. Won't work if 'enable cheats' is checked
	// TODO: find a better way (some unemulated register?)
	if(read32(0x80001800) == 0 && read32(0x80001804) == 'STUB') {
		return ConsoleType_DolphinGC;
	}
	#endif
	return 0;
}

u32 wait_for_pad( void ) {
	while(1) {
		#ifndef GAMECUBE_BUILD
		WPAD_ScanPads();
		u32 pressed_w = WPAD_ButtonsDown(0);
		if(pressed_w) return pressed_w;
		#endif
		PAD_ScanPads();
		u32 pressed = PAD_ButtonsDown(0);
		if(pressed) return pressed;
	}
}

bool is_wii(u8 index) {
	if(index == ConsoleType_Wii || index == ConsoleType_DolphinWii) return 1;
	return 0;
}

bool is_gc(u8 index) {
	if(index == ConsoleType_GameCube || index == ConsoleType_DolphinGC) return 1;
	return 0;
}

void get_size_string(u32 value, char *buf, size_t buf_size) { // TODO: handle the weird cases better
							      // (I vibe coded this one. sorry..)
	if (value >= (1u << 30)) {
        	snprintf(buf, buf_size, "%u GiB", (unsigned)((value + (1u << 30) - 1) >> 30));
	} else if (value >= (1u << 20)) {
        	snprintf(buf, buf_size, "%u MiB", (unsigned)((value + (1u << 20) - 1) >> 20));
	} else if (value >= (1u << 10)) {
        	snprintf(buf, buf_size, "%u KiB", (unsigned)((value + (1u << 10) - 1) >> 10));
	} else {
        	snprintf(buf, buf_size, "%u B", value);
	}
}

u32 get_splash_size( void ) { // thanks Swiss
	if(SPLASH_GLOBAL) return SPLASH_GLOBAL;
	u16 mem_cfg = ((vu16 *)0xcc004000)[20] & 7; // 0xcc004028
						    // TODO: can this be moved to consoleinfo.h?
	
	switch (mem_cfg) { // TODO: Chip config info from BS1
        	case 0: // 16MiB
            		return 0x1000000;
            		break;
        	case 1: case 4: // 32MiB
            		return 0x2000000;
            		break;
        	case 2: case 6: // 24MiB
            		return 0x1800000;
            		break;
        	case 3: case 7: // 48MiB
            		return 0x3000000;
            		break;
        	case 5: // 64MiB
            		return 0x4000000;
            		break;
		default:
	    		return (u32)mem_cfg; // Will produce broken output later, but this really should
					     // never happen anyways..
	    		break;
    		}
}

u32 get_gddr_size( void ) { // thanks libogc-rice
	if(GDDR_GLOBAL) return GDDR_GLOBAL;

	return(1 << (32 - cntlzw(((0x20000 >> GDDR_RANKSEL) & 0x3f000) | ((GDDR_ROWMSK << GDDR_ROWSEL) & 0x3ffff)) + 10));
}
