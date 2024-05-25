#ifdef __NDS__
#include <nds.h>

#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <fat.h>
#include <filesystem.h>

#include <cstdio>

int cp(const char *to, const char *from);

bool checkFile(const char* path, int mode);

void init_nds();

#define access checkFile

#define ROMFS "nitro:/"
#ifdef __BLOCKSDS__
#define SDMC "fat:/nds/Raptor/"
#define SDMC2 "sd:/nds/Raptor/"
#else
#define SDMC "/nds/Raptor/"
#define SDMC2 ""
#endif
#endif