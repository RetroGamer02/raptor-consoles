/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2022 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include "../../SDL_internal.h"

#if SDL_VIDEO_RENDER_N64

#include "SDL_hints.h"
#include "../SDL_sysrender.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#define stack_t libdragon_stack_t
#include <libdragon.h>
#undef stack_t
#include <malloc.h>

/* N64 renderer implementation, based on the PGE  */

#define N64_SCREEN_WIDTH    320
#define N64_SCREEN_HEIGHT   240

#define N64_FRAME_BUFFER_WIDTH  N64_SCREEN_WIDTH * 16
#define N64_FRAME_BUFFER_SIZE   (N64_FRAME_BUFFER_WIDTH * N64_SCREEN_HEIGHT)

extern SDL_RenderDriver N64_RenderDriver;

/**
 * Holds n64 specific texture data
 *
 * Part of a hot-list of textures that are used as render targets
 * When short of vram we spill Least-Recently-Used render targets to system memory
 */
typedef struct N64_TextureData
{
    void                *data;                              /**< Image data. */
    unsigned int        size;                               /**< Size of data in bytes. */
    unsigned int        width;                              /**< Image width. */
    unsigned int        height;                             /**< Image height. */
    unsigned int        textureWidth;                       /**< Texture width (power of two). */
    unsigned int        textureHeight;                      /**< Texture height (power of two). */
    unsigned int        bits;                               /**< Image bits per pixel. */
    unsigned int        format;                             /**< Image format - one of ::pgePixelFormat. */
    unsigned int        pitch;
    sprite_t            *sprite;
} N64_TextureData;

typedef struct N64_BlendState
{
    SDL_BlendMode mode;
    unsigned int color;
    int shadeModel;
    SDL_Texture* texture;
} N64_BlendState;

typedef struct N64_RenderData
{
    SDL_Texture *boundTarget;
    SDL_bool drawing;

    int screen_w;
    int screen_h;

    SDL_Rect viewport;

    sprite_t *last_sprite;
    int last_tlut;
    int last_mode;
    SDL_BlendMode last_blend;

} N64_RenderData;


typedef struct VertV
{
    float   x, y, z;
} VertV;


typedef struct VertTV
{
    float   u, v;
    float   x, y, z;
} VertTV;

typedef struct VertCV
{
    SDL_Color col;
    float     x, y, z;
} VertCV;


typedef struct VertTCV
{
    float     u, v;
    SDL_Color col;
    float     x, y, z;
} VertTCV;

#define PI 3.14159265358979f

#define radToDeg(x) ((x)*180.f/PI)
#define degToRad(x) ((x)*PI/180.f)

/* Helper to avoid reloading TLUT or changing RDP mode unnecessarily */
static inline void
EnsureTextureState(N64_RenderData *data, N64_TextureData *n64_tex)
{
    if (!data || !n64_tex) return;

    sprite_t *sp = n64_tex->sprite;

    if (data->last_sprite != sp) {
        //data_cache_hit_writeback(n64_tex->data, n64_tex->size);

        /* Sprite changed; update TLUT state as required */
        if (sp->format == FMT_CI8) {
            /* palette located at end of pixel data */
            void *palette = (Uint8*)n64_tex->data + n64_tex->size;
            if (data->last_tlut != 1) {
                rdpq_tex_load_tlut(palette, 0, 256);
                rdpq_mode_tlut(TLUT_RGBA16);
                data->last_tlut = 1;
            }
        } else {
            if (data->last_tlut != 0) {
                rdpq_mode_tlut(TLUT_NONE);
                data->last_tlut = 0;
            }
        }
        data->last_sprite = sp;
    }
}

 void
Swap(float *a, float *b)
{
    float n=*a;
    *a = *b;
    *b = n;
}

/* Return next power of 2 */
static int
TextureNextPow2(unsigned int w)
{
    unsigned int n = 2;
    if(w == 0)
        return 0;

    while(w > n)
        n <<= 1;

    return n;
}

