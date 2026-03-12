#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "common.h"
#include "gfxapi.h"

/*==========================================================================
   GFX_ScaleLine() - Does the scale from scale table ( _stable )
 ==========================================================================*/
void 
GFX_ScaleLine(
    char *outmem, 
    char *inmem
)
{
#ifdef __N64__
    int loop = tablelen - 1;
    for (; loop >= 3; loop -= 4)
    {
        *outmem++ = *(inmem + stable[loop]);
        *outmem++ = *(inmem + stable[loop - 1]);
        *outmem++ = *(inmem + stable[loop - 2]);
        *outmem++ = *(inmem + stable[loop - 3]);
    }
    for (; loop >= 0; loop--)
    {
        *outmem++ = *(inmem + stable[loop]);
    }
#else
    int loop;
    for (loop = tablelen - 1; loop >= 0; loop--)
    {
        *outmem = *(inmem + stable[loop]);
        outmem++;
    }
#endif
}

/*==========================================================================
   GFX_CScaleLine() - scale from scale table (_stable) color 0 transparent 
 ==========================================================================*/
void 
GFX_CScaleLine(
    char *outmem, 
    char *inmem
)
{
#ifdef __N64__
    int loop = tablelen - 1;
    for (; loop >= 3; loop -= 4)
    {
        char px1 = *(inmem + stable[loop]);
        if (px1) *outmem = px1;
        outmem++;
        
        char px2 = *(inmem + stable[loop - 1]);
        if (px2) *outmem = px2;
        outmem++;
        
        char px3 = *(inmem + stable[loop - 2]);
        if (px3) *outmem = px3;
        outmem++;
        
        char px4 = *(inmem + stable[loop - 3]);
        if (px4) *outmem = px4;
        outmem++;
    }
    for (; loop >= 0; loop--)
    {
        char px = *(inmem + stable[loop]);
        if (px) *outmem = px;
        outmem++;
    }
#else
    int loop;
    for (loop = tablelen - 1; loop >= 0; loop--)
    {
        char px = *(inmem + stable[loop]);
        if (px)
            *outmem = px;
        outmem++;
    }
#endif
}

/*==========================================================================
   GFX_DisplayScreen() - Puts Display Buffer into Video memory
 ==========================================================================*/
void 
GFX_DisplayScreen(
    void
)
{
    int loop;
    char *src = &displaybuffer[ud_y * SCREENWIDTH + ud_x];
    char *dest = &displayscreen[ud_y * SCREENWIDTH + ud_x];
    
    // Note for N64: Standard memcpy in SDKs (libdragon/NuSystem) is highly 
    // optimized assembly using 64-bit loads/stores and cache prefetching. 
    // It is best to let memcpy handle this rather than writing a manual loop.
    if (ud_lx == SCREENWIDTH)
    {
        memcpy(dest, src, SCREENWIDTH * ud_ly);
    }
    else
    {
        for (loop = 0; loop < ud_ly; loop++)
        {
            memcpy(dest, src, ud_lx);
            dest += SCREENWIDTH;
            src += SCREENWIDTH;
        }
    }
    
    update_start = 0;
}

/*==========================================================================
   GFX_ShadeSprite() -
 ==========================================================================*/
void 
GFX_ShadeSprite(
    char *dest, 
    char *inmem, 
    char *dtable
)
{
    GFX_PIC* h = (GFX_PIC*)inmem;
    GFX_SPRITE* ah = (GFX_SPRITE*)inmem;

    while ((int16_t)ah->offset.get_value() != -1)
    {
        char* d = dest + (uint16_t)ah->offset.get_value();

#ifdef __N64__
        int width = (uint16_t)h->width.get_value();
        int loop = 0;
        int fast_len = width & ~3;
        for (; loop < fast_len; loop += 4)
        {
            d[0] = dtable[(uint8_t)d[0]];
            d[1] = dtable[(uint8_t)d[1]];
            d[2] = dtable[(uint8_t)d[2]];
            d[3] = dtable[(uint8_t)d[3]];
            d += 4;
        }
        for (; loop < width; loop++, d++)
        {
            *d = dtable[(uint8_t)*d];
        }
#else
        for (int loop = 0; loop < (uint16_t)h->width.get_value(); loop++, d++)
            *d = dtable[(uint8_t)*d];
#endif

        h = (GFX_PIC*)((char*)&h->height + (uint16_t)h->width.get_value());
        ah = (GFX_SPRITE*)h;
    }
}

