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
	int state;								   //状态信息
	int attr;									   //多边形的物理属性
	kxColor color;						   //多边形的颜色
	kxColor orgColor;
	kxColor renderColor;
	kxColor* lit_color;				   //用于存储光照后的颜色
	                                               //对于恒定着色，多边形颜色存储在第一个元素中
	                                               //对于Gourand着色，顶点颜色分别存在三个元素中
	kxBitmap* texture;				   //指向纹理信息的指针，用于简单纹理映射
	int mati;								   // -1 表示没有纹理

	kxVertex* vlist;					       //三角形的顶点
	kxVector2* tlist;					   //纹理坐标列表

	int vert[3];							  //指向顶点列表的索引
	int text[3];							 //指向纹理坐标列表的索引
	float nlength;						   //法线长度
 };

class kxPolygonList
{
public:
	int state;
	int attr;
	kxColor color;
	kxColor* lit_color;				   //用于存储光照后的颜色
									               //对于恒定着色，多边形颜色存储在第一个元素中
									               //对于Gourand着色，顶点颜色分别存在三个元素中
	kxBitmap* texture;				   //指向纹理信息的指针，用于简单纹理映射
	int mati;								   // -1 表示没有纹理

	float nlength;						   //法线长度
	kxVector4* normal;                //多边形法线

	float avg_z;                             //平均z值，用于简单排序

	kxVector4 vlist[3];
	kxVector4 tlist[3];

	kxPolygonList *next;
	kxPolygonList *prev;
};
KX_END