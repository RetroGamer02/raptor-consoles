#ifdef __MIPS__
#include "mips.h"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <cstdio>

//Generic file copy function.
int cp(const char *to, const char *from)
{
    int fd_to, fd_from;
    char buf[4096];
    ssize_t nread;
    int saved_errno;

    fd_from = open(from, O_RDONLY);
    if (fd_from < 0)
        return -1;

    fd_to = open(to, O_WRONLY | O_CREAT | O_EXCL, 0666);
    if (fd_to < 0)
        goto out_error;

    while (nread = read(fd_from, buf, sizeof buf), nread > 0)
    {
        char *out_ptr = buf;
        ssize_t nwritten;

        do {
            nwritten = write(fd_to, out_ptr, nread);

            if (nwritten >= 0)
            {
                nread -= nwritten;
                out_ptr += nwritten;
            }
            else if (errno != EINTR)
            {
                goto out_error;
            }
        } while (nread > 0);
    }

    if (nread == 0)
    {
        if (close(fd_to) < 0)
        {
            fd_to = -1;
            goto out_error;
        }
        close(fd_from);

        /* Success! */
        return 0;
    }

  out_error:
    saved_errno = errno;

    close(fd_from);
    if (fd_to >= 0)
        close(fd_to);

    errno = saved_errno;
    return -1;
}

//Replaces access() function for PPC Systems.
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

//Init the target system
void sys_init()
{
    #ifdef __N64__
    dfs_init(DFS_DEFAULT_LOCATION);
    debug_init_sdfs("sd:/", 0);

    timer_init();
    joypad_init();
    console_init();

    debug_init_isviewer();
    debug_init_usblog();
    //console_set_render_mode(RENDER_MANUAL);
    //console_set_debug(false);
    #endif
}
#endif