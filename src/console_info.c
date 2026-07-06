#include "console_info.h"

const char *cpu_type_str_list[CpuType_Count] = {
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

const char *base_console_type_str_list[ConsoleType_Count] = {
	"GameCube",
	"Wii",
	"Wii U",
	"Dolphin (GameCube mode)",
	"Dolphin (Wii mode)"
};

// TODO: Maybe-revs for hollywood and latte unknowns?
const char *chipset_type_str_list[ChipsetType_Count] = {
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

const char *drive_type_str_list[DriveType_Count] = {
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
