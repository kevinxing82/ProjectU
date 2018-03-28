#include "kxRenderObject.h"


USING_KX
kxRenderObject::~kxRenderObject()
{
	if (this->head_vlist_local)
	{
		delete this->head_vlist_local;
	}
	if (this->head_vlist_tran)
	{
		delete this->head_vlist_tran;
	}
	if (this->tlist)
	{
		delete[] this->tlist;
	}
	if (this->plist)
	{
		delete[] this->plist;
	}
	if (this->avg_radius)
	{
		delete[] this->avg_radius;
	}
	if (this->max_radius)
	{
		delete[] this->max_radius;
	}
}

kxRenderObject::~kxRenderObject()
{
}

int kxRenderObject::Init(int _num_vertices, int _num_polys, int _num_frames)
{
	this->vlist_local = new kxVertex[_num_vertices*_num_frames];
	memset((void*)this->vlist_local, 0, sizeof(kxVertex)*_num_vertices*_num_frames);
	this->vlist_tran = new kxVertex[_num_vertices*_num_frames];
	memset((void*)this->vlist_tran, 0, sizeof(kxVertex)*_num_vertices*_num_frames);

	this->tlist = new kxVector2[_num_polys * 3];
	memset((void*)this->tlist, 0, sizeof(kxVector2)*_num_vertices*_num_frames);

	this->avg_radius = new float[_num_frames];
	memset((void*)this->avg_radius,0, sizeof(float)*_num_frames);

	this->max_radius = new float[_num_frames];
	memset((void*)this->max_radius, 0,sizeof(float)*_num_frames);

	this->plist = new kxPolygon[_num_polys];
	memset((void*)this->plist, 0, sizeof(kxPolygon)*_num_polys);

	this->head_vlist_local = this->vlist_local;
	this->head_vlist_tran = this->vlist_tran;

	this->num_frames = _num_frames;
	this->num_polys = _num_polys;
	this->num_vertices = _num_vertices;
	this->total_vertices = _num_vertices*_num_frames;

	return 1;
}

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
			vlist_tran[vertex].position =vlist_local[vertex].position+ world_pos;
		}
	}
	else
	{
		for (int vertex = 0; vertex < num_vertices; vertex++)
		{
			vlist_tran[vertex].position = vlist_tran[vertex].position + world_pos;
		}
	}
}

int kxRenderObject::SetFrame(int frame)
{
	if (!this)
	{
		return 0;
	}

	if (!(this->attr&OBJECT4DV2_ATTR_MULTI_FRAME))
	{
		return 0;
	}

	if (frame < 0)
	{
		frame = 0;
	}
	else if(frame>=this->num_frames)
	{
		frame = this->num_frames - 1;
	}
	this->curr_frame = frame;

	this->vlist_local = &(this->head_vlist_local[frame*this->num_vertices]);
	this->vlist_tran = &(this->head_vlist_tran[frame*this->num_vertices]);
	return 1;
}
