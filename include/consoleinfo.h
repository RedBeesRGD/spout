#define IS_CAFE (*(vu16*)0xcd8005a0 == 0xCAFE)

#define HW_CHIPREVID 	(*(vu32*)0xcd800214)
#define LT_CHIPREVID 	(*(vu8*)0xcd8005a3)

#define PVR_LONESTAR_DD20	0x00080200 // 750L
#define PVR_LONESTAR_DD22	0x00088202

#define PVR_GEKKO_DD10		0x70000100
#define PVR_GEKKO_DD20		0x00080100 // Also Lonestar DD1 apparently
#define PVR_LONESTAR_DD1	PVR_GEKKO_DD20
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

#define PVR_ESPRESSO_ES	0x70010200
#define PVR_ESPRESSO	0x70010201


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

#define DVD_DEVICECODE_ADDR	0x800030E6

// MEI devices, 0x00xx
#define DVD_DEVICECODE_REALDRIVE        0x0000
#define DVD_DEVICECODE_NRDRIVE          0x0001
#define DVD_DEVICECODE_RVLREALDRIVE     0x0002
#define DVD_DEVICECODE_RVLRDRIVE        0x0003

// AMC device, 0x01xx
#define DVD_DEVICECODE_DDH              0x0100

// Hudson devices, 0x02xx
#define DVD_DEVICECODE_NPDP             0x0200
#define DVD_DEVICECODE_GDEV             0x0201
#define DVD_DEVICECODE_NDEV             0x0202
#define DVD_DEVICECODE_RVLH             0x0203

// Arcade devices, 0x03xx
#define DVD_DEVICECODE_RVA              0x0300
    
// Triforce; note this is not what triforce returns to "get device ID"
#define DVD_DEVICECODE_GCAM             0x1000

#define DVD_DEVICECODE_PRESENT          0x8000
#define DVD_DEVICECODE_FATAL            0x0001

typedef enum {
	DvdDevice_GCRetail = 0,
	DvdDevice_GCNR,
	DvdDevice_RVLRetail,
	DvdDevice_RVTR,
	DvdDevice_DDH,
	DvdDevice_NPDP,
	DvdDevice_GDEV,
	DvdDevice_NDEV,
	DvdDevice_RVTH,
	DvdDevice_RVA,
	DvdDevice_Triforce,
	DvdDevice_Fatal,
	DvdDevice_Count
} DvdDevice;

const char *dvd_device_str[DvdDevice_Count] = {
	"Retail (GameCube)",
	"NR Reader",
	"Retail (Wii)",
	"RVT-R Reader",
	"Dolphin Development Hardware",
	"NPDP Reader",
	"GDEV",
	"NDEV",
	"RVT-H Reader",
	"Revolution Arcade",
	"Triforce",
	"Fatal error occured while reading drive code!"
};

typedef enum {
	ConsoleType_GameCube = 0,
	ConsoleType_Wii,
	ConsoleType_WiiU,
	ConsoleType_DolphinGC,
	ConsoleType_DolphinWii,
	ConsoleType_Unknown,
	ConsoleType_Count
} ConsoleType;

const char *console_type_str[ConsoleType_Count] = {
	"Nintendo GameCube",
	"Wii",
	"Wii U",
	"Dolphin (GameCube mode)",
	"Dolphin (Wii mode)",
	"Unknown"
};

typedef enum {
	CpuType_LonestarDD20 = 0,
	CpuType_LonestarDD22,
	CpuType_GekkoDD1or2,
	CpuType_GekkoDD2orLonestar,
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
	CpuType_Espresso,
	CpuType_EspressoEs,
	CpuType_Unknown,
	CpuType_Count
} CpuType;

const char *cpu_type_str[CpuType_Count] = {
	"PowerPC 750L DD2.0 (Lonestar)",
	"PowerPC 750L DD2.2 (Lonestar)",
	"Gekko DD1.0 or 2.0",
	"Gekko DD2.0 (or Lonestar DD1.0)",
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
	"Broadway DD2.0 (Broadway-1)", // Broadway-1
	"Espresso",
	"Espresso (engineering sample)",
	"Unknown"
};

typedef enum {
	ChipsetType_FlipperRevA = 0,
	ChipsetType_FlipperRevB,
	ChipsetType_FlipperRevC,
	ChipsetType_FlipperRevT,
	ChipsetType_HollywoodES10,
	ChipsetType_HollywoodES11,
	ChipsetType_HollywoodES12,
	ChipsetType_HollywoodES20,
	ChipsetType_HollywoodES21,
	ChipsetType_HollywoodES30, // bollywood
	ChipsetType_HollywoodES31,
	ChipsetType_LatteA11,
	ChipsetType_LatteA12,
	ChipsetType_LatteA2X,
	ChipsetType_LatteA3X,
	ChipsetType_LatteA4X,
	ChipsetType_LatteA5X,
	ChipsetType_LatteB1X,
	ChipsetType_LatteB1X70,
	ChipsetType_Unknown,
	ChipsetType_Count
} ChipsetType;

const char *chipset_type_str[ChipsetType_Count] = {
	"Flipper (rev A)",
	"Flipper (rev B)",
	"Flipper (rev C)",
	"Flipper (rev T)",
	"Hollywood ES1.0",
	"Hollywood ES1.1",
	"Hollywood ES1.21",
	"Hollywood ES2.01",
	"Hollywood ES2.1",
	"Hollywood ES3.0 (Bollywood ES)",
	"Hollywood ES3.1 (Bollywood)",
	"Latte A11",
	"Latte A12",
	"Latte A2X",
	"Latte A3X",
	"Latte A4X",
	"Latte A5X",
	"Latte B1X",
	"Latte B1X (late)",
	"Unknown"
};

struct console_info {
	char console_type[64];
	char cpu_type[64];
	char chipset_type[64];
	char ram_info[64];
};
