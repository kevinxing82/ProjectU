#pragma once

#include "kxGlobal.h"
#include "kxVector.h"
#include "kxColor.h"
#include "kxVertex.h"
#include "kxBitmap.h"

KX_BEGIN
class kxPolygon
{
public:
	int state;								   //״̬��Ϣ
	int attr;									   //����ε���������
	kxColor color;						   //����ε���ɫ
	kxColor orgColor;
	kxColor renderColor;
	kxColor* lit_color;				   //���ڴ洢���պ����ɫ
	                                               //���ں㶨��ɫ���������ɫ�洢�ڵ�һ��Ԫ����
	                                               //����Gourand��ɫ��������ɫ�ֱ��������Ԫ����
	kxBitmap* texture;				   //ָ��������Ϣ��ָ�룬���ڼ�����ӳ��
	int mati;								   // -1 ��ʾû������

	kxVertex* vlist;					       //�����εĶ���
	kxVector2* tlist;					   //���������б�

	int vert[3];							  //ָ�򶥵��б������
	int text[3];							 //ָ�����������б������
	float nlength;						   //���߳���
 };

class kxPolygonList
{
public:
	int state;
	int attr;
	kxColor color;
	kxColor* lit_color;				   //���ڴ洢���պ����ɫ
									               //���ں㶨��ɫ���������ɫ�洢�ڵ�һ��Ԫ����
									               //����Gourand��ɫ��������ɫ�ֱ��������Ԫ����
	kxBitmap* texture;				   //ָ��������Ϣ��ָ�룬���ڼ�����ӳ��
	int mati;								   // -1 ��ʾû������

	float nlength;						   //���߳���
	kxVector4* normal;                //����η���

	float avg_z;                             //ƽ��zֵ�����ڼ�����

	kxVector4 vlist[3];
	kxVector4 tlist[3];

	kxPolygonList *next;
	kxPolygonList *prev;
};
KX_END