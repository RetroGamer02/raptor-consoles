#include <stdio.h>
#include <string.h>
#ifdef __3DS__
#include "SDL/SDL.h"
#else
#include "SDL.h"
#endif
#include "common.h"
#include "glbapi.h"
#include "i_video.h"
#include "i_oplmusic.h"
#include "musapi.h"
#include "prefapi.h"
#include "cards.h"
#include "fx.h"
#include "dspapi.h"
#include "rap.h"
#include "gssapi.h"
#include "fileids.h"

#ifdef __NDS__
#include <nds.h>
#include <maxmod9.h>
#include "soundbank.h"  // Soundbank definitions
#endif

int music_volume;
int dig_flag;
int fx_device;
int fx_volume;
static int fx_init = 0;
#ifdef __NDS__
int NDS_Mus = -1;
int dsfx_cnt;
int dsfx_playing = 0;
mm_sfxhand dsfx_handle;
mm_sfxhand ds3dfx_handle;
int fx_freq = 6800;
#elif __3DS__
int fx_freq = 22050;
#else
int fx_freq = 44100;
#endif
int music_song = -1;
int fx_gus;
int fx_channels;
int sys_midi, alsaclient, alsaport;

enum {
    FXDEV_NONE = 0,
    FXDEV_PCS,
    FXDEV_GSS1,
    FXDEV_GSS2,
    FXDEV_PCM
};

struct fxitem_t {
    int f_0;
    int f_4;
    int f_8;
    int f_c;
    int f_10;
    int f_14;
    int f_18;
    int f_1c;
};

fxitem_t fx_items[36];
int fx_loaded;

#ifndef SDL12
SDL_AudioDeviceID fx_dev;
#endif

char cards[10][23] = {
    "None",
    "PC Speaker",
    "Adlib",
    "Gravis Ultra Sound",
    "Pro Audio Spectrum",
    "Sound Blaster",
    "WaveBlaster",
    "Roland Sound Canvas",
    "General Midi",
    "Sound Blaster AWE 32",
};

static void FX_Fill(void *userdata, Uint8 *stream, int len)
{
    memset(stream, 0, len);
    int16_t *stream16 = (int16_t*)stream;
    len /= 4;
    MUS_Mix(stream16, len);
    GSS_Mix(stream16, len);
    DSP_Mix(stream16, len);
}

