#pragma once

#include "kxVector.h"
#include "kxGlobal.h"
#include "kxPolygon.h"
#include "kxMatrix.h"

KX_BEGIN
class kxRenderObject
{
public:
	int Reset();
	int id;
	char name[64];
	int state;
	int attr;

	float avg_radius;
	float max_radius;

	kxVector4 world_pos;
	kxVector4 dir;
	kxVector4 ux, uy, uz;

	int  num_vertices;

	kxVector4 vlist_local[KX_OBJECT_MAX_VERTICES];
	kxVector4 vlist_tran[KX_OBJECT_MAX_VERTICES];

	int  num_polys;
	kxPolygon plist[KX_OBJECT_MAX_POLYS];
};
KX_END
