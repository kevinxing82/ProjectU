#pragma once

#include "kxGlobal.h"
#include "kxVector.h"
#include "kxColor.h"

KX_BEGIN
class kxPolygon
{
public:
	int state;
	int attr;
	kxColor color;
	kxColor orgColor;
	kxColor renderColor;

	kxVector4* vlist;
	int vert[3];
};

class kxPolygonList
{
public:
	int state;
	int attr;
	kxColor color;

	kxVector4 vlist[3];
	kxVector4 tlist[3];

	kxPolygonList *next;
	kxPolygonList *prev;
};
KX_END