int SND_InitSound(void)
{
    int music_card, fx_card, fx_chans;
    char *genmidi = NULL;
    SDL_AudioSpec spec = {}, actual = {};
    if (fx_init)
        return 0;

    #ifndef __NDS__
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
        return 0;
    #endif

    #ifdef __NDS__
    if (isDSiMode())
    fx_freq = 13230;
    #endif

    spec.freq = fx_freq;
    spec.format = AUDIO_S16SYS;
    spec.channels = 2;
    spec.samples = 512;
    spec.callback = FX_Fill;
    spec.userdata = NULL;

	#ifdef SDL12
    #ifndef __NDS__
	SDL_OpenAudio(&spec, NULL);
    #endif
	#else
    if ((fx_dev = SDL_OpenAudioDevice(NULL, 0, &spec, &actual, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE)) == 0)
    {
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return 0;
    }

    fx_freq = actual.freq;
    if (actual.format != AUDIO_S16SYS || actual.channels != 2)
    {
        SDL_CloseAudio();
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return 0;
    }
    #endif

    #ifdef __NDS__
    mmInitDefault( "soundbank.bin" );
    mmSelectMode(MM_MODE_C);

    mmLoadEffect(SFX_BONUS_FX_076);
    mmLoadEffect(SFX_BOSS_FX_0B7);
    mmLoadEffect(SFX_CRASH_FX_07B);
    mmLoadEffect(SFX_DAVE_FX_0E4);
    mmLoadEffect(SFX_DOOR_FX_080);
    mmLoadEffect(SFX_EGRAB_FX_08A);
    mmLoadEffect(SFX_ESHOT_FX_0C1);
    mmLoadEffect(SFX_EXPLO2_FX_071);
    mmLoadEffect(SFX_EXPLO_FX_06C);
    mmLoadEffect(SFX_FLYBY_FX_085);
    mmLoadEffect(SFX_GEXPLO_FX_08F);
    mmLoadEffect(SFX_GUN_FX_094);
    mmLoadEffect(SFX_HIT_FX_0BC);
    mmLoadEffect(SFX_JETSND_FX_099);
    mmLoadEffect(SFX_LASER_FX_09E);
    mmLoadEffect(SFX_MISSLE_FX_0A3);
    mmLoadEffect(SFX_MON1_FX_0C6);
    mmLoadEffect(SFX_MON2_FX_0CB);
    mmLoadEffect(SFX_MON3_FX_0D0);
    mmLoadEffect(SFX_MON4_FX_0D5);
    mmLoadEffect(SFX_MON5_FX_0DA);
    mmLoadEffect(SFX_MON6_FX_0DF);
    mmLoadEffect(SFX_SWEP_FX_0A8);
    mmLoadEffect(SFX_THEME_FX_067);
    mmLoadEffect(SFX_TURRET_FX_0AD);
    mmLoadEffect(SFX_WARN_FX_0B2);
    #endif

    dig_flag = 0;
    fx_device = FXDEV_NONE;

    music_volume = INI_GetPreferenceLong("Music", "Volume", 127);
    #if defined (__3DS__)
    music_card = CARD_BLASTER;
    #elif defined (__NDS__)
    music_card = CARD_NONE;
    #else
    music_card = INI_GetPreferenceLong("Music", "CardType", CARD_NONE);
    #endif
    sys_midi = INI_GetPreferenceLong("Setup", "sys_midi", 0);
    alsaclient = INI_GetPreferenceLong("Setup", "alsa_output_client", 128);
    alsaport = INI_GetPreferenceLong("Setup", "alsa_output_port", 0);

    switch (music_card)
    {
    case CARD_ADLIB:
    case CARD_MV:
    case CARD_BLASTER:
        genmidi = GLB_GetItem(FILE00e_GENMIDI_OP2);
        if (genmidi)
        {
            LoadInstrumentTable(genmidi);
            GLB_FreeItem(14);
        }
        break;
    }

    
    #ifdef __NDS__
        MUS_SetVolume(music_volume);
        printf("Music Enabled (MM XM MOD)\n");
    #else
        printf("Music Enabled (%s)\n", cards[music_card]);
    #endif
    if (music_card != CARD_NONE)                               
    {
        MUS_Init(music_card, 0);
        MUS_SetVolume(music_volume);
    }

    fx_volume = INI_GetPreferenceLong("SoundFX", "Volume", 127);
    #if defined (__3DS__)
    fx_card = 5;
    fx_chans = 2;
    #elif defined (__NDS__)
    //mmEffectVolume(dsfx_handle, fx_volume * 8 -1);
    //mmEffectVolume(ds3dfx_handle, fx_volume * 8 -1);
    fx_card = 0;
    fx_chans = 2;
    #else
    fx_card = INI_GetPreferenceLong("SoundFX", "CardType", 0);
    fx_chans = INI_GetPreferenceLong("SoundFX", "Channels", 2);
    #endif
    switch (fx_card)
    {
    default:
    case CARD_NONE:
        fx_device = FXDEV_NONE;
        break;
    case CARD_PCS:
        fx_device = FXDEV_PCS;
        break;
    case CARD_ADLIB:
        fx_device = FXDEV_GSS1;
        if (!genmidi)
        {
            genmidi = GLB_GetItem(FILE00e_GENMIDI_OP2);
            if (genmidi)
            {
                LoadInstrumentTable(genmidi);
                GLB_FreeItem(14);
            }
        }
        break;
    case CARD_GUS:
    case CARD_MV:
    case CARD_BLASTER:
        fx_device = FXDEV_PCM;
        dig_flag = 1;
        break;
    case CARD_MPU1:
    case CARD_MPU2:
    case CARD_MPU3:
        fx_device = FXDEV_GSS1;
        break;
    }

    #ifdef __NDS__
        printf("SoundFx Enabled (MM WAVE)\n");
    #else
        printf("SoundFx Enabled (%s)\n", cards[fx_card]);
    #endif

    if (fx_chans < 1 || fx_chans > 8)
        fx_chans = 2;

    if (fx_card == CARD_BLASTER || fx_card == CARD_GUS || fx_card == CARD_MV)
    {
        fx_channels = fx_chans;
        if (fx_card == CARD_GUS && fx_channels < 2)
            fx_gus = 1;
        DSP_Init(fx_channels, 11025);
    }
    else
        fx_channels = 1;

    if (fx_card == CARD_ADLIB || fx_card == CARD_MPU1 || fx_card == CARD_MPU2 || fx_card == CARD_MPU3)
        GSS_Init(fx_card, 0);

	#ifdef SDL12
    SDL_PauseAudio(0);
	#else
    SDL_PauseAudioDevice(fx_dev, 0);
    #endif

    fx_init = 1;
    return 1;
}


void SND_DeInit(void)
{
    if (!fx_init)
        return;

    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    fx_init = 0;
}

