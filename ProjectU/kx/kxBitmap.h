#pragma once
#include "kxGlobal.h"
KX_BEGIN
class kxBitmap
{
public:
	int state;
	int attr;
	int x, y;
	int width, height;
	int numBytes;
	int bpp;
	UCHAR* buffer;
};
KX_END