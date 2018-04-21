#pragma once
#include "kxGlobal.h"
#include <d3d9.h>
#include "kxRenderList.h"
KX_BEGIN
class kxDrawer
{
public:
	kxDrawer();
	~kxDrawer();

	int Init(HWND hwnd, int width, int height);
	void SetTexture(void* bits, long pitch, int w, int h);
	void Clear(int mode);
	void SetPixel(int x, int y, IUINT32 color);
	void DrawLine(int x1, int y1, int x2, int y2, IUINT32 c);
	void DrawTriangle(float x1,float y1,float x2,float y2,float x3,float y3,int color);
	void DrawTopTriFP(float x1, float y1, float x2, float y2, float x3, float y3, int color);
	void DrawBottomTriFP(float x1, float y1, float x2, float y2, float x3, float y3, int color);
	void DrawGouraudTriangle(float arg_x0, float arg_y0, kxColor* color0,
		float arg_x1, float arg_y1, kxColor* color1,
		float arg_x2, float arg_y2, kxColor* color2 );
	void DrawTextureTriangle(float x0,float y0,float tu0,float tv0, 
		float x1,float y1,float tu1,float tv1,
		float x2,float y2,float tu2,float tv2,kxBitmap* textmap);
	void DrawTextureTriangleFS(float arg_x0, float arg_y0, float arg_tu0, float arg_tv0,
		float arg_x1, float arg_y1, float arg_tu1, float arg_tv1,
		float arg_x2, float arg_y2, float arg_tu2, float arg_tv2, kxColor* litColor, kxBitmap* textmap);
	void DrawBackground();
	void Render(const kxRenderList& renderList);

	void Update();

public:
	HWND hwnd;
	HDC screenHDC;
	HBITMAP screenHB;
	HBITMAP screenOB;
	unsigned char *screenFB;
	long screenPitch = 0;

	int min_clip_x = 0;
	int max_clip_x = (WIN_WIDTH - 1);
	int min_clip_y = 0;
	int max_clip_y = (WIN_HEIGHT - 1);

	//transform_t transform;      // ����任��
	int width;                  // ���ڿ��
	int height;                 // ���ڸ߶�
	IUINT32 **framebuffer;      // ���ػ��棺framebuffer[y] ����� y��
	float **zbuffer;            // ��Ȼ��棺zbuffer[y] Ϊ�� y��ָ��
	IUINT32 **texture;          // ����ͬ����ÿ������
	int tex_width;              // ������
	int tex_height;             // ����߶�
	float max_u;                // ��������ȣ�tex_width - 1
	float max_v;                // �������߶ȣ�tex_height - 1
	int render_state;           // ��Ⱦ״̬
	IUINT32 background;         // ������ɫ
	IUINT32 foreground;         // �߿���ɫ

};
KX_END

