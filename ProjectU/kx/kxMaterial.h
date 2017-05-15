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

	kxColor color;	 //������ɫ																																										   
	float  ka;		 //�����ⷴ��ϵ��
	float kd;		 //ɢɫ�ⷴ��ϵ��
	float ks;		  //���淴��ⷴ��ϵ��
	float  power;  //���淴��ָ��

	kxColor ra, rd, rs;
	char texture_file[80];
	kxBitmap texture;
	int iaux1, iaux2;
	float faux1, faux2;
	void* ptr;
};
KX_END