#ifndef GAMECUBE_BUILD
#include "3rdparty/libdi.h"
#endif
#ifdef GAMECUBE_BUILD
#include <gctypes.h> // TODO: bleh
#endif

#define IS_CAFE (*(vu16*)0xcd8005a0 == 0xCAFE)

// PVRs

#define PVR_LONESTAR_DD1	0x00080100
#define PVR_LONESTAR_DD20	0x00080200 // 750L
#define PVR_LONESTAR_DD22	0x00088202

#define PVR_GEKKO_DD10		0x70000100
#define PVR_GEKKO_DD20_INTENDED	PVR_LONESTAR_DD1 // Apparently, actual Gekko DD2.0 had the DD1.0 PVR 
						 // written by mistake.
						 // TODO: Also 750CX PVRs??
#define PVR_GEKKO_DD23		0x00083213
#define PVR_GEKKO_DD23a		0x00083203
#define PVR_GEKKO_DD23b		PVR_GEKKO_DD23 // why??
#define PVR_GEKKO_DD23e		PVR_GEKKO_DD23
#define PVR_GEKKO_DD23ei	PVR_GEKKO_DD23a 
#define PVR_GEKKO_DD24		0x00083204
#define PVR_GEKKO_DD24e		0x00083214
#define PVR_GEKKO_DD40		0x00083410 
// 2.3=2.3b=2.3e, 2.3a=2.3ei

#define PVR_BROADWAY_BASE	0x00087000
#define PVR_BROADWAY_DD10	0x00087100
#define PVR_BROADWAY_DD101	0x00087110
#define PVR_BROADWAY_DD12	0x00087102
#define PVR_BROADWAY_DD12i	0x00087112
#define PVR_BROADWAY_DD13	0x00087103
#define PVR_BROADWAY_DD13i	0x00087113
#define PVR_BROADWAY_DD20	0x00087200

#define PVR_ESPRESSO_ES	0x70010200 // This is specifically the one found in that board from GBAtemp.
				   // Other Espresso PVRs are unknown.. I don't even know what
				   // the revision is called
				   // TODO: figure this out (Or maybe guess?? It's probably
				   // 7xx for all Espresso, 1.2/1.21 or something)
#define PVR_ESPRESSO	0x70010201

// Chipset types (no Flipper, sorry, you're special)

#define HOLLYWOOD_ES1_0		0x00000000      // ES1.00
#define HOLLYWOOD_ES1_1		0x00000001      // ES1.10
#define HOLLYWOOD_ES1_2		0x00000002      // ES1.21
#define HOLLYWOOD_ES2_0		0x00000010      // ES2.01
#define HOLLYWOOD_ES2_1		0x00000011      // ES2.11
#define HOLLYWOOD_ES3_0		0x00000020      // ES3.0
#define HOLLYWOOD_ES3_1		0x00000021      // ES3.1

#define LATTE_A11		0x10
#define LATTE_A12		0x18
#define LATTE_A2X		0x21
#define LATTE_A3X		0x31
#define LATTE_A4X		0x40
#define LATTE_A5X		0x50
#define LATTE_B1X		0x60
#define LATTE_B1X70		0x70

// DVD drive types
// TODO: verify gamecube/wii differences.. and make it actually work on gamecube..
// TODO: wiiu??

#define DRIVE_TYPE_ADDR	0x800030E6

// MEI devices, 0x00xx
// AMC device, 0x01xx
// Hudson devices, 0x02xx
// Arcade devices, 0x03xx

#define DRIVE_TYPE_GCNROM	0x0000 // NROM = retail
#define DRIVE_TYPE_NR		0x0001
#define DRIVE_TYPE_NPDP		0x0200

#define DRIVE_TYPE_DDH		0x0100
#define DRIVE_TYPE_GDEV		0x0201

#define DRIVE_TYPE_TRIFORCE	0x1000 // TODO: what was that note about 'not what it returns for get device ID'?

#define DRIVE_TYPE_WIINROM	0x0002
#define DRIVE_TYPE_RVTR		0x0003
#define DRIVE_TYPE_RVTH		0x0203

