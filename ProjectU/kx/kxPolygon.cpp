#include "kxPolygon.h"

USING_KX;
kxPolygon::kxPolygon()
{
	this->color = new kxColor();
	this->lit_color[0] = new kxColor();
	this->lit_color[1] = new kxColor();
	this->lit_color[2] = new kxColor();

}

kxPolygonList::kxPolygonList()
{
	this->lit_color[0] = new kxColor();
	this->lit_color[1] = new kxColor();
	this->lit_color[2] = new kxColor();
	for (int i = 0; i < 3; i++)
	{
		vlist[i] = new kxVertex();
		tlist[i] = new kxVertex();
	}
}
