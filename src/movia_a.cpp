#include <string.h>
#include "common.h"
#include "movie.h"
#include "gfxapi.h"

/*************************************************************************
ANIM_Render () - Renders an ANIM FRAME
 *************************************************************************/
void 
ANIM_Render(
	ANIMLINE *inmem
)
{
	#ifdef __PPC__
	while (inmem->opt.get_value())
	{
		int l = inmem->length.get_value();
		int p = inmem->offset.get_value();
		
		inmem++;
		
		memcpy(&displaybuffer[p], inmem, l);
		
		inmem = (ANIMLINE*)((char*)inmem + l);
	}
	#else
	while (inmem->opt)
	{
		int l = inmem->length;
		int p = inmem->offset;
		
		inmem++;
		
		memcpy(&displaybuffer[p], inmem, l);
		
		inmem = (ANIMLINE*)((char*)inmem + l);
	}
	#endif
}
