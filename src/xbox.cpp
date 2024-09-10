#ifdef XBOX
#include "xbox.h"

int stat(const char *path, struct stat *buf) {
    if (!path || !buf) {
        return -1; // Invalid arguments
    }

    // Clear the buffer
    memset(buf, 0, sizeof(struct stat));

    // Use the Windows API to get file attributes
    WIN32_FILE_ATTRIBUTE_DATA fileInfo;
    if (!GetFileAttributesExA(path, GetFileExInfoStandard, &fileInfo)) {
        return -1; // Failed to get file attributes
    }

    // Fill in the stat structure
    buf->st_size = fileInfo.nFileSizeLow; // File size in bytes (assuming file size is less than 4GB)
    buf->st_mode = S_IRUSR | S_IWUSR | S_IXUSR; // File permissions (example)
    buf->st_mtime = fileInfo.ftLastWriteTime.dwLowDateTime; // Last modification time
    buf->st_atime = fileInfo.ftLastAccessTime.dwLowDateTime; // Last access time
    buf->st_nlink = 1; // Number of hard links (example)

    return 0; // Success
}


int access(const char *pathname, int mode) {
    struct stat st;
    if (stat(pathname, &st) != 0) {
        // File does not exist
        return -1;
    }

    // Check for read permission
    if ((mode & R_OK) && !(st.st_mode & S_IRUSR)) {
        return -1;
    }

    // Check for write permission
    if ((mode & W_OK) && !(st.st_mode & S_IWUSR)) {
        return -1;
    }

    // Check for execute permission
    if ((mode & X_OK) && !(st.st_mode & S_IXUSR)) {
        return -1;
    }

    // All requested permissions are granted
    return 0;
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