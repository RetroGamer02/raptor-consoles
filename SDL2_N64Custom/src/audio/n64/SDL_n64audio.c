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
    Uint8 *mixbuf;
    int mixbuf_size;
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

    audio_init(this->spec.freq, 3); //Was 4 now 6 for PAL Region compat
    
    mixer_init(17); //12 for XM plus 4 for RSP Effects Channels same as default Raptor Setup

    h->mixbuf_size = this->spec.size;
    h->mixbuf = memalign(16, h->mixbuf_size);

    if (!h->mixbuf)
        return SDL_OutOfMemory();

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

    SDL_PrivateAudioData *h = (SDL_PrivateAudioData *)n64_audio_device->hidden;

    while (audio_can_write())
    {
        short *out = audio_write_begin();

        int samples = audio_get_buffer_length();
        int bytes = samples << 2;

        /* CRITICAL: Prevent Buffer Overflow */
        /*if (bytes > h->mixbuf_size) 
        {
            bytes = h->mixbuf_size;
            samples = bytes >> 2; 
        }*/

        /* 1. Mix XM music directly into the hardware buffer */
        mixer_poll(out, samples);

        // 2. Process SDL SFX //
        #ifndef __N64__RSP_ONLY
        if (n64_audio_device->callbackspec.callback)
        {
            // Generate SDL audio into temp buffer //
            n64_audio_device->callbackspec.callback(
                n64_audio_device->callbackspec.userdata,
                h->mixbuf, //
                bytes
            );

            // Tell GCC these memory regions absolutely do not overlap
            int16_t * __restrict dst = (int16_t *)out;
            const int16_t * __restrict src = (int16_t *)h->mixbuf;
            int total_samples = samples << 1; 
            int i = 0;

            // True unrolling: no inner 'j' loop
            for (; i <= total_samples - 4; i += 4) 
            {
                int32_t m0 = dst[i]   + src[i];
                int32_t m1 = dst[i+1] + src[i+1];
                int32_t m2 = dst[i+2] + src[i+2];
                int32_t m3 = dst[i+3] + src[i+3];

                // Branchless clipping
                if ((uint32_t)(m0 + 32768) > 65535) m0 = (m0 < 0) ? -32768 : 32767;
                if ((uint32_t)(m1 + 32768) > 65535) m1 = (m1 < 0) ? -32768 : 32767;
                if ((uint32_t)(m2 + 32768) > 65535) m2 = (m2 < 0) ? -32768 : 32767;
                if ((uint32_t)(m3 + 32768) > 65535) m3 = (m3 < 0) ? -32768 : 32767;

                dst[i]   = (int16_t)m0;
                dst[i+1] = (int16_t)m1;
                dst[i+2] = (int16_t)m2;
                dst[i+3] = (int16_t)m3;
            }

            // Handle tail samples cleanly
            for (; i < total_samples; i++) 
            {
                int32_t mixed = dst[i] + src[i];
                if ((uint32_t)(mixed + 32768) > 65535) 
                {
                    mixed = (mixed < 0) ? -32768 : 32767;
                }
                dst[i] = (int16_t)mixed;
            }
        }
        #endif

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

    impl->ProvidesOwnCallbackThread = 1;
    impl->OnlyHasDefaultOutputDevice = 1;
    impl->HasCaptureSupport = 0;
    impl->SkipMixerLock = 1;

    return 1;
}

AudioBootStrap N64AUDIO_bootstrap = {
    "n64",
    "Libdragon N64 Audio",
    N64AUDIO_Init,
    0
};