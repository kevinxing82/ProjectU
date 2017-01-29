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
	kxRenderer();
	//xyz rotation
	int buildMatrix(float thetaX, float thetaY, float thetaZ);
	int transform(int coord_select);
	int modelToWorld(kxVector4 world_pos, int coord_select);
	int worldToCamera();
	int cameraToPerspective();
	int perspectiveToScreen();
public:
	kxRenderList renderList;
	kxCamera mCamera;
	kxMatrix4 mRot;
};
KX_END