static int
PixelFormatToN64FMT(Uint32 format)
{
    return FMT_RGBA16;
}

static inline void N64_GetDisplaySize(int *w, int *h)
{
    *w = display_get_width();
    *h = display_get_height();
}

static inline uint16_t rgba8888_to_rgba5551(uint32_t c)
{
    uint8_t r = (c >> 16) & 0xFF;
    uint8_t g = (c >> 8)  & 0xFF;
    uint8_t b = (c >> 0)  & 0xFF;
    uint8_t a = (c >> 24) & 0xFF;

    return ((r >> 3) << 11) |
           ((g >> 3) << 6)  |
           ((b >> 3) << 1)  |
           (a > 127);
}

 int
TextureSwizzle(N64_TextureData *n64_texture, void* dst)
{
    // int bytewidth, height;
    // int rowblocks, rowblocksadd;
    // int i, j;
    // unsigned int blockaddress = 0;
    // unsigned int *src = NULL;
    // unsigned char *data = NULL;

    // if(n64_texture->swizzled)
    //     return 1;

    // bytewidth = n64_texture->textureWidth*(n64_texture->bits>>3);
    // height = n64_texture->size / bytewidth;

    // rowblocks = (bytewidth>>4);
    // rowblocksadd = (rowblocks-1)<<7;

    // src = (unsigned int*) n64_texture->data;

    // data = dst;
    // if(!data) {
    //     data = SDL_malloc(n64_texture->size);
    // }

    // if(!data) {
    //     return SDL_OutOfMemory();
    // }

    // for(j = 0; j < height; j++, blockaddress += 16)
    // {
    //     unsigned int *block;

    //     block = (unsigned int*)&data[blockaddress];

    //     for(i = 0; i < rowblocks; i++)
    //     {
    //         *block++ = *src++;
    //         *block++ = *src++;
    //         *block++ = *src++;
    //         *block++ = *src++;
    //         block += 28;
    //     }

    //     if((j & 0x7) == 0x7)
    //         blockaddress += rowblocksadd;
    // }

    // // TextureStorageFree(n64_texture->data);
    // n64_texture->data = data;
    // n64_texture->swizzled = SDL_TRUE;

    // sceKernelDcacheWritebackRange(n64_texture->data, n64_texture->size);
    return 1;
}

 int
TextureUnswizzle(N64_TextureData *n64_texture, void* dst)
{
    // int bytewidth, height;
    // int widthblocks, heightblocks;
    // int dstpitch, dstrow;
    // int blockx, blocky;
    // int j;
    // unsigned int *src = NULL;
    // unsigned char *data = NULL;
    // unsigned char *ydst = NULL;

    // if(!n64_texture->swizzled)
    //     return 1;

    // bytewidth = n64_texture->textureWidth*(n64_texture->bits>>3);
    // height = n64_texture->size / bytewidth;

    // widthblocks = bytewidth/16;
    // heightblocks = height/8;

    // dstpitch = (bytewidth - 16)/4;
    // dstrow = bytewidth * 8;

    // src = (unsigned int*) n64_texture->data;

    // data = dst;

    // if(!data) {
    //     data = SDL_malloc(n64_texture->size);
    // }

    // if(!data)
    //     return SDL_OutOfMemory();

    // ydst = (unsigned char *)data;

    // for(blocky = 0; blocky < heightblocks; ++blocky)
    // {
    //     unsigned char *xdst = ydst;

    //     for(blockx = 0; blockx < widthblocks; ++blockx)
    //     {
    //         unsigned int *block;

    //         block = (unsigned int*)xdst;

    //         for(j = 0; j < 8; ++j)
    //         {
    //             *(block++) = *(src++);
    //             *(block++) = *(src++);
    //             *(block++) = *(src++);
    //             *(block++) = *(src++);
    //             block += dstpitch;
    //         }

    //         xdst += 16;
    //     }

    //     ydst += dstrow;
    // }

    // TextureStorageFree(n64_texture->data);

    // n64_texture->data = data;

    // n64_texture->swizzled = SDL_FALSE;

    // sceKernelDcacheWritebackRange(n64_texture->data, n64_texture->size);
    return 1;
}

 int
