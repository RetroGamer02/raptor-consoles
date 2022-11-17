#include "tonccpy.h"
//# tonccpy.c

//! VRAM-safe cpy.
/*! This version mimics memcpy in functionality, with 
    the benefit of working for VRAM as well. It is also 
    slightly faster than the original memcpy, but faster 
    implementations can be made.
    \param dst  Destination pointer.
    \param src  Source pointer.
    \param size Fill-length in bytes.
    \note   The pointers and size need not be word-aligned.
*/
void tonccpy(void *dst, const void *src, uint size)
{
    if (size==0 || dst==0 || src==0)
        return;

    uint count;
    unsigned short *dst16;     // hword destination
    unsigned char  *src8;      // byte source

    // Ideal case: copy by 4x words. Leaves tail for later.
    if ( ((unsigned int)src|(unsigned int)dst)%4==0 && size>=4)
    {
        unsigned int *src32= (unsigned int*)src, *dst32= (unsigned int*)dst;

        count= size/4;
        uint tmp= count&3;
        count /= 4;

        // Duff's Device, good friend!
        switch(tmp) {
            do {    *dst32++ = *src32++;
        case 3:     *dst32++ = *src32++;
        case 2:     *dst32++ = *src32++;
        case 1:     *dst32++ = *src32++;
        case 0:     ; } while (count--);
        }

        // Check for tail
        size &= 3;
        if (size == 0)
            return;

        src8= (unsigned char*)src32;
        dst16= (unsigned short*)dst32;
    }
    else        // Unaligned.
    {
        uint dstOfs= (unsigned int)dst&1;
        src8= (unsigned char*)src;
        dst16= (unsigned short*)(dst-dstOfs);

        // Head: 1 byte.
        if (dstOfs != 0)
        {
            *dst16= (*dst16 & 0xFF) | *src8++<<8;
            dst16++;
            if (--size==0)
                return;
        }
    }

    // Unaligned main: copy by 2x byte.
    count= size/2;
    while (count--)
    {
        *dst16++ = src8[0] | src8[1]<<8;
        src8 += 2;
    }

    // Tail: 1 byte.
    if (size&1)
        *dst16= (*dst16 &~ 0xFF) | *src8;
}
//# toncset.c

//! VRAM-safe memset, internal routine.
/*! This version mimics memset in functionality, with 
    the benefit of working for VRAM as well. It is also 
    slightly faster than the original memset.
    \param dst  Destination pointer.
    \param fill Word to fill with.
    \param size Fill-length in bytes.
    \note   The \a dst pointer and \a size need not be 
        word-aligned. In the case of unaligned fills, \a fill 
        will be masked off to match the situation.
*/
void __toncset(void *dst, unsigned int fill, uint size)
{
    if (size==0 || dst==0)
        return;

    uint left= (unsigned int)dst&3;
    unsigned int *dst32= (unsigned int*)(dst-left);
    unsigned int count, mask;

    // Unaligned head.
    if (left != 0)
    {
        // Adjust for very small stint.
        if (left+size<4)
        {
            mask= BIT_MASK(size*8)<<(left*8);   
            *dst32= (*dst32 &~ mask) | (fill & mask);
            return;
        }

        mask= BIT_MASK(left*8);
        *dst32= (*dst32 & mask) | (fill&~mask);
        dst32++;
        size -= 4-left;
    }

    // Main stint.
    count= size/4;
    uint tmp= count&3;
    count /= 4;

    switch(tmp) {
        do {    *dst32++ = fill;
    case 3:     *dst32++ = fill;
    case 2:     *dst32++ = fill;
    case 1:     *dst32++ = fill;
    case 0:     ; } while (count--);
    }

    // Tail
    size &= 3;
    if (size)
    {
        mask= BIT_MASK(size*8);
        *dst32= (*dst32 &~ mask) | (fill & mask);
    }
}