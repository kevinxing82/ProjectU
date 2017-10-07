#include "kxDrawer.h"
#include <math.h>
#include <assert.h>
#include <iostream>

USING_KX
kxDrawer::kxDrawer()
{
}		   

kxDrawer::~kxDrawer()
{
	if (framebuffer)
	{
		delete(framebuffer);
	}
	framebuffer = NULL;
	zbuffer = NULL;
	texture = NULL;
}

int kxDrawer::Init(HWND hwnd, int width, int height)
{
	this->hwnd = hwnd;

	BITMAPINFO bi = { {sizeof(BITMAPINFOHEADER),width,-height,1,32,BI_RGB,width*height * 4,0,0,0,0} };
	LPVOID lptr;
	HDC	 hDC = GetDC(hwnd);
	screenHDC = CreateCompatibleDC(hDC);
	ReleaseDC(hwnd, hDC);

	screenHB = CreateDIBSection(screenHDC, &bi, DIB_RGB_COLORS, &lptr, 0, 0);
	if (screenHB == NULL)
	{
		return -3;
	}

	screenOB = (HBITMAP)SelectObject(screenHDC, screenHB);
	screenFB = (unsigned char*)lptr;
	screenPitch = width * 4;            
	memset(screenFB, 0, width*height * 4);


	float need = sizeof(void*)*(height * 2 + 1024) + width*height * 8 + 64;
	char *ptr = new char[need];
	char *framebuf, *zbuf;
	int j;
	assert(ptr);
	this->framebuffer = (IUINT32**)ptr;
	this->zbuffer = (float**)(ptr + sizeof(void*)*height);
	ptr += sizeof(void*)*height * 2;
	this->texture = (IUINT32**)ptr;
	ptr += sizeof(void*) * 1024;
	//framebuf = (char*)ptr;
	zbuf = (char*)ptr + width*height * 4;
	ptr += width*height * 8;
	framebuf = (char*)screenFB;

	for (j = 0; j < height; j++)
	{
		this->framebuffer[j] = (IUINT32*)(framebuf + width * 4 * j);
		this->zbuffer[j] = (float*)(zbuf + width * 4 * j);
	}

	this->texture[0] = (IUINT32*)ptr;
	this->texture[1] = (IUINT32*)(ptr + 16);
	memset(this->texture[0], 0, 64);
	this->tex_width = 2;
	this->tex_height = 2;
	this->max_u = 1.0f;
	this->max_v = 1.0f;
	this->width = width;
	this->height = height;
	this->background = 0x0c0c0;
	this->foreground = 0;
	render_state = RENDER_STATE_WIREFRAME;
	return E_NOTIMPL;
}

void kxDrawer::SetTexture(void * bits, long pitch, int w, int h)
{
	char* ptr = (char*)bits;
	int j;
	assert(w <= 1024 && h <= 1024);
	for (j = 0; j < h; ptr += pitch, j++)
	{
		texture[j] = (IUINT32*)ptr;
	}
	this->tex_width = w;
	this->tex_height = h;
	this->max_u = (float)(w - 1);
	this->max_v = (float)(h - 1);

}

void kxDrawer::Clear(int mode)
{
	int y, x, height = this->height;
	for (y = 0; y < this->height; y++)
	{
		IUINT32 *dst = this->framebuffer[y];
		IUINT32 cc = (height - 1 - y) * 230 / (height - 1);
		cc = (cc << 16) | (cc << 8) | cc;
		if (mode == 0)
		{
			cc = this->background;
		}
		for (x = this->width; x > 0; dst++, x--)
		{
			dst[0] = 0.0f;
		}
	}
}

void kxDrawer::SetPixel(int x, int y, IUINT32 color)
{
	if (((IUINT32)x)<(IUINT32)this->width&&((IUINT32)y)<(IUINT32)this->height)
	{
		this->framebuffer[y][x] = color;
	}
}