TextureBindAsTarget(N64_RenderData* data, N64_TextureData* n64_texture)
{
    return 0;
}

static void
N64_WindowEvent(SDL_Renderer *renderer, const SDL_WindowEvent *event)
{
    //fprintf(stderr, "N64_WindowEvent: %u\n", event->event);
}


static int
N64_CreateTexture(SDL_Renderer *renderer, SDL_Texture *texture)
{
    N64_TextureData *n64_texture = (N64_TextureData *)SDL_calloc(1, sizeof(*n64_texture));
    if(!n64_texture) return SDL_OutOfMemory();

    n64_texture->width = texture->w;
    n64_texture->height = texture->h;
    n64_texture->format = PixelFormatToN64FMT(texture->format);
    n64_texture->bits = SDL_BYTESPERPIXEL(texture->format) * 8;

    // N64 works best when the stride (pitch) matches the texture width
    n64_texture->pitch = n64_texture->width * SDL_BYTESPERPIXEL(texture->format);
    n64_texture->size  = n64_texture->height * n64_texture->pitch;

    // If CI8, we need 512 extra bytes for the 256-color palette (RGBA16 entries)
    size_t extra_size = (n64_texture->format == FMT_CI8) ? 512 : 0;
    
    // Replace the SDL_calloc line for the sprite with this:
    size_t total_sprite_size = sizeof(sprite_t) + n64_texture->size + extra_size;
    sprite_t *sprite = memalign(64, total_sprite_size);
    memset(sprite, 0, total_sprite_size); // Clear it like calloc would
    sprite->width = n64_texture->width;
    sprite->height = n64_texture->height;
    sprite->format = n64_texture->format;
    sprite->bitdepth = n64_texture->bits;
    
    // Slicing: N64 TMEM is strictly 4KB. Max 2048 16-bit pixels per slice.
    // Divide into 64x32 chunks to perfectly fit without overflow.
    sprite->hslices = (sprite->width + 63) / 64; 
    sprite->vslices = (sprite->height + 31) / 32;

    n64_texture->sprite = sprite;
    n64_texture->data = sprite->data;
    texture->driverdata = n64_texture;
    return 0;
}

 int
TextureShouldSwizzle(N64_TextureData* n64_texture, SDL_Texture *texture)
{
    return -1;
    // return !((texture->access == SDL_TEXTUREACCESS_TARGET) && InVram(n64_texture->data))
    //          && (texture->w >= 16 || texture->h >= 16);
}

 void
TextureActivate(SDL_Texture * texture)
{
    // N64_TextureData *n64_texture = (N64_TextureData *) texture->driverdata;
    // int scaleMode = (texture->scaleMode == SDL_ScaleModeNearest) ? GU_NEAREST : GU_LINEAR;

    // /* Swizzling is useless with small textures. */
    // if (TextureShouldSwizzle(n64_texture, texture))
    // {
    //     TextureSwizzle(n64_texture, NULL);
    // }

    // sceGuTexWrap(GU_REPEAT, GU_REPEAT);
    // sceGuTexMode(n64_texture->format, 0, 0, n64_texture->swizzled);
    // sceGuTexFilter(scaleMode, scaleMode); /* GU_NEAREST good for tile-map */
    //                                       /* GU_LINEAR good for scaling */
    // sceGuTexImage(0, n64_texture->textureWidth, n64_texture->textureHeight, n64_texture->textureWidth, n64_texture->data);
}

static int
N64_LockTexture(SDL_Renderer *renderer, SDL_Texture *texture,
                 const SDL_Rect *rect, void **pixels, int *pitch);