void SND_Setup(void)
{
    int i;
    fxitem_t *v1c;
    memset(fx_items, 0, sizeof(fx_items));
    if (fx_device == 0)
    {
        fx_loaded = 0;
        return;
    }
    fx_loaded = 1;

    v1c = &fx_items[1];
    v1c->f_0 = GLB_GetItemID("MON1_FX");
    v1c->f_4 = 1;
    v1c->f_8 = 0x80;
    v1c->f_c = 1;
    v1c->f_10 = -1;
    v1c->f_14 = 0x7f;
    v1c->f_18 = 0;
    v1c->f_1c = 1;

    v1c = &fx_items[2];
    v1c->f_0 = GLB_GetItemID("MON2_FX");
    v1c->f_4 = 1;
    v1c->f_8 = 0x80;
    v1c->f_c = 1;
    v1c->f_10 = -1;
    v1c->f_14 = 0x7f;
    v1c->f_18 = 0;
    v1c->f_1c = 1;

    v1c = &fx_items[3];
    v1c->f_0 = GLB_GetItemID("MON3_FX");
    v1c->f_4 = 1;
    v1c->f_8 = 0x80;
    v1c->f_c = 1;
    v1c->f_10 = -1;
    v1c->f_14 = 0x7f;
    v1c->f_18 = 0;
    v1c->f_1c = 1;

    v1c = &fx_items[4];
    v1c->f_0 = GLB_GetItemID("MON4_FX");
    v1c->f_4 = 1;
    v1c->f_8 = 0x80;
    v1c->f_c = 1;
    v1c->f_10 = -1;
    v1c->f_14 = 0x7f;
    v1c->f_18 = 0;
    v1c->f_1c = 1;

    v1c = &fx_items[5];
    v1c->f_0 = GLB_GetItemID("MON5_FX");
    v1c->f_4 = 1;
    v1c->f_8 = 0x80;
    v1c->f_c = 1;
    v1c->f_10 = -1;
    v1c->f_14 = 0x7f;
    v1c->f_18 = 0;
    v1c->f_1c = 1;

    v1c = &fx_items[6];
    v1c->f_0 = GLB_GetItemID("MON6_FX");
    v1c->f_4 = 1;
    v1c->f_8 = 0x80;
    v1c->f_c = 1;
    v1c->f_10 = -1;
    v1c->f_14 = 0x7f;
    v1c->f_18 = 0;
    v1c->f_1c = 1;

    v1c = &fx_items[7];
    v1c->f_0 = GLB_GetItemID("DAVE_FX");
    v1c->f_4 = 1;
    v1c->f_8 = 0x80;
    v1c->f_c = 0;
    v1c->f_10 = -1;
    v1c->f_14 = 0x7f;
    v1c->f_18 = 0;
    v1c->f_1c = 1;

    v1c = &fx_items[0];
    v1c->f_0 = GLB_GetItemID("THEME_FX");
    v1c->f_4 = 0;
    v1c->f_8 = 0x80;
    v1c->f_c = 0;
    v1c->f_10 = -1;
    v1c->f_14 = 0x7f;
    v1c->f_18 = 0;
    v1c->f_1c = 1;

    v1c = &fx_items[8];
    v1c->f_0 = GLB_GetItemID("EXPLO_FX");
    v1c->f_4 = 1;
    v1c->f_8 = 0x80;
    v1c->f_c = 1;
    v1c->f_10 = -1;
    v1c->f_14 = 0x7f;
    v1c->f_18 = 1;
    v1c->f_1c = 0;

    v1c = &fx_items[9];
    v1c->f_0 = GLB_GetItemID("EXPLO2_FX");
    v1c->f_4 = 1;
    v1c->f_8 = 0x80;
    v1c->f_c = 1;
    v1c->f_10 = -1;
    v1c->f_14 = 0x7f;
    v1c->f_18 = 1;
    v1c->f_1c = 0;

    v1c = &fx_items[10];
    v1c->f_0 = GLB_GetItemID("BONUS_FX");
    v1c->f_4 = 0;
    v1c->f_8 = 0x80;
    v1c->f_c = 0;
    v1c->f_10 = -1;
    v1c->f_14 = 0x7f;
    v1c->f_18 = 1;
    v1c->f_1c = 0;

    v1c = &fx_items[11];
    v1c->f_0 = GLB_GetItemID("CRASH_FX");
    v1c->f_4 = 0;
    v1c->f_8 = 0x80;
    v1c->f_c = 0;
    v1c->f_10 = -1;
    v1c->f_14 = 0x7f;
    v1c->f_18 = 1;
    v1c->f_1c = 0;

    v1c = &fx_items[12];
    v1c->f_0 = GLB_GetItemID("DOOR_FX");
    v1c->f_4 = 0;
    v1c->f_8 = 0x78;
    v1c->f_c = 0;
    v1c->f_10 = -1;
    v1c->f_14 = 0x7f;
    v1c->f_18 = 0;
    v1c->f_1c = 0;

    v1c = &fx_items[13];
    v1c->f_0 = GLB_GetItemID("FLYBY_FX");
    v1c->f_4 = 0;
    v1c->f_8 = 0x78;
    v1c->f_c = 0;
    v1c->f_10 = -1;
    v1c->f_14 = 0x7f;
    v1c->f_18 = 1;
    v1c->f_1c = 0;

    v1c = &fx_items[14];
    v1c->f_0 = GLB_GetItemID("EGRAB_FX");
    v1c->f_4 = 2;
    v1c->f_8 = 0x80;
    v1c->f_c = 0;
    v1c->f_10 = -1;
    v1c->f_14 = 0x28;
    v1c->f_18 = 1;
    v1c->f_1c = 0;

    v1c = &fx_items[15];                                
    v1c->f_0 = GLB_GetItemID("GEXPLO_FX");
    v1c->f_4 = 2;
    v1c->f_8 = 0x80;
    v1c->f_c = 1;
    v1c->f_10 = -1;
    v1c->f_14 = 0x7f;
    v1c->f_18 = 1;
    v1c->f_1c = 0;

    v1c = &fx_items[16];
    v1c->f_0 = GLB_GetItemID("GUN_FX");
    v1c->f_4 = 10;
    v1c->f_8 = 0x7d;
    v1c->f_c = 1;
    v1c->f_10 = -1;
    v1c->f_14 = 0x1e;
    v1c->f_18 = 1;
    v1c->f_1c = 0;

    v1c = &fx_items[17];
    v1c->f_0 = GLB_GetItemID("JETSND_FX");
    v1c->f_4 = 4;
    v1c->f_8 = 0x78;
    v1c->f_c = 0;
    v1c->f_10 = -1;
    v1c->f_14 = 0x78;
    v1c->f_18 = 0;
    v1c->f_1c = 0;

    v1c = &fx_items[18];
    v1c->f_0 = GLB_GetItemID("LASER_FX");
    v1c->f_4 = 2;
    v1c->f_8 = 0x78;
    v1c->f_c = 0;
    v1c->f_10 = -1;
    v1c->f_14 = 0x32;
    v1c->f_18 = 1;
    v1c->f_1c = 0;

    v1c = &fx_items[19];
    v1c->f_0 = GLB_GetItemID("MISSLE_FX");
    v1c->f_4 = 3;
    v1c->f_8 = 0x78;
    v1c->f_c = 1;
    v1c->f_10 = -1;
    v1c->f_14 = 0x32;
    v1c->f_18 = 1;
    v1c->f_1c = 0;

    v1c = &fx_items[20];
    v1c->f_0 = GLB_GetItemID("SWEP_FX");
    v1c->f_4 = 1;
    v1c->f_8 = 0x80;
    v1c->f_c = 0;
    v1c->f_10 = -1;
    v1c->f_14 = 0x7f;
    v1c->f_18 = 1;
    v1c->f_1c = 0;

    v1c = &fx_items[21];
    v1c->f_0 = GLB_GetItemID("TURRET_FX");
    v1c->f_4 = 1;
    v1c->f_8 = 0x80;
    v1c->f_c = 1;
    v1c->f_10 = -1;
    v1c->f_14 = 0x3c;
    v1c->f_18 = 1;
    v1c->f_1c = 0;

    v1c = &fx_items[22];
    v1c->f_0 = GLB_GetItemID("WARN_FX");
    v1c->f_4 = 2;
    v1c->f_8 = 0x80;
    v1c->f_c = 0;
    v1c->f_10 = -1;
    v1c->f_14 = 100;
    v1c->f_18 = 1;
    v1c->f_1c = 0;

    v1c = &fx_items[23];
    v1c->f_0 = GLB_GetItemID("BOSS_FX");
    v1c->f_4 = 1;
    v1c->f_8 = 0x7f;
    v1c->f_c = 0;
    v1c->f_10 = -1;
    v1c->f_14 = 0x7f;
    v1c->f_18 = 1;
    v1c->f_1c = 1;

    v1c = &fx_items[24];
    v1c->f_0 = GLB_GetItemID("JETSND_FX");
    v1c->f_4 = 1;
    v1c->f_8 = 0xeb;
    v1c->f_c = 0;
    v1c->f_10 = -1;
    v1c->f_14 = 0x2d;
    v1c->f_18 = 0;
    v1c->f_1c = 0;

    v1c = &fx_items[25];
    v1c->f_0 = GLB_GetItemID("JETSND_FX");
    v1c->f_4 = 1;
    v1c->f_8 = 0x41;
    v1c->f_c = 0;
    v1c->f_10 = -1;
    v1c->f_14 = 0x78;
    v1c->f_18 = 0;
    v1c->f_1c = 0;

    v1c = &fx_items[26];
    v1c->f_0 = GLB_GetItemID("GUN_FX");
    v1c->f_4 = 1;
    v1c->f_8 = 0xd7;
    v1c->f_c = 1;
    v1c->f_10 = -1;
    v1c->f_14 = 0x7f;
    v1c->f_18 = 0;
    v1c->f_1c = 0;

    v1c = &fx_items[27];
    v1c->f_0 = GLB_GetItemID("GUN_FX");
    v1c->f_4 = 1;
    v1c->f_8 = 0x6e;
    v1c->f_c = 1;
    v1c->f_10 = -1;
    v1c->f_14 = 0x7f;
    v1c->f_18 = 0;
    v1c->f_1c = 0;

    v1c = &fx_items[28];
    v1c->f_0 = GLB_GetItemID("ESHOT_FX");
    v1c->f_4 = 1;
    v1c->f_8 = 100;
    v1c->f_c = 1;
    v1c->f_10 = -1;
    v1c->f_14 = 0x32;
    v1c->f_18 = 1;
    v1c->f_1c = 0;

    v1c = &fx_items[29];
    v1c->f_0 = GLB_GetItemID("LASER_FX");
    v1c->f_4 = 1;
    v1c->f_8 = 0x46;
    v1c->f_c = 1;
    v1c->f_10 = -1;
    v1c->f_14 = 0x78;
    v1c->f_18 = 1;
    v1c->f_1c = 0;

    v1c = &fx_items[30];
    v1c->f_0 = GLB_GetItemID("MISSLE_FX");
    v1c->f_4 = 2;
    v1c->f_8 = 0x8c;
    v1c->f_c = 1;
    v1c->f_10 = -1;
    v1c->f_14 = 0x37;
    v1c->f_18 = 1;
    v1c->f_1c = 0;

    v1c = &fx_items[31];
    v1c->f_0 = GLB_GetItemID("ESHOT_FX");
    v1c->f_4 = 2;
    v1c->f_8 = 0x7f;
    v1c->f_c = 1;
    v1c->f_10 = -1;
    v1c->f_14 = 0x7f;
    v1c->f_18 = 1;
    v1c->f_1c = 0;

    v1c = &fx_items[32];
    v1c->f_0 = GLB_GetItemID("HIT_FX");
    v1c->f_4 = 1;
    v1c->f_8 = 0x84;
    v1c->f_c = 1;
    v1c->f_10 = -1;
    v1c->f_14 = 0x7f;
    v1c->f_18 = 1;
    v1c->f_1c = 0;

    v1c = &fx_items[33];
    v1c->f_0 = GLB_GetItemID("GUN_FX");
    v1c->f_4 = 1;
    v1c->f_8 = 0xd6;
    v1c->f_c = 1;
    v1c->f_10 = -1;
    v1c->f_14 = 100;
    v1c->f_18 = 1;
    v1c->f_1c = 0;

    v1c = &fx_items[34];
    v1c->f_0 = GLB_GetItemID("ESHOT_FX");
    v1c->f_4 = 2;
    v1c->f_8 = 0xfe;
    v1c->f_c = 0;
    v1c->f_10 = -1;
    v1c->f_14 = 0x28;
    v1c->f_18 = 1;
    v1c->f_1c = 0;

    v1c = &fx_items[35];
    v1c->f_0 = GLB_GetItemID("ESHOT_FX");
    v1c->f_4 = 2;
    v1c->f_8 = 100;
    v1c->f_c = 1;
    v1c->f_10 = -1;
    v1c->f_14 = 0x32;
    v1c->f_18 = 1;
    v1c->f_1c = 1;

    for (i = 0; i < 36; i++)
    {
        v1c = &fx_items[i];
        v1c->f_10 = -1;
        if ((unsigned int)v1c->f_0 > 0)
        {
            v1c->f_0 += fx_device;
            GLB_CacheItem(v1c->f_0);
        }
        else
            v1c->f_0 = -1;
    }
}

