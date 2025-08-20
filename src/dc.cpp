#ifdef __DC__
#include "dc.h"

//Replaces access() function for ds.
bool checkFile(const char* path, int mode)
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

void dcprintf(const char* fmt, ...)
{
    //bfont_draw_str(vram_s + 0*SCREEN_WIDTH_DC+0, SCREEN_WIDTH_DC, 1,fmt);
    //sleep(3);
}

void init_dc()
{
    //kos_init_all();
    vid_set_mode(DM_640x480, PM_RGB555);
    if(sd_init()) {
        printf("Could not initialize the SD card. Please make sure that you have an SD card adapter plugged in and an SD card inserted.\n");
    } else {
        printf("SD card initialized successfully.\n");
    }

    //bfont_draw_str(vram_s + 0*SCREEN_WIDTH_DC+0, SCREEN_WIDTH_DC, 1,"Test");

    extern uint8 romdisk[];
    fs_romdisk_mount("/rd", romdisk, 0);
}
#endif