static int
N64_UpdateTexture(SDL_Renderer *renderer, SDL_Texture *texture,
                  const SDL_Rect *rect, const void *pixels, int pitch)
{
    N64_TextureData *n64_texture = texture->driverdata;
    Uint8 *dst = (Uint8 *)n64_texture->data + (rect->y * n64_texture->pitch + rect->x * SDL_BYTESPERPIXEL(texture->format));
    const Uint8 *src = (const Uint8 *)pixels;
    int bpp = SDL_BYTESPERPIXEL(texture->format);
    int row_bytes = rect->w * bpp;

    /* If source rows are contiguous and match row_bytes, copy in one shot */
    if (pitch == row_bytes) {
        memcpy(dst, src, (size_t)row_bytes * rect->h);
    } else {
        /* fallback: copy row-by-row (still uses memcpy) */
        for (int y = 0; y < rect->h; y++) {
            memcpy(dst + y * n64_texture->pitch,
                   src + y * pitch,
                   row_bytes);
        }
    }
    size_t flush_offset = rect->y * n64_texture->pitch + rect->x * bpp;
    size_t flush_size   = rect->h * row_bytes;

    data_cache_hit_writeback(
        (uint8_t*)n64_texture->data + flush_offset,
        flush_size
    );
    return 0;
}

static int
N64_LockTexture(SDL_Renderer *renderer, SDL_Texture *texture,
                 const SDL_Rect *rect, void **pixels, int *pitch)
{
    // fprintf(stderr, "N64_LockTexture\n");
    N64_TextureData *n64_texture = (N64_TextureData *)texture->driverdata;

    *pixels = (void *)((Uint8 *)n64_texture->data + rect->y * n64_texture->pitch +
                rect->x * SDL_BYTESPERPIXEL(texture->format));
    *pitch = n64_texture->pitch;

    return 0;
}

static void
N64_UnlockTexture(SDL_Renderer *renderer, SDL_Texture *texture)
{
    N64_TextureData *n64_texture = (N64_TextureData *)texture->driverdata;
    
    // FIX 3: Flush the data cache for the texture region
    if (n64_texture && n64_texture->data) {
        //data_cache_hit_writeback(n64_texture->data, n64_texture->size);
        data_cache_hit_writeback(n64_texture->data, n64_texture->pitch * n64_texture->height);
    }
}

static void
N64_SetTextureScaleMode(SDL_Renderer *renderer, SDL_Texture *texture, SDL_ScaleMode scaleMode)
{
    //fprintf(stderr, "N64_SetTextureScaleMode\n");
    /* Nothing to do because TextureActivate takes care of it */
}

static int
N64_SetRenderTarget(SDL_Renderer *renderer, SDL_Texture *texture)
{
    //fprintf(stderr, "N64_SetRenderTarget\n");

    return 0;
}

static int
N64_QueueSetViewport(SDL_Renderer *renderer, SDL_RenderCommand *cmd)
{
    // fprintf(stderr, "N64_QueueSetViewport\n");
    N64_RenderData *data = renderer->driverdata;
    data->viewport = cmd->data.viewport.rect;
    return 0;
}

static int
N64_QueueDrawPoints(SDL_Renderer *renderer, SDL_RenderCommand *cmd, const SDL_FPoint *points, int count)
{
    //fprintf(stderr, "N64_QueueDrawPoints\n");
    VertV *verts = (VertV *) SDL_AllocateRenderVertices(renderer, count * sizeof (VertV), 4, &cmd->data.draw.first);
    int i;

    if (!verts) {
        return -1;
    }

    cmd->data.draw.count = count;

    for (i = 0; i < count; i++, verts++, points++) {
        verts->x = points->x;
        verts->y = points->y;
        verts->z = 0.0f;
    }

    return 0;
}

