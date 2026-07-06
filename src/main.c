#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <string.h>
#include <ogc/machine/processor.h>
#include <ogc/ipc.h>
#include <ogc/system.h>
#include <ogc/dvd.h>
#include <wiiuse/wpad.h>

#include "console_info.h"
#include "utils.h"

#ifndef GAMECUBE_BUILD
#include "3rdparty/libhaxx.h"
#include "3rdparty/libseeprom.h"
#endif

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

struct console c = {0};

u8 get_cpu_type( void ) {
	switch(c.pvr) {
		case PVR_LONESTAR_DD1:
			return CpuType_LonestarDD1;
		case PVR_LONESTAR_DD20:
			return CpuType_LonestarDD20;
		case PVR_LONESTAR_DD22:
			return CpuType_LonestarDD22;

		case PVR_GEKKO_DD10:
			return CpuType_GekkoDD1or2;
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
		
		case PVR_ESPRESSO_ES:
			return CpuType_EspressoEs;
		case PVR_ESPRESSO:
			return CpuType_Espresso;
	
		default:
			if((c.pvr & PVR_BROADWAY_BASE) == 0x87000) return CpuType_UnknownBroadway;
			return CpuType_UnknownGeneric;
			// TODO: Implement guessing for Gekko
			// and Espresso
		}
}

u8 get_base_console_type( void ) {
	u8 dolphin_type = get_dolphin_type();
	if(dolphin_type) return dolphin_type;

	if(c.cpu_type_index >= CpuType_BroadwayDD10 && c.cpu_type_index <= CpuType_UnknownBroadway) 
		// TODO: handle RVL_EMU (no Broadway, but I guess it's a Wii?)
		return ConsoleType_Wii;
	if(IS_CAFE) // TODO: Handle Cortado? 
		    // (no Latte so won't have this register, but is Wii U)
		return ConsoleType_WiiU;
	else
		return ConsoleType_GameCube;
}

int get_chipset_type( void ) {
	if (c.is_wii) { // TODO: handle BEB (Broadway + no Hollywood)
					       // and RVL_EMU
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
				return ChipsetType_UnknownHollywood;
		}
	} else if(c.is_cafe) {
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
			default:
				return ChipsetType_UnknownLatte;
			}
	} else {
		c.flipper_rev_letter = 'A' + (c.flipper_id >> 28);
		if(c.flipper_id == 0x53200B1) {
			c.flipper_rev_letter = 'T';
		}
		switch(c.flipper_rev_letter) {
			case 'A':
				return ChipsetType_FlipperRevA;
			case 'B':
				return ChipsetType_FlipperRevB;
			case 'C':
				return ChipsetType_FlipperRevC;
			case 'T':
				return ChipsetType_FlipperRevT;
			default:
				return ChipsetType_UnknownFlipper;
			}
	}
			
	return ChipsetType_UnknownGeneric;
}

void get_ram_info_str(char *buf) {
	char substr[64];
	if(c.is_gc) { // TODO: handle RVL_EMU
		      // (Will be seen as a wii, but
		      // I think its splash should be
		      // called MRAM..)
		strcpy(buf, "MRAM: ");
	} else {
		strcpy(buf, "MEM1: ");
	}

	get_size_string(c.splash_size, substr, sizeof(substr));
	strcat(buf, substr);
	if(!c.is_gc) {
		strcat(buf, "\n        MEM2: ");
		get_size_string(c.gddr_size, substr, sizeof(substr));
		strcat(buf, substr);
	}
}

#ifndef GAMECUBE_BUILD // TODO: Make drive stuff work on gamecube.
		       // (It depends on RVL DI, right now..)

void get_drive_info( void ) {
	DI_Init(); // TODO: error handling
		   // and rewrite the
		   // old liberror!
	DI_Identify(&c.drive_info);
}

u8 get_drive_type( void ) {
	switch(c.drive_info.dev_code) {
		case DRIVE_TYPE_GCNROM:
			return DriveType_GCNROM;
		case DRIVE_TYPE_NR:
			if(!c.is_gc) return DriveType_Fatal; // TODO: RVL_EMU, BEB
							     // TODO: verify
			return DriveType_GCNR;
		case DRIVE_TYPE_NPDP:
			return DriveType_NPDP;
		case DRIVE_TYPE_DDH:
			return DriveType_DDH;
		case DRIVE_TYPE_GDEV:
			return DriveType_GDEV;
		case DRIVE_TYPE_TRIFORCE:
			return DriveType_Triforce;

		case DRIVE_TYPE_WIINROM:
			return DriveType_WiiNROM;
		case DRIVE_TYPE_RVTR:
			return DriveType_RVTR;
		case DRIVE_TYPE_NDEV:
			return DriveType_NDEV;
		case DRIVE_TYPE_RVA:
			return DriveType_RVA;

		default:
			return DriveType_Unknown;
	}
}

void get_drive_date_str( void ) { // thanks SysCheck
				  // (pretty much verbatim)
	char temp[9] = {0};
	char day[3] = {0};
	char month[3] = {0};
	char year[6] = {0};

	sprintf(temp, "%08x", c.drive_info.rel_date);
	snprintf(year, sizeof(year), "%c%c%c%c", temp[0], temp[1], temp[2], temp[3]);
	sprintf(month, "%c%c", temp[4], temp[5]);
	sprintf(day, "%c%c", temp[6], temp[7]);


	char result[10] = {0};

	sprintf(c.drive_date_str, "%s.%s.%s", month, day, year);
}