void SND_FreeFX(void)
{
    int i;
    fxitem_t *v1c;
    SND_StopPatches();
    for (i = 0; i < 36; i++)
    {
        v1c = &fx_items[i];
        if ((unsigned int)v1c->f_0 > 0)
            GLB_FreeItem(v1c->f_0);
    }
}

void SND_CacheFX(void)
{
    int i;
    fxitem_t *v1c;
    for (i = 0; i < 36; i++)
    {
        v1c = &fx_items[i];
        if (v1c->f_0 != -1)
            GLB_CacheItem(v1c->f_0);
    }
}

void SND_CacheGFX(void)
{
    int i;
    fxitem_t *v1c;
    SND_StopPatches();
    for (i = 0; i < 36; i++)
    {
        v1c = &fx_items[i];
        if (v1c->f_0 != -1 && v1c->f_18 != 0)
            GLB_CacheItem(v1c->f_0);
    }
}

void SND_CacheIFX(void)
{
    int i;
    fxitem_t *v1c;
    SND_StopPatches();
    for (i = 0; i < 36; i++)
    {
        v1c = &fx_items[i];
        if (v1c->f_0 != -1 && v1c->f_18 == 0)
            GLB_CacheItem(v1c->f_0);
    }
}

int SFX_Playing(int handle)
{
    switch (handle & FXHAND_TMASK)
    {
    case FXHAND_GSS1:
        return GSS_PatchIsPlaying(handle);
    case FXHAND_DSP:
        return DSP_PatchIsPlaying(handle);
    }
    return 0;
}