static int
N64_QueueGeometry(SDL_Renderer *renderer, SDL_RenderCommand *cmd, SDL_Texture *texture,
        const float *xy, int xy_stride, const SDL_Color *color, int color_stride, const float *uv, int uv_stride,
        int num_vertices, const void *indices, int num_indices, int size_indices,
        float scale_x, float scale_y)
{
    //fprintf(stderr, "N64_QueueGeometry\n");
    int i;
    int count = indices ? num_indices : num_vertices;

    cmd->data.draw.count = count;
    size_indices = indices ? size_indices : 0;

    if (texture == NULL) {
        VertCV *verts;
        verts = (VertCV *)SDL_AllocateRenderVertices(renderer, count * sizeof(VertCV), 4, &cmd->data.draw.first);
        if (!verts) {
            return -1;
        }

        for (i = 0; i < count; i++) {
            int j;
            float *xy_;
            SDL_Color col_;
            if (size_indices == 4) {
                j = ((const Uint32 *)indices)[i];
            } else if (size_indices == 2) {
                j = ((const Uint16 *)indices)[i];
            } else if (size_indices == 1) {
                j = ((const Uint8 *)indices)[i];
            } else {
                j = i;
            }

            xy_ = (float *)((char*)xy + j * xy_stride);
            col_ = *(SDL_Color *)((char*)color + j * color_stride);

            verts->x = xy_[0] * scale_x;
            verts->y = xy_[1] * scale_y;
            verts->z = 0;

            verts->col = col_;

            verts++;
        }
    } else {
        N64_TextureData *n64_texture = (N64_TextureData *)texture->driverdata;
        VertTCV *verts;
        verts = (VertTCV *) SDL_AllocateRenderVertices(renderer, count * sizeof (VertTCV), 4, &cmd->data.draw.first);
        if (!verts) {
            return -1;
        }

        for (i = 0; i < count; i++) {
            int j;
            float *xy_;
            SDL_Color col_;
            float *uv_;

            if (size_indices == 4) {
                j = ((const Uint32 *)indices)[i];
            } else if (size_indices == 2) {
                j = ((const Uint16 *)indices)[i];
            } else if (size_indices == 1) {
                j = ((const Uint8 *)indices)[i];
            } else {
                j = i;
            }

            xy_ = (float *)((char*)xy + j * xy_stride);
            col_ = *(SDL_Color *)((char*)color + j * color_stride);
            uv_ = (float *)((char*)uv + j * uv_stride);

            verts->x = xy_[0] * scale_x;
            verts->y = xy_[1] * scale_y;
            verts->z = 0;

            verts->col = col_;

            verts->u = uv_[0] * n64_texture->textureWidth;
            verts->v = uv_[1] * n64_texture->textureHeight;

            verts++;
        }
    }

    return 0;
}

static int
N64_QueueFillRects(SDL_Renderer *renderer, SDL_RenderCommand *cmd, const SDL_FRect *rects, int count)
{
    //fprintf(stderr, "N64_QueueFillRects\n");
    VertV *verts = (VertV *)SDL_AllocateRenderVertices(renderer, count * 2 * sizeof(VertV), 4, &cmd->data.draw.first);
    int i;

    if (!verts) {
        return -1;
    }

    cmd->data.draw.count = count;
    for (i = 0; i < count; i++, rects++) {
        const SDL_FRect *rect = &rects[i];
        verts->x = rect->x;
        verts->y = rect->y;
        verts->z = 0.0f;
        verts++;

        verts->x = rect->x + rect->w;
        verts->y = rect->y + rect->h;
        verts->z = 0.0f;
        verts++;
    }

    return 0;
}

static int
N64_QueueCopy(SDL_Renderer *renderer, SDL_RenderCommand *cmd, SDL_Texture *texture,
             const SDL_Rect *srcrect, const SDL_FRect *dstrect)
{
    // We store the rects in the command's internal data
    // Since SDL_RenderCommand has limited space, we use the 'draw' struct members
    // or allocate a small piece of vertex memory to hold the parms.
    SDL_FRect *rects = (SDL_FRect *) SDL_AllocateRenderVertices(renderer, 2 * sizeof(SDL_FRect), 4, &cmd->data.draw.first);
    if (!rects) return -1;

    rects[0].x = srcrect->x; rects[0].y = srcrect->y;
    rects[0].w = srcrect->w; rects[0].h = srcrect->h;
    
    rects[1] = *dstrect;

    cmd->data.draw.count = 1;
    return 0;
}

