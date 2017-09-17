#pragma once
#include "kxGlobal.h"
#include "kxVector.h"
#include "kxPolygon.h"
#include "kxObject.h"
#include "kxCamera.h"
#include "kxRenderList.h"
#include "kxParser.h"
#include "kxLight.h"

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

	void RestLights();
	int InitLight(int index,int _state,int _attr,
		kxColor _c_ambient,kxColor _c_diffuse,kxColor _c_specular,
		kxVector4* _pos, kxVector4* _dir,
		float _kc,float _kl,float _kq,float _spot_inner,float _spot_outer,float _pf);

	kxRenderList* renderList;
	kxCamera mCamera;
	kxMatrix44 mRot;

	kxLight lights[MAX_LIGHTS];
	int num_lights;
};
KX_END