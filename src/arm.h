#ifdef __ARM__
#ifdef __NDS__
#include <SDL.h>
#include <nds.h>
#include <fat.h>
#include <filesystem.h>
#elif __3DS__
#include <SDL2/SDL.h>
#include <3ds.h>
#elif __SWITCH__
#include <SDL2/SDL.h>
#include <switch.h>
#endif

//Generic file copy function.
int cp(const char *to, const char *from);

//Replaces access() function for ARM Systems.
#ifdef __NDS__
int checkFile(const char* path, int mode);
#endif

//Init the target system
void sys_init();

#define access checkFile

#ifdef __NDS__
#define ROMFS "nitro:/"
#define SDMC "/"
#define RAP_SD_DIR SDMC "nds/Raptor/"
#elif __3DS__
#define ROMFS "romfs:/"
#define SDMC "sdmc:/"
#define RAP_SD_DIR SDMC "3ds/Raptor/"
#elif __SWITCH__
#define ROMFS "romfs:/"
#define SDMC "sdmc:/"
#define RAP_SD_DIR SDMC "switch/Raptor/"
#endif
#endif