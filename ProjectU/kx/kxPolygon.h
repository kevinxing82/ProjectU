#pragma once

#include "kxVector.h"

namespace KevinX
{
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
}