void kxDrawer::DrawLine(int x1, int y1, int x2, int y2, IUINT32 c)
{
	int x, y, rem = 0;
	if (x1 == x2&&y1 == y2)
	{
		SetPixel(x1, y1, c);
	}
	else if (x1 == x2)
	{
		int inc = (y1 <= y2) ? 1 : -1;
		for (y = y1; y != y2; y += inc)
		{
			SetPixel(x1, y, c);
		}
		SetPixel(x2, y2, c);
	}
	else if (y1 == y2)
	{
		int inc = (x1 <= x2) ? 1 : -1;
		for (x = x1; x != x2; x += inc)
		{
			SetPixel(x, y1, c);
		}
		SetPixel(x2, y2, c);
	}
	else
	{
		int dx = (x1 < x2) ? x2 - x1 : x1 - x2;
		int dy = (y1 < y2) ? y2 - y1 : y1 - y2;
		if (dx >= dy)
		{
			if (x2 < x1)x = x1, y = y1, x1 = x2, y1 = y2, x2 = x, y2 = y;
			for (x = x1, y = y1; x <= x2; x++)
			{
				SetPixel(x, y, c);
				rem += dy;
				if (rem >= dx)
				{
					rem -= dx;
					y += (y2 >= y1) ? 1 : -1;
					SetPixel(x, y, c);
				}
			}
			SetPixel(x2, y2, c);
		}
		else
		{
			if (y2 < y1)x = x1, y = y1, x1 = x2, y1 = y2, x2 = x, y2 = y;
			for (x = x, y = y1; y <= y2; y++)
			{
				SetPixel(x, y, c);
				rem += dx;
				if (rem >= dy)
				{
					rem -= dy;
					x += (x2 >= x1) ? 1 : -1;
					SetPixel(x, y, c);
				}
			}
			SetPixel(x2, y2, c);
		}
	}
}

void kxDrawer::DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, int color)
{
	float tmp_x;
	float tmp_y;
	float new_x;
	if( (FCMP(x1,x2)&&FCMP(x2,x3)) || (FCMP(y1,y2)&&FCMP(y2,y3)))
	{
		return;
	}

	if (y2 < y1)
	{
		SWAP(x1, x2, tmp_x);
		SWAP(y1, y2, tmp_y);
	}

	if (y3 < y1)
	{
		SWAP(x1, x3, tmp_x);
		SWAP(y1, y3, tmp_y);
	}

	if (y3 < y2)
	{
		SWAP(x2, x3, tmp_x);
		SWAP(y2, y3, tmp_y);
	}

	if (y3<min_clip_y || y1>max_clip_y ||
		(x1 < min_clip_x&&x2 < min_clip_x&&x3 < min_clip_x) ||
		(x1 > max_clip_x&&x2 > max_clip_x&&x3 > max_clip_x))
		return;

	if (FCMP(y1,y2))
	{
		DrawTopTriFP(x1, y1, x2, y2, x3, y3, color);
	}
	else if (FCMP(y2,y3))
	{
		DrawBottomTriFP(x1, y1, x2, y2, x3, y3, color);
	}
	else
	{
		new_x = x1 + (int)(0.5 + (float)(y2 - y1)*(float)(x3 - x1) / (float)(y3 - y1));
		DrawBottomTriFP(x1, y1, new_x, y2, x2, y2,color);
		DrawTopTriFP(x2, y2, new_x, y2, x3, y3, color);
	}
}

void kxDrawer::DrawTopTriFP(float x1, float y1, float x2, float y2, float x3, float y3, int color)
{
	int dx_right,
		dx_left,
		xs, xe,
		height;
	int temp_x,
		temp_y,
		right,
		left;

	int iy1, iy3, loop_y;

	if (x2 < x1)
	{
		SWAP(x1, x2, temp_x);
	}

	height = y3 - y1;

	dx_left = (x3 - x1) / height;
	dx_right = (x3 - x2) / height;

	xs =x1;
	xe =x2;

#if(RASTERIZER_MODE==RASTERIZER_ACCURATE)
	if (y1 < min_clip_y)
	{
		xs = xs + dx_left*(-y1 + min_clip_y);
		xe = xe + dx_right*(-y1 + min_clip_y);

		y1 = min_clip_y;

		iy1 = y1;
	}
	else
	{
		iy1 = ceil(y1);

		xs = xs + dx_left*(iy1 - y1);
		xe = xe + dx_right*(iy1 - y1);
	}

	if (y3 > max_clip_y)
	{
		y3 = max_clip_y;
		iy3 = y3 - 1;
	}
	else
	{
		iy3 = ceil(y3) - 1;
	}
#endif

#if((RASTERIZER_MODE==RASTERIZER_FAST)||(RASTERIZER_MODE==RASTERIZER_FASTEST))
	if (y1 < min_clip_y)
	{
		xs = xs + dx_left*(-y1 + min_clip_y);
		xe = xe + dx_right*(-y1 + min_clip_y);
	}
	
	if (y3 > max_clip_y)
		y3 = max_clip_y;

	iy1 = ceil(y1);
	iy3 = ceil(y3) - 1;
#endif

	if (x1 >= min_clip_x&&x1 <= max_clip_x&&
		x2 >= min_clip_x&&x2 <= max_clip_x&&
		x3 >= min_clip_x&&x3 <= max_clip_x)
	{
		for (loop_y = iy1; loop_y <= iy3; loop_y++)
		{
			DrawLine(xs, loop_y, xe, loop_y, color);
			xs += dx_left;
			xe += dx_right;
		}
	}
	else
	{
		for (loop_y = iy1; loop_y <= iy3; loop_y++)
		{
			left = xs;
			right = xe;

			xs += dx_left;
			xe += dx_right;

			if (left < min_clip_x)
			{
				left = min_clip_x;
				if (right < min_clip_x)
					continue;
			}

			if (right > max_clip_x)
			{
				right = max_clip_x;
				if (left > max_clip_x)
					continue;
			}
			DrawLine(xs, loop_y, xe, loop_y, color);
		}
	}																																														  
}

