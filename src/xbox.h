#ifdef XBOX
#include <nxdk/mount.h>
#include <hal/debug.h>
#include <hal/xbox.h>
#include <hal/video.h>

#include <stdio.h>
#include <fileapi.h>
#include <string.h>

#define S_IRUSR 0400  // Read permission, owner
#define S_IWUSR 0200  // Write permission, owner
#define S_IXUSR 0100  // Execute/search permission, owner

#define R_OK 4  // Test for read permission
#define W_OK 2  // Test for write permission
#define X_OK 1  // Test for execute permission

struct stat {
    size_t st_size;   // Total size, in bytes
    int st_mode;      // File mode (permissions)
    int st_mtime;     // Time of last modification
    int st_atime;     // Time of last access
    int st_nlink;     // Number of hard links
};

int access(const char *pathname, int mode);

//Init the target system
void sys_init();

#define printf debugPrint

#define XBOX_DVD_DIR "D:\\"
#define XBOX_HDD_DIR "E:\\Raptor\\"
#define XBOX_TMP_DIR "Z:\\Temp\\"
#endif