#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <string.h>
#include <ogc/machine/processor.h>
#include <wiiuse/wpad.h>
#include "consoleinfo.h"

#define VERSION_MAJOR 0
#define VERSION_MINOR 1

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

// from swiss
static bool is_gamecube( void ) {
	u32 pvr = mfpvr();
	return ((pvr >> 16) & 0xFFFF) == 0x0008 && ((pvr >> 12) & 0xF) != 0x7;
}

int get_cpu_type(u32 pvr) {
	switch(pvr) {
		case PVR_LONESTAR_DD20:
			return CpuType_LonestarDD20;
		case PVR_LONESTAR_DD22:
			return CpuType_LonestarDD22;

		case PVR_GEKKO_DD10:
			return CpuType_GekkoDD10;
		case PVR_GEKKO_DD20: // or lonestar dd1
			return CpuType_GekkoDD20;
		case PVR_GEKKO_DD23: // or b or e
			return CpuType_GekkoDD23;
		case PVR_GEKKO_DD23a: // or ei
			return CpuType_GekkoDD23a;
		case PVR_GEKKO_DD24:
			return CpuType_GekkoDD24;
		case PVR_GEKKO_DD24e:
			return CpuType_GekkoDD24e;
		case PVR_GEKKO_DD40:
			return CpuType_GekkoDD40;

		case PVR_BROADWAY_DD10:
			return CpuType_BroadwayDD10;
		case PVR_BROADWAY_DD101:
			return CpuType_BroadwayDD101;
		case PVR_BROADWAY_DD12:
			return CpuType_BroadwayDD12;
		case PVR_BROADWAY_DD12i:
			return CpuType_BroadwayDD12i;
		case PVR_BROADWAY_DD13:
			return CpuType_BroadwayDD13;
		case PVR_BROADWAY_DD13i:
			return CpuType_BroadwayDD13i;
		case PVR_BROADWAY_DD20:
			return CpuType_BroadwayDD20;
		default:
			return CpuType_Unknown;
		}
}

int get_chipset_type( void ) {
	if(is_gamecube()) {
		}

	if (!IS_CAFE) {
		switch(HW_CHIPREVID) {
			case HOLLYWOOD_ES1_0:
				return ChipsetType_HollywoodES10;
			case HOLLYWOOD_ES1_1:
				return ChipsetType_HollywoodES11;
			case HOLLYWOOD_ES1_2:
				return ChipsetType_HollywoodES12;
			case HOLLYWOOD_ES2_0:
				return ChipsetType_HollywoodES20;
			case HOLLYWOOD_ES2_1:
				return ChipsetType_HollywoodES21;
			case HOLLYWOOD_ES3_0:
				return ChipsetType_HollywoodES30;
			case HOLLYWOOD_ES3_1:
				return ChipsetType_HollywoodES31;
			default:
				return ChipsetType_Unknown;
		}
	} else {
		switch(LT_CHIPREVID) {
			case LATTE_A11:
				return ChipsetType_LatteA11;
			case LATTE_A12:
				return ChipsetType_LatteA12;
			case LATTE_A2X:
				return ChipsetType_LatteA2X;
			case LATTE_A3X:
				return ChipsetType_LatteA3X;
			case LATTE_A4X:
				return ChipsetType_LatteA4X;
			case LATTE_A5X:
				return ChipsetType_LatteA5X;
			case LATTE_B1X:
				return ChipsetType_LatteB1X;
			case LATTE_B1X70:
				return ChipsetType_LatteB1X70;
			}
	}
			
	return ChipsetType_Unknown;
}

int get_console_type( void ) {
	int fd = IOS_Open("/dev/dolphin", 1);
	if(fd >= 0) {
		IOS_Close(fd);
		return ConsoleType_Dolphin;
	}
	IOS_Close(fd);
	
	if(IS_CAFE)
		return ConsoleType_WiiU;
	else
		return ConsoleType_Wii;
}

void get_console_info(struct console_info* c_ptr) {
	// 3 phases:
	// Variables are retrieved by helper function
	// Strings are permuted for special cases
	// Strings are copied into struct
	u32 pvr = mfpvr();
	int cputype_index = get_cpu_type(pvr);
	int chipset_type_index = get_chipset_type();
	int ctype_index = get_console_type();
	char cpu_type[64];
	char chipset_type[64];
	char console_type[64];
	
	strcpy(cpu_type, cpu_type_str[cputype_index]);
	strcpy(chipset_type, chipset_type_str[chipset_type_index]);
	strcpy(console_type, console_type_str[ctype_index]);
	
	if(cputype_index == CpuType_Unknown) {
		if((pvr & PVR_BROADWAY_BASE) == 0x87000) {
			sprintf(cpu_type, "maybe Broadway DD%d.%d", 
			((pvr & 0x00000f00) >> 8), (pvr & 0x0000000f));
		}
	}
	
	strcpy(c_ptr->cpu_type, cpu_type);
	strcpy(c_ptr->chipset_type, chipset_type);
	strcpy(c_ptr->console_type, console_type);
}

void libogc_init( void ) {
	//---------------------------------------------------------------------------------

	// Initialise the video system
	VIDEO_Init();

	// This function initialises the attached controllers
	WPAD_Init();

	// Obtain the preferred video mode from the system
	// This will correspond to the settings in the Wii menu
	rmode = VIDEO_GetPreferredMode(NULL);

	// Allocate memory for the display in the uncached region
	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

	// Initialise the console, required for printf
	console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);

	// Set up the video registers with the chosen mode
	VIDEO_Configure(rmode);

	// Tell the video hardware where our display memory is
	VIDEO_SetNextFramebuffer(xfb);

	// Make the display visible
	VIDEO_SetBlack(false);

	// Flush the video register changes to the hardware
	VIDEO_Flush();

	// Wait for Video setup to complete
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();


	// The console understands VT terminal escape codes
	// This positions the cursor on row 2, column 0
	// we can use variables for this with format codes too
	// e.g. printf ("\x1b[%d;%dH", row, column );
	printf("\x1b[2;0H");
}

int main(int argc, char **argv) {
	libogc_init();
	
	struct console_info c;
	struct console_info* c_ptr;
	c_ptr = &c;

	get_console_info(c_ptr);
	 
	printf("Spout v%i.%i by RedBees - raregamingdump.ca\n", VERSION_MAJOR, VERSION_MINOR);
	printf("Console Type: %s\n", c.console_type);
	printf("CPU: %s\n", c.cpu_type);
	printf("Chipset: %s\n", c.chipset_type);

	while(SYS_MainLoop()) {

		// Call WPAD_ScanPads each loop, this reads the latest controller states
		WPAD_ScanPads();

		// WPAD_ButtonsDown tells us which buttons were pressed in this loop
		// this is a "one shot" state which will not fire again until the button has been released
		u32 pressed = WPAD_ButtonsDown(0);

		// We return to the launcher application via exit
		if ( pressed & WPAD_BUTTON_HOME ) exit(0);

		// Wait for the next frame
		VIDEO_WaitVSync();
	}

	return 0;
}
