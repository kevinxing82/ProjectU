#include "kxRenderList.h"

USING_KX
void kxRenderList::Reset()
{
	num_polys = 0;
}

int kxRenderList::modelToWorld(const kxVector4 & world_pos, int coord_select)
{
	if (coord_select == TRANSFORM_LOCAL_TO_TRANS)
	{
		for (int poly = 0; poly < num_polys; poly++)
		{
			kxPolygonList* currPoly = poly_ptrs[poly];

			if ((currPoly == NULL) ||
				!(currPoly->state&POLY4DV1_STATE_ACTIVE) ||
				(currPoly->state&POLY4DV1_STATE_CLIPPED) ||
				(currPoly->state&POLY4DV1_STATE_BACKFACE))
			{
				continue;
			}
			for (int vertex = 0; vertex < 3; vertex++)
			{
				currPoly->tlist[vertex] = currPoly->vlist[vertex] + world_pos;
			}
		}
	}
	else
	{
		for (int poly = 0; poly < num_polys; poly++)
		{
			kxPolygonList* currPoly = poly_ptrs[poly];

			if ((currPoly == NULL) || !(currPoly->state&POLY4DV1_STATE_ACTIVE) ||
				(currPoly->state&POLY4DV1_STATE_CLIPPED) ||
				(currPoly->state&POLY4DV1_STATE_BACKFACE))
			{
				continue;
			}
			for (int vertex = 0; vertex < 3; vertex++)
			{
				currPoly->tlist[vertex] = currPoly->tlist[vertex] + world_pos;
			}
		}
	}
	return 0;
}

int kxRenderList::Insert(kxPolygonList * polygonList)
{
	if (num_polys >= RENDERLIST4DV1_MAX_POLYS)
	{
		return (0);
	}
	memcpy_s((void*)&poly_data[num_polys], sizeof(kxPolygonList), (void *)polygonList, sizeof(kxPolygonList));

	if (num_polys == 0)
	{
		poly_data[0].next = NULL;
		poly_data[0].prev = NULL;
	}
	else
	{
		poly_data[num_polys].next = NULL;
		poly_data[num_polys].prev = &poly_data[num_polys - 1];

		poly_data[num_polys - 1].next = &poly_data[num_polys];
	}
	num_polys++;
	return (1);
}

int kxRenderList::Insert(kxPolygon * polygon)
{
	if (num_polys >= RENDERLIST4DV1_MAX_POLYS)
	{
		return (0);
	}
	poly_ptrs[num_polys] = &poly_data[num_polys];

	poly_data[num_polys].state = polygon->state;
	poly_data[num_polys].attr = polygon->state;
	poly_data[num_polys].color = polygon->state;

	poly_data[num_polys].tlist[0] = kxVector4(polygon->vlist[polygon->vert[0]]);
	poly_data[num_polys].tlist[1] = kxVector4(polygon->vlist[polygon->vert[1]]);
	poly_data[num_polys].tlist[2] = kxVector4(polygon->vlist[polygon->vert[2]]);

	poly_data[num_polys].vlist[0] = kxVector4(polygon->vlist[polygon->vert[0]]);
	poly_data[num_polys].vlist[1] = kxVector4(polygon->vlist[polygon->vert[1]]);
	poly_data[num_polys].vlist[2] = kxVector4(polygon->vlist[polygon->vert[2]]);

	if (num_polys == 0)
	{
		poly_data[0].next = NULL;
		poly_data[0].prev = NULL;
	}
	else
	{
		poly_data[num_polys].next = NULL;
		poly_data[num_polys].prev = &poly_data[num_polys - 1];

		poly_data[num_polys - 1].next = &poly_data[num_polys];
	}
	num_polys++;
	return (1);
}

int kxRenderList::Insert(kxRenderObject * object,int insert_local)
{
	if (!(object->state&OBJECT4DV1_STATE_ACTIVE) ||
		(object->state&OBJECT4DV1_STATE_CULLED) ||
		!(object->state&OBJECT4DV1_STATE_VISIBLE))
	{
		return (0);
	}
	for (int poly = 0; poly < object->num_polys; poly++)
	{
		kxPolygon* currPoly = &object->plist[poly];

		if (!(currPoly->state&POLY4DV1_STATE_ACTIVE) ||
			(currPoly->state&POLY4DV1_STATE_CLIPPED) ||
		    (currPoly->state&POLY4DV1_STATE_BACKFACE))
		{
			continue;
		}

		kxVector4* vlistOld = currPoly->vlist;
		if (insert_local)
		{
			currPoly->vlist = object->vlist_local;
		}
		else
		{
			currPoly->vlist = object->vlist_tran;
		}
		if (!Insert(currPoly))
		{
			currPoly->vlist = vlistOld;
			return 0;
		}
		currPoly->vlist = vlistOld;
	}
	return 1;
}
