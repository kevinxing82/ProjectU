#pragma once

#include "kxGlobal.h"
#include "kxVector.h"

KX_BEGIN
class kxParmLine2D
{
	kxVector2 p0;
	kxVector2 p1;
	kxVector2 v;
};

class kxParmLine3D
{
	kxVector3 p0;
	kxVector3 p1;
	kxVector3 v;
};

class kxPlane
{
	kxVector3 p0;
	kxVector3 n;
};
KX_END