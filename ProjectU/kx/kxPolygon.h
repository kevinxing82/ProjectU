#pragma once

#include "kxGlobal.h"
#include "kxVector.h"

KX_BEGIN
class kxPolygon
{
	int state;
	int attr;
	int color;

	kxVector4* vlist;
	int vert[3];
};

class kxPolygonList
{
	int state;
	int attr;
	int color;

	kxVector4 vlist[3];
	kxVector4 tlist[3];

	kxPolygon *next;
	kxPolygon *prev;
};
KX_END