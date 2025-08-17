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
	while (inmem->opt.get_value())
	{
		int l = inmem->length.get_value();
		int p = inmem->offset.get_value();
		
		inmem++;
		
		memcpy(&displaybuffer[p], inmem, l);
		
		inmem = (ANIMLINE*)((char*)inmem + l);
	}
}
