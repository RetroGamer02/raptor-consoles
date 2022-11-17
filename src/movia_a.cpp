#include <string.h>
#include "common.h"
#include "movie.h"
#include "gfxapi.h"

#include "tonccpy.h"

/*************************************************************************
ANIM_Render () - Renders an ANIM FRAME
 *************************************************************************/
void 
ANIM_Render(
	movanim_t *inmem
)
{
	while (inmem->f_0)
	{
		int l = inmem->f_6;
		int p = inmem->f_4;
		
		inmem++;
		
		tonccpy(&displaybuffer[p], inmem, l);
		
		inmem = (movanim_t*)((char*)inmem + l);
	}
}
