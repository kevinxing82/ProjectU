#pragma once

#include "kxGlobal.h"
#include "kxColor.h"
#include "kxBitmap.h"

KX_BEGIN
class kxMaterial
{
public:
	int state;
	int id;
	char name[64];
	int attr;

	kxColor color;	 //材质颜色																																										   
	float  ka;		 //环境光反射系数
	float kd;		 //散色光反射系数
	float ks;		  //镜面反射光反射系数
	float  power;  //镜面反射指数

	kxColor ra, rd, rs;
	char texture_file[80];
	kxBitmap texture;
	int iaux1, iaux2;
	float faux1, faux2;
	void* ptr;
};
KX_END