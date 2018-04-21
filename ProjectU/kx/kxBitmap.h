#pragma once
#include "kxGlobal.h"
#include <Windows.h>
KX_BEGIN
class kxBitmapData
{
public:
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeadr;
	PALETTEENTRY           palette[256];
	UCHAR                       *buffer;
};

class kxBitmap
{
public:
	int state;
	int attr;
	int x, y;
	int width, height;
	int numBytes;
	int bpp;
	kxBitmapData* bitmapData;
public:
	int load(char* filename);
	int unload();
	int  flip();
	int copy(kxBitmap* dest,int dest_x,int dest_y,int source_x,int source_y,int width,int height);
};
KX_END