#define DRIVE_TYPE_NDEV		0x0202

#define DRIVE_TYPE_RVA		0x0300 // Revolution Arcade

#define DRIVE_IS_PRESENT	0x8000 // TODO: verify

// misc. registers and globals
#define HW_CHIPREVID 	(*(vu32*)0xcd800214)
#define LT_CHIPREVID 	(*(vu8*)0xcd8005a3)

#define SPLASH_GLOBAL	(*(u32*)0x80000028)
#define GDDR_GLOBAL	(*(u32*)0x80003118)

#define GDDR_ROWSEL	(*(u16*)0xcd8b4212) & 0x7
#define GDDR_RANKSEL	(*(u16*)0xcd8b4216) & 0x7
#define GDDR_ROWMSK	(*(u16*)0xcd8b421a) & 0x3fff

typedef enum {
	CpuType_LonestarDD1 = 0,
	CpuType_LonestarDD20,
	CpuType_LonestarDD22,
	CpuType_GekkoDD1or2,
	CpuType_GekkoDD23,
	CpuType_GekkoDD23a,
	CpuType_GekkoDD24,
	CpuType_GekkoDD24e,
	CpuType_GekkoDD40,
	CpuType_BroadwayDD10,
	CpuType_BroadwayDD101,
	CpuType_BroadwayDD12,
	CpuType_BroadwayDD12i,
	CpuType_BroadwayDD13,
	CpuType_BroadwayDD13i,
	CpuType_BroadwayDD20,
	CpuType_UnknownBroadway,
	CpuType_Espresso,
	CpuType_EspressoEs,
	CpuType_UnknownGeneric, // TODO: Can we get some 'maybe's for gekko and espresso
	CpuType_Count
} CpuType;


static const char *cpu_type_str_list[CpuType_Count] = {
	"PowerPC 750L (Lonestar) DD1.0 (or Gekko DD2.0)",
	"PowerPC 750L (Lonestar) DD2.0",
	"PowerPC 750L (Lonestar) DD2.2",
	"Gekko DD1.0 or 2.0",
	"Gekko DD2.3 (or 2.3b/e)",
	"Gekko DD2.3a (or 2.3ei)",
	"Gekko DD2.4",
	"Gekko DD2.4e",
	"Gekko DD4.0",
	"Broadway DD1.0",
	"Broadway DD1.01",
	"Broadway DD1.2",
	"Broadway DD1.2i",
	"Broadway DD1.3",
	"Broadway DD1.3i",
	"Broadway DD2.0 (Broadway-1)",
	"Unknown (PVR 0x08X - maybe Broadway DD%d.%d?)",
	"Espresso",
	"Espresso (engineering sample)",
	"Unknown (PVR 0x%08X)"
};

typedef enum {
	ConsoleType_GameCube = 0,
	ConsoleType_Wii,
	ConsoleType_WiiU,
	ConsoleType_DolphinGC,
	ConsoleType_DolphinWii,
	ConsoleType_Count
} BaseConsoleType;

static const char *base_console_type_str_list[ConsoleType_Count] = {
	"GameCube",
	"Wii",
	"Wii U",
	"Dolphin (GameCube mode)",
	"Dolphin (Wii mode)"
};

typedef enum {
	ChipsetType_FlipperRevA = 0,
	ChipsetType_FlipperRevB,
	ChipsetType_FlipperRevC,
	ChipsetType_FlipperRevT,
	ChipsetType_UnknownFlipper,
	ChipsetType_HollywoodES10,
	ChipsetType_HollywoodES11,
	ChipsetType_HollywoodES12,
	ChipsetType_HollywoodES20,
	ChipsetType_HollywoodES21,
	ChipsetType_HollywoodES30, // bollywood
	ChipsetType_HollywoodES31,
	ChipsetType_UnknownHollywood,
	ChipsetType_LatteA11,
	ChipsetType_LatteA12,
	ChipsetType_LatteA2X,
	ChipsetType_LatteA3X,
	ChipsetType_LatteA4X,
	ChipsetType_LatteA5X,
	ChipsetType_LatteB1X,
	ChipsetType_LatteB1X70,
	ChipsetType_UnknownLatte,
	ChipsetType_UnknownGeneric,
	ChipsetType_Count
} ChipsetType;

