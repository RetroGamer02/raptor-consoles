#ifndef SDL_n64audio_h_
#define SDL_n64audio_h_

#include "../SDL_sysaudio.h"

struct SDL_PrivateAudioData {
    int initialized;
    Uint8 *mixbuf;       /* Main output buffer */
    Uint8 *staging_buf;  /* Decoupling tank */
    Uint16 *work_buffer; /* NEW: Dedicated heap space for the mixer to use instead of the stack */
    int staging_len;
};

#endif