#pragma once
#include "kxGlobal.h"

KX_BEGIN
class kxColor
{
public:
	kxColor::kxColor()
	{
		this->setRGBA(0, 0, 0, 0);
	}
	inline int getRGBA()
	{
		return rgba;
	}
	inline void setRGBA(int _r, int _g, int _b, int _a)
	{
		r = _r;
		g = _g;
		b = _b;
		a = _a;
		rgba = (_a)+((_b) << 8) + ((_g) << 16) + ((_r) << 24);
	}
	inline void setRGBA(int _rgba)
	{
		r = (_rgba & 0xff000000) >> 24;
		g = (_rgba & 0x00ff0000) >> 16;
		b = (_rgba & 0x0000ff00) >> 8;
		a = (_rgba & 0x000000ff);
		rgba = _rgba;
	}
	inline int getRGB()
	{
		return (b)+((g) << 8) + ((r) << 16);
	}

	inline const UCHAR getRed()
	{
		return r;
	}
	void setRed(UCHAR _r)
	{
		r = _r;
		setRGBA(r, g, b, a);
	}

	inline UCHAR getBlue()
	{
		return b;
	}
	inline void setBlue(UCHAR _b)
	{
		b = _b;
		setRGBA(r, g, b, a);
	}

	inline UCHAR getGreen()
	{
		return g;
	}
	inline void setGreen(UCHAR _g)
	{
		g = _g;
		setRGBA(r, g, b, a);
	}

	inline UCHAR getAlpha()
	{
		return a;
	}
	inline void setAlpha(UCHAR _a)
	{
		a = _a;
		setRGBA(r, g, b, a);
	}
private:
	int rgba;
	UCHAR rgba_M[4];
	UCHAR r, g, b, a;
};
KX_END