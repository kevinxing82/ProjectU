#include "kxRenderObject.h"

USING_KX
kxRenderObject::kxRenderObject()
{
}

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

//int kxRenderObject::Init()
//{
//	this->Init(this->num_vertices, this->num_polys, this->num_frames);
//	return 0;
//}

int kxRenderObject::Init(int _num_vertices, int _num_polys, int _num_frames)
{
	this->vlist_local = new kxVertex[_num_vertices*_num_frames];
	//memset((void*)this->vlist_local, 0, sizeof(kxVertex)*_num_vertices*_num_frames);
	this->vlist_tran = new kxVertex[_num_vertices*_num_frames];
	//memset((void*)this->vlist_tran, 0, sizeof(kxVertex)*_num_vertices*_num_frames);

	this->tlist = new kxVector2[_num_polys * 3];
	//memset((void*)this->tlist, 0, sizeof(kxVector2)*_num_vertices*_num_frames);

	this->avg_radius = new float[_num_frames];
	//memset((void*)this->avg_radius,0, sizeof(float)*_num_frames);

	this->max_radius = new float[_num_frames];
	//memset((void*)this->max_radius, 0,sizeof(float)*_num_frames);

	this->plist = new kxPolygon[_num_polys];

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
	RESET_BIT(state, OBJECT4D_STATE_CULLED);
	for (int poly = 0; poly < num_polys; poly++)
	{
		kxPolygon* currPoly = &plist[poly];
		if (!(currPoly->state&POLY4D_STATE_ACTIVE))
			continue;
		RESET_BIT(currPoly->state, POLY4D_STATE_CLIPPED);
		RESET_BIT(currPoly->state, POLY4D_STATE_BACKFACE);
	}
	return 0;
}

void kxRenderObject::SetColor(int col)
{
	for (int poly = 0; poly < num_polys; poly++)
	{
		kxPolygon* currPoly = &plist[poly];
		currPoly->color->setRGBA(col);
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

	if (!(this->attr&OBJECT4D_ATTR_MULTI_FRAME))
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

float kxRenderObject::ComputeRadius()
{
	this->avg_radius[this->curr_frame] = 0;
	this->max_radius[this->curr_frame] = 0;

	for (int vertex = 0; vertex < this->num_vertices; vertex++)
	{
		float distToVertex = 
			sqrt(this->vlist_local[vertex].position.x*this->vlist_local[vertex].position.x +
			       this->vlist_local[vertex].position.y*this->vlist_local[vertex].position.y +
			       this->vlist_local[vertex].position.z*this->vlist_local[vertex].position.z);

		this->avg_radius[this->curr_frame] += distToVertex;
		if (distToVertex > this->max_radius[this->curr_frame])
		{
			this->max_radius[this->curr_frame] = distToVertex;
		}
	}

	this->avg_radius[this->curr_frame] /= this->num_vertices;
	return (this->max_radius[0]);
}

int kxRenderObject::ComputePolyNormals()
{
	for (int poly = 0; poly < this->num_polys; poly++)
	{
		int vindex_0 = this->plist[poly].vert[0];
		int vindex_1 = this->plist[poly].vert[1];
		int vindex_2 = this->plist[poly].vert[2];

		kxVector4 u,v;
		kxVector3 n;
		u = this->vlist_local[vindex_1].position - this->vlist_local[vindex_0].position;
		u.w = 1;

		v = this->vlist_local[vindex_2].position - this->vlist_local[vindex_0].position;
		v.w = 1;

		n = kxVector3(u.x, u.y, u.z).cross(kxVector3(v.x, v.y, v.z));
		 
		this->plist[poly].nlength = n.length();
	}
	return 1;
}

int kxRenderObject::ComputeVertexNormasl()
{
	int* polys_touch_vertex = new int[OBJECT4D_MAX_VERTICES];
	for (int poly = 0; poly < this->num_polys; poly++)
	{
		if (this->plist[poly].attr&POLY4D_ATTR_SHADE_MODE_GOURAUD)
		{
			int vindex_0 = this->plist[poly].vert[0];
			int vindex_1 = this->plist[poly].vert[1];
			int vindex_2 = this->plist[poly].vert[2];

			kxVector4 u, v;
			kxVector3 n;

			u = this->vlist_local[vindex_1].position - this->vlist_local[vindex_0].position;
			u.w = 1;

			v = this->vlist_local[vindex_2].position - this->vlist_local[vindex_0].position;
			v.w = 1;

			n = kxVector3(u.x, u.y, u.z).cross(kxVector3(v.x, v.y, v.z));

			polys_touch_vertex[vindex_0]++;
			polys_touch_vertex[vindex_1]++;
			polys_touch_vertex[vindex_2]++;

			this->vlist_local[vindex_0].normal += kxVector4(n.x, n.y, n.z, 1);
			this->vlist_local[vindex_1].normal += kxVector4(n.x, n.y, n.z, 1);
			this->vlist_local[vindex_2].normal += kxVector4(n.x, n.y, n.z, 1);
		}
	}
	for (int vertex = 0; vertex < this->num_vertices; vertex++)
	{
		if (polys_touch_vertex[vertex] >= 1)
		{
			this->vlist_local[vertex].normal.x /= polys_touch_vertex[vertex];
			this->vlist_local[vertex].normal.y /= polys_touch_vertex[vertex];
			this->vlist_local[vertex].normal.z /= polys_touch_vertex[vertex];

			this->vlist_local[vertex].normal.normalize();
		}
	}
	return 1;
}