// TODO: Maybe-revs for hollywood and latte unknowns?
static const char *chipset_type_str_list[ChipsetType_Count] = {
	"Flipper (rev A) (HW1)",
	"Flipper (rev B) (HW2)",
	"Flipper (rev C) (HW3)",
	"Flipper (rev T)", // TODO: HW4??
	"Flipper (unknown, 0x%08X) (rev %c?)",
	"Hollywood ES1.0",
	"Hollywood ES1.1",
	"Hollywood ES1.21",
	"Hollywood ES2.01",
	"Hollywood ES2.1",
	"Hollywood ES3.0 (Bollywood ES)",
	"Hollywood ES3.1 (Bollywood)",
	"Hollywood (unknown, 0x%08X)",
	"Latte A11",
	"Latte A12",
	"Latte A2X",
	"Latte A3X",
	"Latte A4X",
	"Latte A5X",
	"Latte B1X",
	"Latte B1X (late)",
	"Latte (unknown, 0x%02X)", // TODO: This one is a u8,
				   // so it gets just one byte
				   // output.
				   // Realistically, could the same
				   // be done for other values?
				   // Do we need so many u32s?
	"Unknown" // TODO: What can we show here?
		  // (In other words, is it ok to try to get
		  // the chipset id value on the wrong platform?
		  // Same with other things like gddr value..
		  // Will impact if i need to do all those
		  // conditionals or not)
};

typedef enum {
	DriveType_GCNROM = 0, // TODO: wii u..
	DriveType_GCNR,
	DriveType_NPDP,
	DriveType_DDH,
	DriveType_GDEV,
	DriveType_Triforce,
	DriveType_WiiNROM,
	DriveType_RVTR,
	DriveType_RVTH,
	DriveType_NDEV,
	DriveType_RVA,
	DriveType_Present,
	DriveType_Fatal,
	DriveType_Unknown,
	DriveType_Count
} DriveType;

static const char *drive_type_str_list[DriveType_Count] = {
	"NROM Reader (Retail GameCube)",
	"NR Reader",
	"NPDP Reader",
	"Dolphin Development Hardware",
	"GDEV",
	"Triforce",
	"NROM Reader (Retail Wii)",
	"RVT-R Reader",
	"RVT-H Reader",
	"NDEV",
	"Revolution Arcade",
	"Unknown (device code 0x%04X)", // TODO: verify, until then
					// won't be used
	"Drive is missing or broken!",  // TODO: does this come up
		  		        // on a console with no drive?
	"Unknown (device code 0x%04X)"
};

struct console { // TODO: can we initialize these as consts
	// params (raw data)
	// TODO: can we initialize some of these 
	// with macros and the compiler knows 
	// to just fill it out when accessed??
	bool is_wii;
	bool is_cafe;
	bool is_gc;
	u32 pvr;
	u32 flipper_id;
	u32 hw_chiprevid;
	u8  lt_chiprevid;
	char flipper_rev_letter;

	u32 splash_size;
	u32 gddr_size;
	u32 gddr_rowsel;
	u32 gddr_ranksel;
	u32 gddr_rowmsk;
	
	#ifndef GAMECUBE_BUILD
	DI_DriveID drive_info; // TODO: do something with the 'rev' field?
	#endif

	// indices
	u8 cpu_type_index;
	u8 base_console_type_index;
	u8 chipset_type_index;
	#ifndef GAMECUBE_BUILD
	u8 drive_type_index;
	#endif

	// strings
	char cpu_type_str[64];
	char chipset_type_str[64];
	char ram_info_str[64];

	#ifndef GAMECUBE_BUILD
	char drive_type_str[64];
	char drive_date_str[64];
	#endif

	char full_console_type_str[128];
};

