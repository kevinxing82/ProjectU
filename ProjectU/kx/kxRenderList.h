#pragma once

#include "kxGlobal.h"
#include "kxPolygon.h"
#include "kxRenderObject.h"
//#include <vector>

KX_BEGIN
using namespace std;
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
	int modelToWorld(const kxVector4& world_pos, int coord_select = TRANSFORM_LOCAL_TO_TRANS);

	int Insert(kxPolygonList* polygonList);
	int Insert(kxPolygon* polygon);
	int Insert(kxRenderObject* object,int insert_local=TRANSFORM_LOCAL_ONLY);
	int lightWorld(kxLight* lights, int maxLights);
	void Sort(int method);

public:
	int  static __cdecl CompareAvgZ(const void* arg1, const void* arg2);
	int static __cdecl CompareNearZ(const void* arg1, const void* arg2);
	int static __cdecl CompareFarZ(const void* arg1, const void* arg2);
};
KX_END