int SFX_PlayPatch(char* patch, int pitch, int sep, int vol, int priority)
{
    int type = *(int16_t*)patch;

    switch (type)
    {
    case 0:
        break;
    case 1:
    case 2:
        return GSS_PlayPatch(patch, sep, pitch, vol, priority);
    case 3:
        return DSP_StartPatch((dsp_t*)patch, sep, pitch, vol, priority);
    }
    return -1;
}

void SFX_StopPatch(int handle)
{
    switch (handle & FXHAND_TMASK)
    {
    case FXHAND_GSS1:
        GSS_StopPatch(handle);
        break;
    case FXHAND_DSP:
        DSP_StopPatch(handle);
        break;
    }
}

void SND_Patch(int a1, int a2)
{
    char *v1c;
    int v28, v24, v2c;
    fxitem_t *v18;
    int i;
    if (fx_volume < 1)
        return;
    v28 = 0;
    v24 = 0;
    v18 = fx_items;
    for (i = 0; i < 36; i++, v18++)
    {
        if (v18->f_10 != -1)
        {
            if (!SFX_Playing(v18->f_10) && i != a1)
                SND_StopPatch(i);
            else
                v24++;
        }
    }
    if (v24 <= fx_channels + 2)
    {
        v18 = &fx_items[a1];
        if ((!v18->f_1c || dig_flag) && v18->f_0 != -1)
        {
            if (v18->f_c)
            {
                v28 = wrand() % 40;
                v28 -= 20;
            }
            v1c = GLB_LockItem(v18->f_0);
            v2c = (v18->f_14 * fx_volume) / 127;
            v18->f_10 = SFX_PlayPatch(v1c, v18->f_8 + v28, a2, v2c, v18->f_4);
        }
    }
    #ifdef __NDS__
        //printf("FX: %d\n", a1);
        v18 = &fx_items[a1];
        //dsfx_playing = 1;
        //mm_sfxhand dsfx_handle;
        switch (a1) {
            case 16:
            dsfx_handle = mmEffect(SFX_GUN_FX_094);
            //mmEffectRelease(dsfx_handle);
            break;
            case 8:
            dsfx_handle = mmEffect(SFX_GEXPLO_FX_08F);
            //mmLoadEffect(SFX_HIT_FX_0BC); //Sounds maybe off
            //dsfx_handle = mmEffect(SFX_HIT_FX_0BC);
            //mmEffectRelease(dsfx_handle);
            break;
            case 9:
            dsfx_handle = mmEffect(SFX_EXPLO_FX_06C);
            break;
            case 12:
            dsfx_handle = mmEffect(SFX_DOOR_FX_080);
            break;
            case 18:
            dsfx_handle = mmEffect(SFX_LASER_FX_09E);
            break;
            case 19:
            dsfx_handle = mmEffect(SFX_MISSLE_FX_0A3);
            //mmEffectRelease(dsfx_handle);
            break;
            case 20:
            dsfx_handle = mmEffect(SFX_SWEP_FX_0A8);
            break;
            case 21:
            dsfx_handle = mmEffect(SFX_EXPLO2_FX_071);
            break;
            case 22:
            dsfx_handle = mmEffect(SFX_WARN_FX_0B2);
            break;
            case 24:
            //mmEffectCancel(dsfx_handle);
            //mmLoadEffect(SFX_JETSND_FX_099);
            //dsfx_handle = mmEffect(SFX_JETSND_FX_099);
            //mmEffectRelease(dsfx_handle);
            break;
            case 25:
            dsfx_handle = mmEffect(278);
            break;
            case 26:
            dsfx_handle = mmEffect(SFX_GUN_FX_094);
            break;
            case 27:
            dsfx_handle = mmEffect(SFX_GUN_FX_094);
            break;
            case 34:
            dsfx_handle = mmEffect(SFX_LASER_FX_09E);
            break;
            case 15:
            dsfx_handle = mmEffect(SFX_GEXPLO_FX_08F);
            break;
            case 17:
            //mmEffectCancel(dsfx_handle);
            dsfx_handle = mmEffect(SFX_JETSND_FX_099);
            //mmEffectRelease(dsfx_handle);
            break;
            case 10:
            //mmEffectVolume(279 ,(v18->f_14 * fx_volume) / 127);
            dsfx_handle = mmEffect(SFX_BONUS_FX_076);
            break;
            case 11:
            //mmEffectVolume(279 ,(v18->f_14 * fx_volume) / 127);
            dsfx_handle = mmEffect(SFX_EXPLO_FX_06C);
            break;
            case 13:
            //mmEffectVolume(279 ,(v18->f_14 * fx_volume) / 127);
            dsfx_handle = mmEffect(SFX_FLYBY_FX_085);
            break;
            case 23:
            //mmEffectVolume(271 ,(v18->f_14 * fx_volume) / 127);
            dsfx_handle = mmEffect(SFX_BOSS_FX_0B7);
            break;
            case 33:
            dsfx_handle = mmEffect(SFX_GEXPLO_FX_08F);
            break;
            default:
            printf("FX id: %d\n", a1);
            break;
        }
        mmEffectRate(dsfx_handle, 512 * 2^(v18->f_8 + v28)/12); //Sounds right
        mmEffectPanning(dsfx_handle, a2);
        //printf("FX Vol: %d\n", (v18->f_14 * fx_volume) / 127);
        //mmEffectVolume(dsfx_handle, (a2 * fx_volume) / 127);
        mmEffectVolume(dsfx_handle, fx_volume * 2 -1);
        //printf("Priority: %d\n", v18->f_4);
        if(v18->f_4 <= 0) {
            mmEffectRelease(dsfx_handle);
        }
    #endif
}

