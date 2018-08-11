#include "kxPolygon.h"

USING_KX;
kxPolygon::kxPolygon()
{
	this->color = new kxColor();
	this->lit_color = new kxColor();
}

kxPolygonList::kxPolygonList()
{
	for (int i = 0; i < 3; i++)
	{
		vlist[i] = new kxVertex();
		tlist[i] = new kxVertex();
	}
}
