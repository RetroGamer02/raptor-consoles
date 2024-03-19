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
#define SDMC "/nds/Raptor/"