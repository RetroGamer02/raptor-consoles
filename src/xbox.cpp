#ifdef XBOX
#include "xbox.h"

//Replaces access() function for Xbox.
int checkFile(const char* path, int mode)
{
    //Todo add better mode check.
    FILE* f;
    //Mode 1 being write is a guess as no 
    //docs or source was found on how access works.
    if (mode == 1) {
        f = fopen(path, "w");
    } else {
        f = fopen(path, "r");
    }
	
	if (f) {
        fclose(f);
		return false;
	} else {
        return true;
    }
}

void sys_init()
{
    XVideoSetMode(640, 480, 32, REFRESH_DEFAULT); 
    
    // Mount E:
    BOOL ret = nxMountDrive('E', "\\Device\\Harddisk0\\Partition1\\");
    if (!ret) {
        printf("Failed to mount E: drive!\n");
        printf("Loading from D: drive instead.\n");
    } else {
        CreateDirectoryA(XBOX_HDD_DIR, NULL);
    }

    // Mount Z:
    ret = nxMountDrive('Z', "\\Device\\Harddisk0\\Partition5\\");
    if (!ret) {
        printf("Failed to mount Z: drive!\n");
    } else {
        CreateDirectoryA(XBOX_TMP_DIR, NULL);
    }
}

#endif