static int
N64_QueueCopyEx(SDL_Renderer *renderer, SDL_RenderCommand *cmd, SDL_Texture *texture,
                const SDL_Rect *srcrect, const SDL_FRect *dstrect,
                const double angle, const SDL_FPoint *center, const SDL_RendererFlip flip)
{
    // CopyEx adds rotation (angle) and flipping
    typedef struct {
        SDL_FRect src;
        SDL_FRect dst;
        float angle;
        SDL_RendererFlip flip;
    } CopyExData;

    CopyExData *data = (CopyExData *) SDL_AllocateRenderVertices(renderer, sizeof(CopyExData), 4, &cmd->data.draw.first);
    if (!data) return -1;

    data->src.x = srcrect->x; data->src.y = srcrect->y;
    data->src.w = srcrect->w; data->src.h = srcrect->h;
    data->dst = *dstrect;
    data->angle = (float)angle;
    data->flip = flip;

    cmd->data.draw.count = 1;
    return 0;
}

 void
ResetBlendState(N64_BlendState* state)
{
}

static void
StartDrawing(SDL_Renderer *renderer)
{
    N64_RenderData *data = (N64_RenderData *)renderer->driverdata;

    /* Only attach if we haven't already attached this frame.
       SDL may call RunCommandQueue multiple times before RenderPresent.
    */
    if (!data->drawing) {
        surface_t *disp = display_get(); // This blocks until a backbuffer is available
            rdpq_attach(disp, NULL);
            data->drawing = SDL_TRUE;
    }
}

 void
N64_SetBlendState(N64_RenderData *data, N64_BlendState *state)
{
}