#endif

void get_full_console_type_str( void ) { // TODO: actually do it
	strcpy(c.full_console_type_str, base_console_type_str_list[c.base_console_type_index]);
}

void get_all_console_info( void ) {
	c.pvr = mfpvr();
	c.cpu_type_index = get_cpu_type();
	c.base_console_type_index = get_base_console_type();

	// params
	c.is_wii = is_wii(c.base_console_type_index);
	c.is_cafe = (c.base_console_type_index == ConsoleType_WiiU);
	c.is_gc = is_gc(c.base_console_type_index);

	if(c.is_gc) { // TODO: Handle RVL_EMU, BEB
		      // (base wii, but has flipper)
		c.flipper_id = ((vu32*)0xcc003000)[11]; // TODO: can this be moved to consoleinfo.h?
							// Also, what's the actual name of this register?
	}

	if(c.is_wii) { // TODO: Handle RVL_EMU, BEB
		       // (base wii, but has no HW)
		c.hw_chiprevid = HW_CHIPREVID;
	}
	
	if(c.is_cafe) { // TODO: Handle Cortado
			// (base wiiu, but has no Latte)
		c.lt_chiprevid = LT_CHIPREVID;
	}

	c.splash_size = get_splash_size();
	if(!c.is_gc) { // TODO: Handle RVL_EMU, BEB
		       // (base !gc, but has no GDDR)
		c.gddr_size = get_gddr_size();
		c.gddr_rowsel = GDDR_ROWSEL;
		c.gddr_ranksel = GDDR_RANKSEL;
		c.gddr_rowmsk = GDDR_ROWMSK;
	}
	
	#ifndef GAMECUBE_BUILD
	get_drive_info();
	#endif
	
	// indices
	c.chipset_type_index = get_chipset_type();
	#ifndef GAMECUBE_BUILD
	c.drive_type_index = get_drive_type();
	#endif
	
	// strings
	strcpy(c.cpu_type_str, cpu_type_str_list[c.cpu_type_index]);
	if(c.cpu_type_index == CpuType_UnknownBroadway) {
		sprintf(c.cpu_type_str, c.cpu_type_str, c.pvr, ((c.pvr & 0x00000f00) >> 8), (c.pvr & 0x0000000f));
	}
	if(c.cpu_type_index == CpuType_UnknownGeneric) {
		sprintf(c.cpu_type_str, c.cpu_type_str, c.pvr);
	}

	strcpy(c.chipset_type_str, chipset_type_str_list[c.chipset_type_index]);
	if(c.chipset_type_index == ChipsetType_UnknownFlipper) {
		sprintf(c.chipset_type_str, c.chipset_type_str, c.flipper_id, c.flipper_rev_letter);
	}
	if(c.chipset_type_index == ChipsetType_UnknownHollywood) {
		sprintf(c.chipset_type_str, c.chipset_type_str, c.hw_chiprevid);
	}
	if(c.chipset_type_index == ChipsetType_UnknownLatte) {
		sprintf(c.chipset_type_str, c.chipset_type_str, c.lt_chiprevid);
	}

	get_ram_info_str(c.ram_info_str); // TODO: maybe make all these
					  // get str functions
					  // consistent
	#ifndef GAMECUBE_BUILD
	strcpy(c.drive_type_str, drive_type_str_list[c.drive_type_index]);
	if(c.drive_type_index == DriveType_Unknown) {
		sprintf(c.drive_type_str, c.drive_type_str, c.drive_info.dev_code);
	}

	get_drive_date_str();
	#endif

	get_full_console_type_str();
}

void libogc_init( void ) {

	VIDEO_Init();

	#ifndef GAMECUBE_BUILD
	WPAD_Init();
	#endif
	PAD_Init();
	rmode = VIDEO_GetPreferredMode(NULL);
	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(xfb);
	VIDEO_SetBlack(false);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();

	printf("\x1b[2;0H");
}

int main( void ) {
	libogc_init();
	#ifndef GAMECUBE_BUILD
	if(!Haxx_GetBusAccess()) {
		printf("Fatal error: Couldn't get bus access!\n\nPress any button to return to loader");
		wait_for_pad();
		exit(0);
	}
	#endif

	CON_EnableGecko(1, true);

	printf("\e[0;36mSpout\e[0;37m");
	#ifdef GAMECUBE_BUILD
	printf(" - GameCube Version");
	#else
	printf(" - Wii Version");
	#endif

	get_all_console_info();
	
	printf("\n\n");
	printf("Console Type: %s\n", c.full_console_type_str);
	printf("CPU: %s\n", c.cpu_type_str);
	printf("Chipset: %s\n", c.chipset_type_str);
	printf("RAM info:\n        %s\n", c.ram_info_str);
	printf("\e[1G");
	#ifndef GAMECUBE_BUILD
	printf("Drive type: %s\n", c.drive_type_str);
	printf("Drive date: %s\n", c.drive_date_str);
	#endif

	//SEEPROMDisplayInfo();
	
	printf("\n\nPress any button to exit..");
	
	wait_for_pad();
	exit(0);

	return 0;
}
