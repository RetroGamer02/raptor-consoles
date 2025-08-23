#ifdef __PPC__
#include "ppc.h"
#include <dirent.h>
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

#if defined (__GCN__) || defined (__WII__)
static void *xfb = NULL;
static GXRModeObj *rmode = NULL;
#endif

#ifdef __GCN__
void *GC_Initialise();

void * GC_Initialise() {

	void *framebuffer;

	VIDEO_Init();
	PAD_Init();
	
	rmode = VIDEO_GetPreferredMode(NULL);

	framebuffer = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	console_init(framebuffer,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
	
	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(framebuffer);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();

	return framebuffer;

}
#endif

//Init the target system
void sys_init()
{
#ifdef __GCN__

xfb = GC_Initialise();

if (!fatInitDefault()) {
		printf("fatInitDefault failure: terminating\n");
	}

#elif __WII__

    // Initialise the video system
	VIDEO_Init();

	// This function initialises the attached controllers
	WPAD_Init();

    // Always get buttons, accelerometer & IR by default
    WPAD_SetDataFormat(0, WPAD_FMT_BTNS_ACC_IR);

	// Obtain the preferred video mode from the system
	// This will correspond to the settings in the Wii menu
	rmode = VIDEO_GetPreferredMode(NULL);

	// Allocate memory for the display in the uncached region
	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

	// Initialise the console, required for printf
	console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);

	// Set up the video registers with the chosen mode
	VIDEO_Configure(rmode);

	// Tell the video hardware where our display memory is
	VIDEO_SetNextFramebuffer(xfb);

	// Make the display visible
	VIDEO_SetBlack(false);

	// Flush the video register changes to the hardware
	VIDEO_Flush();

	// Wait for Video setup to complete
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();

	// The console understands VT terminal escape codes
	// This positions the cursor on row 2, column 0
	// we can use variables for this with format codes too
	// e.g. printf ("\x1b[%d;%dH", row, column );
	printf("\x1b[2;0H");

	if (!fatInitDefault()) {
		printf("fatInitDefault failure: terminating\n");
	}

#elif __WIIU__

    // Initialize the screen subsystem and double buffers
    OSScreenInit();

    // Query required buffer sizes for TV (0) and DRC/GamePad (1)
    uint32_t tvSize  = OSScreenGetBufferSizeEx(SCREEN_TV);
    uint32_t drcSize = OSScreenGetBufferSizeEx(SCREEN_DRC);

    // Allocate from default heap (aligned)
    void *tvBuffer  = MEMAllocFromDefaultHeapEx(tvSize,  0x100);
    void *drcBuffer = MEMAllocFromDefaultHeapEx(drcSize, 0x100);

    // Attach the buffers
    OSScreenSetBufferEx(SCREEN_TV,  tvBuffer);
    OSScreenSetBufferEx(SCREEN_DRC, drcBuffer);

    // Clear once before the main loop
    OSScreenClearBufferEx(SCREEN_TV,  0);
    OSScreenClearBufferEx(SCREEN_DRC, 0);
    OSScreenFlipBuffersEx(SCREEN_TV);
    OSScreenFlipBuffersEx(SCREEN_DRC);

    // Setup VPAD
    VPADInit();

    // Foreground lock so HOME/OS interactions behave nicely
    //OSEnableForeground();

   WHBLogUdpInit();

    if (!WHBMountSdCard()) {
        WHBLogPrintf("Failed to mount SD.");
    }
    
#endif
}
#endif