/*==========================================================================
   GFX_DrawSprite() -
 ==========================================================================*/
void 
GFX_DrawSprite(
    char *dest, 
    char *inmem
)
{
    // memcpy is utilized here. Best left as-is for the compiler/SDK.
    GFX_PIC* h = (GFX_PIC*)inmem;
    GFX_SPRITE* ah = (GFX_SPRITE*)inmem;
    
    while ((int16_t)ah->offset.get_value() != -1)
    {
        memcpy(dest + (uint16_t)ah->offset.get_value(), (char*)&h->height, (uint16_t)h->width.get_value());

        h = (GFX_PIC*)((char*)&h->height + (uint16_t)h->width.get_value());
        ah = (GFX_SPRITE*)h;
    }
}

/*==========================================================================
   GFX_DrawChar() -
 ==========================================================================*/
void 
GFX_DrawChar(
    char *dest, 
    char *inmem, 
    int width, 
    int height, 
    int addx, 
    int color
)
{
    do
    {
#ifdef __N64__
        int loop = 0;
        int fast_w = width & ~3;
        for (; loop < fast_w; loop += 4)
        {
            if (inmem[0]) dest[0] = color + inmem[0];
            if (inmem[1]) dest[1] = color + inmem[1];
            if (inmem[2]) dest[2] = color + inmem[2];
            if (inmem[3]) dest[3] = color + inmem[3];
            dest += 4;
            inmem += 4;
        }
        for (; loop < width; loop++)
        {
            if (*inmem) *dest = color + *inmem;
            dest++;
            inmem++;
        }
#else
        for (int loop = 0; loop < width; loop++)
        {
            if (*inmem)
                *dest = color + *inmem;
            dest++;
            inmem++;
        }
#endif
        inmem += addx;
        dest += SCREENWIDTH - width;
    
    } while (--height);
}

/*==========================================================================
   GFX_Shade() - Remaps Bytes according to shade table 
 ==========================================================================*/
void 
GFX_Shade(
    char *outmem, 
    int maxlen, 
    char *dtable
)
{
#ifdef __N64__
    int loop = 0;
    int fast_len = maxlen & ~3;
    for (; loop < fast_len; loop += 4)
    {
        outmem[loop]     = dtable[(uint8_t)outmem[loop]];
        outmem[loop + 1] = dtable[(uint8_t)outmem[loop + 1]];
        outmem[loop + 2] = dtable[(uint8_t)outmem[loop + 2]];
        outmem[loop + 3] = dtable[(uint8_t)outmem[loop + 3]];
    }
    for (; loop < maxlen; loop++)
    {
        outmem[loop] = dtable[(uint8_t)outmem[loop]];
    }
#else
    for (int loop = 0; loop < maxlen; loop++)
    {
        outmem[loop] = dtable[(uint8_t)outmem[loop]];
    }
#endif
}

/*==========================================================================
   GFX_PutPic() - Puts Picture into buffer 
 ==========================================================================*/
void 
GFX_PutPic(
    void
)
{
    char *p = &displaybuffer[gfx_xp + gfx_yp * SCREENWIDTH];
    char *src = gfx_inmem;
    
    for (int loop = 0; loop < gfx_ly; loop++)
    {
        // memcpy is optimal here
        memcpy(p, src, gfx_lx);
        p += SCREENWIDTH;
        src += gfx_lx + gfx_imga;
    }
}

/*==========================================================================
   GFX_PutMaskPic() - Puts Picture into buffer with color 0 see thru
 ==========================================================================*/
void 
GFX_PutMaskPic(
    void
)
{
    char *p = &displaybuffer[gfx_xp + gfx_yp * SCREENWIDTH];
    char *src = gfx_inmem;
    
    for (int loop = 0; loop < gfx_ly; loop++)
    {
#ifdef __N64__
        int i = 0;
        int fast_lx = gfx_lx & ~3;
        for (; i < fast_lx; i += 4)
        {
            if (src[i])     p[i]     = src[i];
            if (src[i + 1]) p[i + 1] = src[i + 1];
            if (src[i + 2]) p[i + 2] = src[i + 2];
            if (src[i + 3]) p[i + 3] = src[i + 3];
        }
        for (; i < gfx_lx; i++)
        {
            if (src[i] != 0) p[i] = src[i];
        }
#else
        for (int i = 0; i < gfx_lx; i++)
        {
            if (src[i] != 0)
                p[i] = src[i];
        }
#endif
        p += SCREENWIDTH;
        src += gfx_lx + gfx_imga;
    }
}