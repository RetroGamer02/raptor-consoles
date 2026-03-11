#include "../../SDL_internal.h"
#include "SDL_audio.h"
#include "SDL_error.h"
#include "../SDL_audio_c.h"
#include "../SDL_sysaudio.h"

#include <libdragon.h>
#include <string.h>
#include <malloc.h>

/* Define our private device data */
typedef struct
{
    int initialized;
} SDL_PrivateAudioData;

static SDL_AudioDevice *n64_audio_device = NULL;

static int N64AUDIO_OpenDevice(SDL_AudioDevice *this, void *handle, const char *devname, int iscapture)
{
    this->hidden = SDL_calloc(1, sizeof(SDL_PrivateAudioData));
    if (!this->hidden)
        return SDL_OutOfMemory();

    SDL_PrivateAudioData *h = (SDL_PrivateAudioData *)this->hidden;

    /* Force a format libdragon supports */
    this->spec.format = AUDIO_S16SYS;
    this->spec.channels = 1;

    SDL_CalculateAudioSpec(&this->spec);

    audio_init(this->spec.freq, 4);

    h->initialized = 1;
    n64_audio_device = this;

    return 0;
}

static void N64AUDIO_PlayDevice(SDL_AudioDevice *device)
{
    /* Nothing needed */
}

static Uint8* N64AUDIO_GetDeviceBuf(SDL_AudioDevice *device)
{
    return NULL;
}

static void N64AUDIO_WaitDevice(SDL_AudioDevice *device)
{
}

void SDL_N64_PumpAudio(void)
{
    if (!n64_audio_device || !SDL_AtomicGet(&n64_audio_device->enabled))
        return;

    while (audio_can_write())
    {
        short *out = audio_write_begin();
        
        /* audio_get_buffer_length returns stereo samples. Multiply by 4 bytes (2 channels * 16-bit) */
        int bytes = audio_get_buffer_length() << 2; 

        if (n64_audio_device->callbackspec.callback)
        {
            /* ZERO-COPY: Write directly into libdragon's DMA buffer */
            n64_audio_device->callbackspec.callback(
                n64_audio_device->callbackspec.userdata,
                (Uint8 *)out,
                bytes
            );
        }
        else
        {
            SDL_memset(out, 0, bytes);
        }

        audio_write_end();
    }
}

static void N64AUDIO_CloseDevice(SDL_AudioDevice *this)
{
    if (!this->hidden)
        return;

    SDL_PrivateAudioData *h = (SDL_PrivateAudioData *)this->hidden;

    if (h->initialized)
        audio_close();

    SDL_free(h);

    if (n64_audio_device == this)
        n64_audio_device = NULL;
}

static int N64AUDIO_Init(SDL_AudioDriverImpl *impl)
{
    impl->OpenDevice = N64AUDIO_OpenDevice;
    impl->CloseDevice = N64AUDIO_CloseDevice;

    impl->PlayDevice = N64AUDIO_PlayDevice;
    impl->WaitDevice = N64AUDIO_WaitDevice;
    impl->GetDeviceBuf = N64AUDIO_GetDeviceBuf;

    /* SDL will NOT spawn a thread */
    impl->ProvidesOwnCallbackThread = 1;

    impl->OnlyHasDefaultOutputDevice = 1;

    impl->HasCaptureSupport = 0;

    impl->SkipMixerLock = 1; //Is required!

    return 1;
}

AudioBootStrap N64AUDIO_bootstrap = {
    "n64",
    "Libdragon N64 Audio",
    N64AUDIO_Init,
    0
};