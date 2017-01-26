#pragma once
#include "kxVector.h"
#include "kxPolygon.h"
#include "kxObject.h"
#include "kxCamera.h"

#define KX_OBJECT_MAX_VERTICES 1024
#define KX_OBJECT_MAX_POLYS     1024
#define RENDERLIST4DV1_MAX_POLYS          32768// 16384

#define TRANSFORM_LOCAL_ONLY       0  // perform the transformation in place on the
// local/world vertex list 
#define TRANSFORM_TRANS_ONLY       1  // perfrom the transformation in place on the 
// "transformed" vertex list

#define TRANSFORM_LOCAL_TO_TRANS   2  // perform the transformation to the local
// vertex list, but store the results in the
// transformed vertex list

namespace KevinX
{
	class kxRenderObject
	{
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

		kxVector4* vlist_local[KX_OBJECT_MAX_VERTICES];
		kxVector4* vlist_tran[KX_OBJECT_MAX_VERTICES];

		int  num_polys;
		kxPolygon* plist[KX_OBJECT_MAX_POLYS];
	};

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

	class kxRenderer
	{
	public:
		kxRenderer();
		//xyz rotation
		int buildMatrix(float thetaX, float thetaY, float thetaZ);
		int transform(int coord_select);
		int modelToWorld(kxVector4 world_pos,int coord_select);
		int worldToCamera();
		int cameraToPerspective();
		int perspectiveToScreen();
	public:
		//kxCamera mCamera;
		kxRenderList mRenderList;
		kxMatrix4 mRot;
	};
}