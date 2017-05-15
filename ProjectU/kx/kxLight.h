#pragma once

#include "kxGlobal.h"
#include "kxVector.h"
KX_BEGIN
class kxLight
{
	int state;
	int id;
	int attr;

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