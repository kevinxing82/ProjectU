#include "kxRenderObject.h"


USING_KX

int kxRenderObject::Reset()
{
	RESET_BIT(state, OBJECT4DV1_STATE_CULLED);
	for (int poly = 0; poly < num_polys; poly++)
	{
		kxPolygon* currPoly = &plist[poly];
		if (!(currPoly->state&POLY4DV1_STATE_ACTIVE))
			continue;
		RESET_BIT(currPoly->state, POLY4DV1_STATE_CLIPPED);
		RESET_BIT(currPoly->state, POLY4DV1_STATE_BACKFACE);
	}
	return 0;
}

void kxRenderObject::SetColor(int col)
{
	for (int poly = 0; poly < num_polys; poly++)
	{
		kxPolygon* currPoly = &plist[poly];
		currPoly->color.setRGBA(col);
	}
}

void kxRenderObject::ModelToWorld(int coordSelect)
{
	if (coordSelect == TRANSFORM_LOCAL_TO_TRANS)
	{
		for (int vertex = 0; vertex < num_vertices; vertex++)
		{
			vlist_tran[vertex] = vlist_local[vertex] + world_pos;
		}
	}
	else
	{
		for (int vertex = 0; vertex < num_vertices; vertex++)
		{
			vlist_tran[vertex] = vlist_tran[vertex] + world_pos;
		}
	}
}
