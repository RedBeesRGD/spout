#include "di.h"
#include <stddef.h>

#include "dvd.h"

//struct _dvddrvinfo drv_info = {0};

DI_DriveID init_dvd_info( void ) {
	DI_Init();
	DI_DriveID id;
	DI_Identify(&id);
	return id;
}
