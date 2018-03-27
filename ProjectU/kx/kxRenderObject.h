#pragma once

#include "kxVector.h"
#include "kxGlobal.h"
#include "kxPolygon.h"
#include "kxMatrix.h"
#include "kxLight.h"

KX_BEGIN
class kxRenderObject
{
public:
	int Reset();
	void SetColor(int col);
	void ModelToWorld(int coordSelect = TRANSFORM_LOCAL_TO_TRANS);

	int id;										//物体的数字ID
	char name[64];						//物体的名称
	int state;									//物体的状态
	int attr;									    //物体的属性
	int mati;								   // 材质索引 -1表示没有材质

	//[OBJECT4DV2_MAX_FRAMES]
	float *avg_radius;
	float *max_radius;

	kxVector4 world_pos;
	kxVector4 dir;
	kxVector4 ux, uy, uz;

	int  num_vertices;
	int  num_frames;   //帧数

	int total_vertices;  //全部顶点
	int curr_frame;      //当前帧

	kxVertex* vlist_local;				//局部顶点数组
	kxVertex* vlist_tran;				    //变换后的顶点数组

	kxVertex* head_vlist_local;
	kxVertex* head_vlist_tran;

	kxVector2* tlist;   //3*最大多边形数
	kxBitmap* texture;

	int  num_polys;
	kxPolygon* plist;
};
KX_END
