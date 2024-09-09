#ifdef XBOX
#include <nxdk/mount.h>
#include <hal/debug.h>
#include <hal/xbox.h>
#include <hal/video.h>

#include <stdio.h>
#include <fileapi.h>

//Replaces access() function for Xbox.
int checkFile(const char* path, int mode);

//Init the target system
void sys_init();

#define printf debugPrint
#define access checkFile

#define XBOX_DVD_DIR "D:\\"
#define XBOX_HDD_DIR "E:\\Raptor\\"
#define XBOX_TMP_DIR "Z:\\Temp\\"
#endif