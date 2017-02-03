#pragma once

#include "kxGlobal.h"
#include "kxPolygon.h"
#include "kxRenderObject.h"
//#include <vector>

KX_BEGIN
//using namespace std;
class kxRenderList
{
public:
	int state;
	int attr;

	//vector<kxPolygonList*>	poly_ptrs;
	kxPolygonList* poly_ptrs[RENDERLIST4DV1_MAX_POLYS];
	//vector<kxPolygonList> poly_data;
	kxPolygonList  poly_data[RENDERLIST4DV1_MAX_POLYS];

	int num_polys;
public:
	void Reset();
	int Insert(kxPolygonList* polygonList);
	int Insert(kxPolygon* polygon);
	int Insert(kxRenderObject* object,int insert_local);

};
KX_END