void SND_3DPatch(int a1, int a2, int a3)
{
    int v24;
    int v20, v28;
    int i;
    int v30, v34, v38, v3c, v2c, v4c;
    char *v18;
    fxitem_t *v14;
    if (fx_volume < 1)
        return;

    v24 = 0;
    v20 = 0;
    v14 = fx_items;
    for (i = 0; i < 36; i++, v14++)
    {
        if (v14->f_10 != -1)
        {
            if (!SFX_Playing(v14->f_10))
                SND_StopPatch(i);
            else
                v20++;
        }
    }
    if (v20 <= fx_channels + 2)
    {
        v30 = a2 - player_cx;
        v34 = a3 - player_cy;
        v28 = v30 + 127;
        if (v28 < 1)
            v28 = 1;
        else if (v28 > 255)
            v28 = 255;
        v30 = abs(v30);
        v34 = abs(v34);
        if (v30 < v34)
            v4c = v30;
        else
            v4c = v34;
        v38 = v30 + v34 - (v4c / 2);
        if (v38 < 40)
            v2c = 127;
        else if (v38 > 500)
            v2c = 1;
        else
            v2c = 127 - ((v38 - 40) * 127) / (500 - 40);
        v14 = &fx_items[a1];
        if (!v14->f_1c || dig_flag)
        {
            if (v14->f_c)
            {
                v24 = wrand() % 40;
                v24 -= 20;
            }
            v3c = (v2c * fx_volume) / 127;
            v3c = (v3c * v14->f_14) / 127;
            v18 = GLB_LockItem(v14->f_0);
            v14->f_10 = SFX_PlayPatch(v18, v14->f_8 + v24, v28, v3c, v14->f_4);
        }
    }
    #ifdef __NDS__
        
        v14 = &fx_items[a1];
        //dsfx_playing = 1;
        
        switch (a1) {
            case 8:
            ds3dfx_handle = mmEffect(SFX_GEXPLO_FX_08F);
            //mmEffectRelease(ds3dfx_handle);
            break;
            case 15:
            ds3dfx_handle = mmEffect(SFX_EXPLO2_FX_071);
            //mmEffectRelease(ds3dfx_handle);
            break;
            case 28:
            ds3dfx_handle = mmEffect(SFX_ESHOT_FX_0C1);
            //mmEffectRelease(ds3dfx_handle);
            break;
            case 30:
            ds3dfx_handle = mmEffect(SFX_MISSLE_FX_0A3);
            //mmEffectRelease(ds3dfx_handle);
            break;
            default:
            printf("3DFX id: %d\n", a1);
            break;
        }
        mmEffectRate(ds3dfx_handle, 512 * 2^(v14->f_8 + v24)/12); //Sounds right*/
        //mmEffectPanning(ds3dfx_handle, a2);
        mmEffectVolume(ds3dfx_handle, fx_volume * 2 -1);
        //printf("Priority: %d\n", v14->f_4);

        if(v14->f_4 <= 0) {
            mmEffectRelease(ds3dfx_handle);
        }
    #endif
}