void kxDrawer::DrawBottomTriFP(float x1, float y1, float x2, float y2, float x3, float y3, int color)
{
	float dx_right,
		dx_left,
		xs, xe,
		height,
		temp_x,
		temp_y,
		right,
		left;
	int iy1, iy3, loop_y;

	if (x3 < x2)
	{
		SWAP(x2, x3, temp_x);
	}

	height = y3 - y1;

	dx_left = (x2 - x1) / height;
	dx_right = (x3 - x1) / height;

	xs = x1;
	xe = x1;

#if(RASTERIZER_MODE==RASTERIZER_ACCURATE)
	if (y1 < min_clip_y)
	{
		xs = xs + dx_left*(-y1 + min_clip_y);
		xe = xe + dx_right*(-y1 + min_clip_y);

		y1 = min_clip_y;
		iy1 = y1;
	}
	else
	{
		iy1 = ceil(y1);

		xs = xs + dx_left*(iy1 - y1);
		xe = xe + dx_right*(iy1 - y1);
	}
	if (y3 > max_clip_y)
	{
		y3 = max_clip_y;
		iy3 = y3 - 1;
	}
	else
	{
		iy3 = ceil(y3) - 1;
	}
#endif

#if((RASTERIZER_MODE==RASTERIZER_FAST)||(RASTERIZER_MODE==RASTERIZER_FASTEST))
	if (y1 < min_clip_y)
	{
		xs = xs + dx_left*(-y1 + min_clip_y);
		xe = xe + dx_right*(-y1 + min_clip_y);
	}

	if (y3 > max_clip_y)
		y3 = max_clip_y;

	iy1 = ceil(y1);
	iy3 = ceil(y3) - 1;
#endif

	if (x1 >= min_clip_x&&x1 <= max_clip_x&&
		x2 >= min_clip_x&&x2 <= max_clip_x&&
		x3 >= min_clip_x&&x3 <= max_clip_x)
	{
		for (loop_y = iy1; loop_y <= iy3; loop_y++)
		{
			DrawLine(xs, loop_y, xe, loop_y, color);
			xs += dx_left;
			xe += dx_right;
		}
	}
	else
	{
		for (loop_y = iy1; loop_y <= iy3; loop_y++)
		{
			left = xs;
			right = xe;

			xs += dx_left;
			xe += dx_right;

			if (left < min_clip_x)
			{
				left = min_clip_x;
				if (right < min_clip_x)
					continue;
			}

			if (right > max_clip_x)
			{
				right = max_clip_x;
				if (left > max_clip_x)
					continue;
			}
			DrawLine(xs, loop_y, xe, loop_y, color);
	   }
	}
}

void kxDrawer::Render(const kxRenderList & renderList)
{
	Clear(0);
	for (int poly = 0; poly < renderList.num_polys; poly++)
	{
		if (!(renderList.poly_ptrs[poly]->state&POLY4DV1_STATE_ACTIVE) ||
			(renderList.poly_ptrs[poly]->state&POLY4DV1_STATE_CLIPPED) ||
			(renderList.poly_ptrs[poly]->state&POLY4DV1_STATE_BACKFACE))
		{
			continue;
		}
		DrawTriangle(renderList.poly_ptrs[poly]->tlist[0].x, renderList.poly_ptrs[poly]->tlist[0].y,
			renderList.poly_ptrs[poly]->tlist[1].x, renderList.poly_ptrs[poly]->tlist[1].y ,
			renderList.poly_ptrs[poly]->tlist[2].x, renderList.poly_ptrs[poly]->tlist[2].y, renderList.poly_ptrs[poly]->color.getRGBA());
	}
	Update();
}

void kxDrawer::Update()
{
	HDC hDC = GetDC(this->hwnd);
	BitBlt(hDC, 0, 0, width, height, this->screenHDC, 0, 0, SRCCOPY);
	ReleaseDC(this->hwnd, hDC);
}
