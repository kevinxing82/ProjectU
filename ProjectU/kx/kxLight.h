#pragma once

#include "kxGlobal.h"
#include "kxVector.h"
#include "kxColor.h"


KX_BEGIN
class kxLight
{
public:
	int state;
	int id;
	int attr;

	kxColor ambient;
	kxColor diffuse;
	kxColor specular;

	kxVector4 pos;
	kxVector4 dir;

	float kc, kl, kq;
	float spotInner;
	float spotOuter;
	float pf;

	int iaux1, iaux2;
	float faux1, faux2;
	void* ptr;
};
KX_END