#ifdef __3DS__
#include <3ds.h>

#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <cstdio>

int cp(const char *to, const char *from);

bool checkFile(const char* path, int mode);

void init_ctr();

#define access checkFile

#define ROMFS "romfs:/"
#define SDMC "sdmc:/3ds/Raptor/"
#define SDMC2 ""
#endif