int SND_IsPatchPlaying(int a1)
{
    fxitem_t *v1c;
    v1c = &fx_items[a1];
    #ifdef __NDS__
    if (v1c->f_10 != -1)
        return 1;
    #else
    if (v1c->f_10 != -1 && SFX_Playing(v1c->f_10))
        return 1;
    #endif
    return 0;
}

void SND_StopPatch(int a1)
{
    fxitem_t *v1c;
    v1c = &fx_items[a1];
    if (v1c->f_10 != -1)
    {
        SFX_StopPatch(v1c->f_10);
        #ifdef __NDS__
        //mmEffectCancel(dsfx_handle);
        #endif
        GLB_UnlockItem(v1c->f_0);
        v1c->f_10 = -1;
    }
}

void SND_StopPatches(void)
{
    int i;
    fxitem_t *v1c;
    v1c = fx_items;
    for (i = 0; i < 36; i++, v1c++)
    {
        if (v1c->f_10 != -1)
            SFX_StopPatch(v1c->f_10);
    }
    v1c = fx_items;
    for (i = 0; i < 36; i++, v1c++)
    {
        if (v1c->f_10 != -1)
        {
            GLB_UnlockItem(v1c->f_0);
            v1c->f_10 = -1;
        }
    }
}

void SND_PlaySong(int musres, int loop, int fade)
{
    char *ptr;
    if (music_volume <= 1)
        return;
    if (music_song == musres)
        return;
    if (music_song != -1)
    {
        MUS_StopSong(fade);
        if (fade)
        {
            #ifdef __NDS__
                mmStop();
            #endif
            while (MUS_SongPlaying())
            {
                I_GetEvent();
            }
        }
        GLB_UnlockItem(music_song);
        music_song = -1;
    }
    if (musres != -1)
    {
        music_song = musres;
        ptr = GLB_LockItem(musres);
        MUS_PlaySong(ptr, loop, fade);

        #ifdef __NDS__
        if (NDS_Mus != -1)
        mmUnload(NDS_Mus);
        printf("Current Song: %d\n", music_song);
        switch (music_song)
        {
            case 86:
                NDS_Mus = 14; //Was 15
                break;
            case 87:
                NDS_Mus = 6;
                break;
            case 88:
                NDS_Mus = 5;
                break;
            case 89:
                NDS_Mus = 7;
                break;
            case 90:
                NDS_Mus = 8;
                break;
            case 91:
                NDS_Mus = 9;
                break;
            case 92:
                NDS_Mus = 10;
                break;
            case 93:
                NDS_Mus = 11;
                break;
            case 94:
                NDS_Mus = 12;
                break;
            case 95:
                NDS_Mus = 13; //?
                break;
            case 96:
                NDS_Mus = 15; //Was 14
                break;
            case 97:
                NDS_Mus = 0;
                break;
            default:
                NDS_Mus = -1;
                break;
        }
        
        mmLoad(NDS_Mus);
        if (loop)
        {
            mmStart(NDS_Mus, MM_PLAY_LOOP );
            //mmResume();
        } else {
            mmStart(NDS_Mus, MM_PLAY_ONCE );
            //mmResume();
        }
        #endif
    }
}

int SND_SongPlaying(void) {
    return MUS_SongPlaying();
}

void SND_FadeOutSong(void)
{
    if (music_song != -1)
    {
        if (MUS_SongPlaying())
            MUS_StopSong(1);
        while (MUS_SongPlaying())
        {
            I_GetEvent();
        }
        GLB_UnlockItem(music_song);
    }
    music_song = -1;
}

static int lockcount;

void SND_Lock(void)
{
    if (!lockcount)
    #ifdef SDL12
        SDL_LockAudio();
    #else
        SDL_LockAudioDevice(fx_dev);
    #endif
    lockcount++;
}

void SND_Unlock(void)
{
    lockcount--;
    if (!lockcount)
    #ifdef SDL12
        SDL_UnlockAudio();
    #else
        SDL_UnlockAudioDevice(fx_dev);
    #endif
}
