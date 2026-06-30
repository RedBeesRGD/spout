#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <string.h>
#include <ogc/machine/processor.h>
#include <ogc/ipc.h>
#include <ogc/system.h>
#include <ogc/dvd.h>
#include <wiiuse/wpad.h>

#ifndef GAMECUBE_BUILD
#include "di.h"
#include "dvd.h"
#endif

#include "consoleinfo.h"
#include "utils.h"
#include "haxx.h"

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

int pvr = 0;
int ctype_index = 0;

char g_drive_date[10];

#ifndef GAMECUBE_BUILD

void make_drive_date(u32 date) {
	char temp[9] = {0};
	char day[3] = {0};
	char month[3] = {0};
	char year[6] = {0};

	sprintf(temp, "%08x", date);
	snprintf(year, sizeof(year), "%c%c%c%c", temp[0], temp[1], temp[2], temp[3]);
	sprintf(month, "%c%c", temp[4], temp[5]);
	sprintf(day, "%c%c", temp[6], temp[7]);


	char result[10] = {0};

			sprintf(g_drive_date, "%s.%s.%s", month, day, year);
}

int get_drive_code(u16 deviceCode) {
	switch(deviceCode) {
		case DVD_DEVICECODE_REALDRIVE:
			return DvdDevice_GCRetail;
		case DVD_DEVICECODE_NRDRIVE:
			if(ctype_index == ConsoleType_Wii) return DvdDevice_Fatal;
			return DvdDevice_GCNR;
		case DVD_DEVICECODE_RVLREALDRIVE:
			return DvdDevice_RVLRetail;
		case DVD_DEVICECODE_RVLRDRIVE:
			return DvdDevice_RVTR;

		case DVD_DEVICECODE_DDH:
			return DvdDevice_DDH;
		case DVD_DEVICECODE_NPDP:
			return DvdDevice_NPDP;
		case DVD_DEVICECODE_GDEV:
			return DvdDevice_GDEV;
		case DVD_DEVICECODE_NDEV:
			return DvdDevice_NDEV;

		case DVD_DEVICECODE_RVLH:
			return DvdDevice_RVTH;
		case DVD_DEVICECODE_RVA:
			return DvdDevice_RVA;
		case DVD_DEVICECODE_GCAM:
			return DvdDevice_Triforce;
		default:
			if(deviceCode & DVD_DEVICECODE_PRESENT) { // verify
				return DvdDevice_Present;
			} 
			else {
				return DvdDevice_Unknown;
			}
	}
}
#endif

int get_cpu_type( void ) {
	switch(pvr) {
		case PVR_LONESTAR_DD1:
			return CpuType_GekkoDD2orLonestar;
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
			return CpuType_Unknown;
		}
}

int get_chipset_type( void ) {
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

void get_ram_info(char *buf) {
	char substr[64];
	int dolphin = (ctype_index == ConsoleType_DolphinGC || ctype_index == ConsoleType_DolphinWii);
	strcpy(buf, "MEM1: ");
	ram_to_units(get_mem1_size(dolphin), substr, sizeof(substr));
	strcat(buf, substr);
	if(get_mem2_size()) {
		strcat(buf, "\n        MEM2: ");
		ram_to_units(get_mem2_size(), substr, sizeof(substr));
		strcat(buf, substr);
	}
}

int get_console_type( void ) {
	#ifndef GAMECUBE_BUILD
	int fd = IOS_Open("/dev/dolphin", 1);
	if(fd >= 0) {
		IOS_Close(fd);
		return ConsoleType_DolphinWii;
	}
	IOS_Close(fd);
	#else
	//thanks Raiiri
       	// If the reload stub has zero at its entry point then this is dolphin
	// won't work if 'enable cheats' is checked
	if (*(u32 *)0x80001800 == 0 && *(u32 *)0x80001804 == 'STUB') {
		return ConsoleType_DolphinGC;
	}
	#endif

	if(get_cpu_type() >= CpuType_BroadwayDD10 && get_cpu_type() <= CpuType_BroadwayDD20)
		return ConsoleType_Wii;
	if(IS_CAFE)
		return ConsoleType_WiiU;
	else
		return ConsoleType_GameCube;
}

void get_console_info(struct console_info* c_ptr) {
	int cputype_index = get_cpu_type();
	int chipset_type_index = get_chipset_type();
	ctype_index = get_console_type();
	char cpu_type[64];
	char chipset_type[64];
	char console_type[64];
	char ram_info[64];
	char drive_type[64];
	get_ram_info(ram_info);

	#ifndef GAMECUBE_BUILD
	DI_DriveID drive_info = init_dvd_info();
	int drive_index = get_drive_code(drive_info.dev_code);
	#endif

	u32 flipperId = ((vu32*)0xCC003000)[11];

	strcpy(cpu_type, cpu_type_str[cputype_index]);
	strcpy(chipset_type, chipset_type_str[chipset_type_index]);
	strcpy(console_type, console_type_str[ctype_index]);
	
	#ifndef GAMECUBE_BUILD
	strcpy(drive_type, dvd_device_str[drive_index]);
	#endif

	if(cputype_index == CpuType_Unknown) {
		if((pvr & PVR_BROADWAY_BASE) == 0x87000) {
			sprintf(cpu_type, "maybe Broadway DD%d.%d", 
			((pvr & 0x00000f00) >> 8), (pvr & 0x0000000f));
		}
		else {
			sprintf(cpu_type, "Unknown (PVR %08X)", pvr);
		}
	}
	if(ctype_index == ConsoleType_GameCube || ctype_index == ConsoleType_DolphinGC) {
		char flipper_rev_letter = 'A' + (flipperId >> 28);
		if(flipperId == 0x53200B1) {
			flipper_rev_letter = 'T';
		}
		sprintf(chipset_type, "Flipper rev %c", flipper_rev_letter);
	}

	strcpy(c_ptr->ram_info, ram_info);
	strcpy(c_ptr->cpu_type, cpu_type);
	strcpy(c_ptr->chipset_type, chipset_type);

	#ifndef GAMECUBE_BUILD
	strcpy(c_ptr->drive_type, drive_type);
	make_drive_date(drive_info.rel_date);
	strcpy(c_ptr->drive_date, g_drive_date);
	#endif

	strcpy(c_ptr->console_type, console_type);
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
	Haxx_GetBusAccess();
	#endif

	CON_EnableGecko(1, true);

	printf("\e[0;36mSpout\e[0;37m");
	#ifdef GAMECUBE_BUILD
	printf(" - GameCube Version");
	#else
	printf(" - Wii Version");
	#endif
	
	pvr = mfpvr();
	struct console_info c = {0};
	get_console_info(&c);

	printf("\n\n");
	printf("Console Type: %s\n", c.console_type);
	printf("CPU: %s\n", c.cpu_type);
	printf("Chipset: %s\n", c.chipset_type);
	printf("RAM info:\n        %s\n", c.ram_info);
	printf("\e[1GDrive type: %s\n", c.drive_type);
	printf("Drive date: %s\n", c.drive_date);

	while(SYS_MainLoop()) {
		#ifndef GAMECUBE_BUILD
			WPAD_ScanPads();
			u32 pressed_w = WPAD_ButtonsDown(0);
			if ( pressed_w & WPAD_BUTTON_HOME ) exit(0);
		#endif
		PAD_ScanPads();
		u32 pressed = PAD_ButtonsDown(0);
		if(pressed & PAD_BUTTON_A) exit(0);
		VIDEO_WaitVSync();
		printf("Returning to HBC...\n\n");
		exit(0);
	}

	return 0;
}
