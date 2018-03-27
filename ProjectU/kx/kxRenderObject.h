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

	int id;										//���������ID
	char name[64];						//���������
	int state;									//�����״̬
	int attr;									    //���������
	int mati;								   // �������� -1��ʾû�в���

	//[OBJECT4DV2_MAX_FRAMES]
	float *avg_radius;
	float *max_radius;

	kxVector4 world_pos;
	kxVector4 dir;
	kxVector4 ux, uy, uz;

	int  num_vertices;
	int  num_frames;   //֡��

	int total_vertices;  //ȫ������
	int curr_frame;      //��ǰ֡

	kxVertex* vlist_local;				//�ֲ���������
	kxVertex* vlist_tran;				    //�任��Ķ�������

	kxVertex* head_vlist_local;
	kxVertex* head_vlist_tran;

	kxVector2* tlist;   //3*���������
	kxBitmap* texture;

	int  num_polys;
	kxPolygon* plist;
};
KX_END