static int
N64_RunCommandQueue(SDL_Renderer * renderer, SDL_RenderCommand *cmd, void *vertices, size_t vertsize)
{
    const int MODE_UNKNOWN = 0;
    const int MODE_STANDARD = 1;
    const int MODE_FILL = 2;
    const int MODE_COPY = 3;

    N64_RenderData *data = (N64_RenderData *) renderer->driverdata;

    StartDrawing(renderer);
    if (!data->drawing) return -1;

    while (cmd) {
        switch (cmd->command) {
            case SDL_RENDERCMD_CLEAR: {
                color_t clear_color = RGBA32(cmd->data.color.r, cmd->data.color.g, cmd->data.color.b, cmd->data.color.a);
                rdpq_clear(clear_color);
                data->last_mode = MODE_UNKNOWN; // Clear resets the RDP state
                break;
            }
            case SDL_RENDERCMD_FILL_RECTS: {
                const size_t count = cmd->data.draw.count;
                const VertV *verts = (const VertV *)((Uint8 *)vertices + cmd->data.draw.first);
                rdpq_set_mode_fill(RGBA16(cmd->data.draw.r, cmd->data.draw.g, cmd->data.draw.b, cmd->data.draw.a));
                data->last_mode = MODE_FILL;
                for (size_t i = 0; i < count; i++) {
                    rdpq_fill_rectangle(verts[i*2].x, verts[i*2].y, verts[i*2+1].x, verts[i*2+1].y);
                }
                break;
            }
            case SDL_RENDERCMD_COPY: {
                const SDL_FRect *rects = (const SDL_FRect *) ((Uint8 *)vertices + cmd->data.draw.first);
                SDL_Texture *texture = cmd->data.draw.texture;
                N64_TextureData *n64_tex = (N64_TextureData *)texture->driverdata;

                if (rects[0].w <= 0 || rects[0].h <= 0) break;

                /* 1. Sync Texture & TLUT State */
                EnsureTextureState(data, n64_tex);

                /* 2. Optimized Mode Sync: Only set COPY mode if we aren't already in it */
                if (data->last_mode != MODE_COPY) {
                    rdpq_set_mode_copy(false); 
                    data->last_mode = MODE_COPY;
                }

                /* 3. Fast-Blit Check: If scale is 1:1, bypass the parms/matrix logic entirely */
                //if (rects[1].w == rects[0].w && rects[1].h == rects[0].h) {
                    rdpq_sprite_blit(n64_tex->sprite, rects[1].x, rects[1].y, NULL);
                /*} else {
                    // Only do the division and parms if we are actually scaling //
                    rdpq_blitparms_t parms = {
                        .s0 = rects[0].x, .t0 = rects[0].y,
                        .width = rects[0].w, .height = rects[0].h,
                        .scale_x = rects[1].w / rects[0].w,
                        .scale_y = rects[1].h / rects[0].h,
                    };
                    rdpq_sprite_blit(n64_tex->sprite, rects[1].x, rects[1].y, &parms);
                }*/
                break;
            }
            case SDL_RENDERCMD_COPY_EX: {
                typedef struct { SDL_FRect src; SDL_FRect dst; float angle; SDL_RendererFlip flip; } CopyExData;
                const CopyExData *ex_data = (const CopyExData *) ((Uint8 *)vertices + cmd->data.draw.first);
                N64_TextureData *n64_tex = (N64_TextureData *)cmd->data.draw.texture->driverdata;

                if (ex_data->src.w <= 0 || ex_data->src.h <= 0) break;

                EnsureTextureState(data, n64_tex);

                if (data->last_mode != MODE_STANDARD) {
                    rdpq_set_mode_standard();
                    data->last_mode = MODE_STANDARD;
                }

                /* Disable N64 blender for opaque textures to double fillrate */
                if (cmd->data.draw.texture->blendMode == SDL_BLENDMODE_NONE) {
                    if (data->last_blend != SDL_BLENDMODE_NONE) {
                        rdpq_mode_blender(0);
                        data->last_blend = SDL_BLENDMODE_NONE;
                    }
                } else {
                    // Restore standard alpha blending
                    rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY); 
                }

                if (data->last_blend != SDL_BLENDMODE_BLEND) {
                    rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
                    data->last_blend = SDL_BLENDMODE_BLEND;
                }

                rdpq_blitparms_t parms = {
                    .s0 = ex_data->src.x, .t0 = ex_data->src.y,
                    .width = ex_data->src.w, .height = ex_data->src.h,
                    .scale_x = ex_data->dst.w / ex_data->src.w,
                    .scale_y = ex_data->dst.h / ex_data->src.h,
                    .flip_x = (ex_data->flip & SDL_FLIP_HORIZONTAL),
                    .flip_y = (ex_data->flip & SDL_FLIP_VERTICAL),
                    .theta = ex_data->angle * (M_PI / 180.0f)
                };

                rdpq_sprite_blit(n64_tex->sprite, ex_data->dst.x, ex_data->dst.y, &parms);
                break;
            }
            default:
                break;
        }
        cmd = cmd->next;
    }
    return 0;
}

static int
N64_RenderReadPixels(SDL_Renderer *renderer, const SDL_Rect *rect,
                    Uint32 pixel_format, void *pixels, int pitch)
{
    //fprintf(stderr, "N64_RenderReadPixels\n");
    return SDL_Unsupported();
}

// #include <malloc.h>
// void print_memory(display_context_t disp) {
//     struct mallinfo mem_info = mallinfo();
//     int mem_used = mem_info.uordblks / 1024;
//     int mem_total = get_memory_size() / 1024;

//     char text[100];
//     snprintf(text, 100, "total_mem: %dKB/%dKB\n", mem_used, mem_total);
//     graphics_set_color(0xffffffff, 0x000000ff);
//     graphics_draw_text(disp, 10, 10, text);
// }

void N64_RenderPresent(SDL_Renderer * renderer)
{
    N64_RenderData *data = (N64_RenderData *) renderer->driverdata;

    if (data->drawing) {
        rdpq_detach_show();
        data->drawing = SDL_FALSE;
    } else {
        // If nothing was drawn, we MUST still flip the buffer 
        // using the RDP's logic to keep the cycle consistent.
        surface_t *disp = display_get();
        if (disp) {
            rdpq_attach(disp, NULL);
            rdpq_detach_show();
        }
    }
    return;
}

