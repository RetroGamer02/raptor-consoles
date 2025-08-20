#ifdef __DC__
#include <kos.h>
#include <time.h>
#include <dcplib/fnt.h>
#include <dc/sd.h>
#include <kos/blockdev.h>
#include <dc/maple.h>
#include <dc/maple/vmu.h>

#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <cstdio>

#define SCREEN_WIDTH_DC 640
#define SCREEN_HEIGHT_DC 480

//int cp(const char *to, const char *from);

bool checkFile(const char* path, int mode);
void dcprintf(const char* fmt, ...);

void init_dc();

#define access checkFile
#define printf dcprintf

#define ROMFS "/rd/"
#define SDMC "/vmu/a1/"
#define SDMC2 "/sd/dc/Raptor/"
#endif