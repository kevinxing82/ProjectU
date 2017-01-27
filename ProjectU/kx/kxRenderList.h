#pragma once

#include "kxGlobal.h"
#include "kxPolygon.h"
#include "kxRenderObject.h"

KX_BEGIN
class kxRenderList
{
public:
	int state;
	int attr;

	kxPolygonList* poly_ptrs[RENDERLIST4DV1_MAX_POLYS];
	kxPolygonList  poly_data[RENDERLIST4DV1_MAX_POLYS];

	int num_polys;
public:
	void Reset();
	int Insert(kxPolygonList* polygonList);
	int Insert(kxPolygon* polygon);
	int Insert(kxRenderObject* object);

};
KX_END