static void
N64_DestroyTexture(SDL_Renderer *renderer, SDL_Texture *texture)
{
    N64_RenderData *renderdata = (N64_RenderData *)renderer->driverdata;
    N64_TextureData *n64_texture = (N64_TextureData *)texture->driverdata;

    if (renderdata == 0)
        return;

    if(n64_texture == 0)
        return;

    SDL_free(n64_texture->sprite);
    SDL_free(n64_texture);
    texture->driverdata = NULL;
}

static void
N64_DestroyRenderer(SDL_Renderer * renderer)
{
    //fprintf(stderr, "N64_DestroyRenderer\n");
    N64_RenderData *data = (N64_RenderData *)renderer->driverdata;
    if (data) {
        SDL_free(data);
        renderer->driverdata = NULL;
    }
    SDL_free(renderer);
}

static int
N64_SetVSync(SDL_Renderer * renderer, const int vsync)
{
    //fprintf(stderr, "N64_SetVSync\n");
    return 0;
}

static SDL_Renderer *
N64_CreateRenderer(SDL_Window *window, Uint32 flags)
{
    SDL_Renderer *renderer;
    N64_RenderData *data;

    renderer = (SDL_Renderer *) SDL_calloc(1, sizeof(*renderer));
    if (!renderer) {
        SDL_OutOfMemory();
        return NULL;
    }

    data = (N64_RenderData *) SDL_calloc(1, sizeof(*data));
    if (!data) {
        SDL_free(renderer);
        SDL_OutOfMemory();
        return NULL;
    }

    // Standard SDL2 Renderer Assignments
    renderer->WindowEvent = N64_WindowEvent;
    renderer->CreateTexture = N64_CreateTexture;
    renderer->UpdateTexture = N64_UpdateTexture;
    renderer->LockTexture = N64_LockTexture;
    renderer->UnlockTexture = N64_UnlockTexture;
    renderer->SetTextureScaleMode = N64_SetTextureScaleMode;
    renderer->SetRenderTarget = N64_SetRenderTarget;
    renderer->QueueSetViewport = N64_QueueSetViewport;
    renderer->QueueSetDrawColor = N64_QueueSetViewport; 
    renderer->QueueDrawPoints = N64_QueueDrawPoints;
    renderer->QueueDrawLines = N64_QueueDrawPoints; 
    renderer->QueueGeometry = N64_QueueGeometry;
    renderer->QueueFillRects = N64_QueueFillRects;
    renderer->QueueCopy = N64_QueueCopy;
    renderer->QueueCopyEx = N64_QueueCopyEx;
    renderer->RunCommandQueue = N64_RunCommandQueue;
    renderer->RenderReadPixels = N64_RenderReadPixels;
    renderer->RenderPresent = N64_RenderPresent;
    renderer->DestroyTexture = N64_DestroyTexture;
    renderer->DestroyRenderer = N64_DestroyRenderer;
    renderer->SetVSync = N64_SetVSync;
    
    renderer->info = N64_RenderDriver.info;
    renderer->driverdata = data;
    renderer->window = window;

        /* initialize draw-state cache */
    data->last_sprite = NULL;
    data->last_tlut = -1;
    data->last_mode = 0;
    data->last_blend = SDL_BLENDMODE_INVALID; /* NEW */

    N64_GetDisplaySize(&data->screen_w, &data->screen_h);

    return renderer;
}

SDL_RenderDriver N64_RenderDriver = {
    .CreateRenderer = N64_CreateRenderer,
    .info = {
        .name = "N64",
        // CHANGED: Use ACCELERATED so SDL picks this over the software fallback
        .flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC,
        .num_texture_formats = 1,
        .texture_formats = { 
            [0] = SDL_PIXELFORMAT_RGBA5551,
        },
        .max_texture_width = 640,
        .max_texture_height = 480,
     }
};

#endif /* SDL_VIDEO_RENDER_N64 */

/* vi: set ts=4 sw=4 expandtab: */

