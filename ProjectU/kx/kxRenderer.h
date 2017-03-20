#pragma once
#include "kxGlobal.h"
#include "kxVector.h"
#include "kxPolygon.h"
#include "kxObject.h"
#include "kxCamera.h"
#include "kxRenderList.h"
#include "kxParser.h"

KX_BEGIN
class kxRenderer
{
public:
	void init();
	//xyz rotation
	int buildMatrix(float thetaX, float thetaY, float thetaZ);
	void transform(kxRenderObject* obj, int coord_select,int transform_basis);
	int transform(int coord_select);
	int worldToCamera();
	int cameraToPerspective();
	int perspectiveToScreen();
	int CullObject(kxRenderObject* obj, int cullFlag);
	void RemoveBackfaces();

	kxRenderList* renderList;
	kxCamera mCamera;
	kxMatrix44 mRot;
};
KX_END